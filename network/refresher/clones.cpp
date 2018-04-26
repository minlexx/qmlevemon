#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"
#include "models/character.h"
#include "models/character_implants_group.h"
#include "db/db.h"
#include "qmlevemon_app.h"

namespace EM {


/**
 * Depending on locationType sends proepr request to ESI API
 * and parses the result.
 * @brief PeriodicalRefresherWorker::send_location_request
 * @param locationId - quint64 location id from ESI
 * @param locationType - "station", "structure" as in ESI
 * @param accessToken - Character access token to query structures from ESI
 * @return EveLocation object
 */
EveLocation PeriodicalRefresherWorker::send_location_request(quint64 locationId, const QString &locationType, const QByteArray &accessToken)
{
    EveLocation loc;
    if (locationType == QLatin1String("station")) {
        QJsonObject jstation;
        if (m_api->get_universe_station(jstation, locationId)) {
            loc = EveLocation::fromJson(jstation);
        }
    } else if (locationType == QLatin1String("structure")) {
        QJsonObject jstructure;
        if (m_api->get_universe_structure(jstructure, locationId, accessToken)) {
            loc = EveLocation::fromJson(jstructure);
        }
    }
    // forcefully set location id/type, because factory does not do that
    loc.setLocationId(locationId);
    loc.setType(locationType);
    return loc;
}


/**
 * First tries to load cached location;
 * If not in cache, requests from ESI, saves into cache.
 * @brief PeriodicalRefresherWorker::resolve_location
 * @param locationId - quint64 id of location
 * @param locationType - "station", "structure" as in ESI
 * @param accessToken - Character access token to query structures from ESI
 * @return EveLocation object
 */
EveLocation PeriodicalRefresherWorker::resolve_location(quint64 locationId, const QString &locationType, const QByteArray &accessToken)
{
    // try to load from cache
    QmlEvemonApp *gApp = globalAppInstance();
    if (gApp) {
        Db *db = gApp->database();
        if (db) {
            QJsonObject locationObj = db->loadCachedLocation(locationId);
            if (!locationObj.isEmpty()) {
                // loaded from cache OK
                EveLocation loc = EveLocation::fromJson(locationObj);
                loc.setLocationId(locationId);
                loc.setType(locationType);
                return loc;
            }
        }
    }
    // if not in cache, send request
    const EveLocation loc = send_location_request(locationId, locationType, accessToken);
    // save in cache, only if network request was successful
    //   prevent empty locations to get into the cache :(
    if (gApp && (loc.typeId() != 0) && (!loc.name().isEmpty()) && (loc.solarSystemId() != 0)) {
        Db *db = gApp->database();
        if (db) {
            db->saveCachedLocation(locationId, loc.toJson());
        }
    }
    return loc;
}


int PeriodicalRefresherWorker::resresh_clones(Character *ch)
{
    if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::CLONES)) {
        qCDebug(logRefresher) << " no need to refresh clones for" << ch->toString();
        return 0;  // no update needed, too early
    }
    // check if tokens needs refresing
    if (!this->check_refresh_token(ch)) {
        return 0;
    }

    QmlEvemonApp *gApp = globalAppInstance();
    if (!gApp) return 0;
    Db *db = gApp->database();
    if (!db) return 0;

    int nChanges = 0;

    qCDebug(logRefresher) << " refreshing clones for" << ch->toString();
    QJsonObject reply;

    // 1. refresh clones
    if (m_api->get_character_clones(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
        // example response:
        //      {
        //        "jump_clones": [
        //          {
        //            "jump_clone_id": 37293234,
        //            "location_id": 60003466,
        //            "location_type": "station",
        //            "implants": [
        //              10212,
        //              10216,
        //              13258,
        //              13259,
        //              19684
        //            ],
        //            "name": "Active armor"
        //          },
        //          {
        //            "jump_clone_id": 36981517,
        //            "location_id": 1021551599501,
        //            "location_type": "structure",
        //            "implants": [],
        //            "name": ""
        //          }
        //        ],
        //        "last_clone_jump_date": "2018-03-08T18:43:04Z",
        //        "home_location": {
        //          "location_id": 60003916,
        //          "location_type": "station"
        //        }
        //      }

        const QDateTime dtLastCloneJump = reply.value(QLatin1String("last_clone_jump_date")).toVariant().toDateTime();
        const QJsonObject homeLocationObject = reply.value(QLatin1String("home_location")).toObject();
        const QJsonArray jjump_clones = reply.value(QLatin1String("jump_clones")).toArray();
        // parse home location
        if (!homeLocationObject.isEmpty()) {
            const quint64 locationId = homeLocationObject.value(QLatin1String("location_id")).toVariant().toULongLong();
            const QString locationType = homeLocationObject.value(QLatin1String("location_type")).toString();

            // check if character's home station has changed;
            if (ch->currentClone()->location()->locationId() != locationId) {
                // need load info about new location
                const EveLocation loc = resolve_location(locationId, locationType, ch->getAuthTokens().access_token);
                qCDebug(logRefresher) << "location info:" << loc;
                ch->setHomeLocation(loc);
            }
        }

        ch->setLastCloneJumpDate(dtLastCloneJump);

        // parse jump clones
        QVector<CharacterClone> newClones;
        for (const QJsonValue &jval: jjump_clones) {
            const QJsonObject jobj = jval.toObject();
            const QString cloneName = jobj.value(QLatin1String("name")).toString();
            const quint64 jumpCloneId = jobj.value(QLatin1String("jump_clone_id")).toVariant().toULongLong();
            const quint64 locationId = jobj.value(QLatin1String("location_id")).toVariant().toULongLong();
            const QString locationType = jobj.value(QLatin1String("location_type")).toString();
            const QJsonArray jimplants = jobj.value(QLatin1String("implants")).toArray();

            // first, load implants
            CharacterImplantsGroup newCloneImplants;
            for (const QJsonValue& jval: jimplants) {
                const quint64 implantTypeID = jval.toVariant().toULongLong();
                if (implantTypeID > 0) {
                    // load type info from database
                    const QJsonObject dbJson = db->typeInfo(implantTypeID);
                    const QJsonArray dbJsonAttrs = db->typeAttributes(implantTypeID);
                    // cosntruct implant inventory item
                    const InvType implantItem = InvType::fromDatabaseJson(dbJson, dbJsonAttrs);
                    // store it in implant set
                    newCloneImplants.addImplant(std::move(implantItem));
                }
            }

            // load location
            const EveLocation loc = resolve_location(locationId, locationType, ch->getAuthTokens().access_token);

            // construct clone object
            CharacterClone newClone;
            newClone.setCloneId(jumpCloneId);
            newClone.setCloneName(cloneName);
            newClone.setLocation(loc);
            newClone.setImplantsGroup(newCloneImplants);

            newClones.append(std::move(newClone));
        }

        ch->setClones(newClones);

        nChanges++;
    }

    // if (QThread::currentThread()->isInterruptionRequested()) break; // break early

    // 2, refresh implants
    QJsonArray replyArr;
    if (m_api->get_character_implants(replyArr, ch->characterId(), ch->getAuthTokens().access_token)) {
        // example response : [ 22107, 22108, 22111, 22109, 22110, 13229, 13249 ]
        CharacterImplantsGroup currentCloneImps;
        for (const QJsonValue& jval: replyArr) {
            const quint64 implantTypeID = jval.toVariant().toULongLong();
            if (implantTypeID > 0) {
                // load type info from database
                const QJsonObject dbJson = db->typeInfo(implantTypeID);
                const QJsonArray dbJsonAttrs = db->typeAttributes(implantTypeID);
                // cosntruct implant inventory item
                const InvType implantItem = InvType::fromDatabaseJson(dbJson, dbJsonAttrs);
                // store it in implant set
                currentCloneImps.addImplant(std::move(implantItem));
            }
        }

        ch->currentClone()->setImplantsGroup(std::move(currentCloneImps));

        nChanges++;
    }

    // example request into eve_sde db:
    // SELECT it.typeName, ta.*, at.attributeName, at.description
    //  FROM dgmtypeattributes ta, dgmAttributeTypes at, invTypes it
    //  WHERE ta.typeid=13229 AND at.attributeid=ta.attributeid AND ta.typeID=it.typeID;

    if (nChanges > 0) {
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::CLONES);
    }
    return nChanges; // >0 : there was an update
}


int PeriodicalRefresherWorker::refresh_jump_fatigue(Character *ch)
{
    int nChanges = 0;
    if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::FATIGUE)) {
        qCDebug(logRefresher) << " no need to refresh jump fatigue for" << ch->toString();
        return nChanges;  // no update needed, too early
    }
    // check if tokens needs refresing
    if (!this->check_refresh_token(ch)) {
        return nChanges;
    }

    qCDebug(logRefresher) << " refreshing jump fatigue for" << ch->toString();
    QJsonObject reply;

    if (m_api->get_character_fatigue(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
        // {
        //   "jump_fatigue_expire_date": "2017-01-07T19:17:51Z",
        //   "last_jump_date": "2017-01-07T18:21:31Z",
        //   "last_update_date": "2017-01-07T18:27:09Z"
        // }
        QDateTime jump_fatigue_expire_date = reply.value(QLatin1String("jump_fatigue_expire_date")).toVariant().toDateTime();
        QDateTime last_jump_date = reply.value(QLatin1String("last_jump_date")).toVariant().toDateTime();
        if (jump_fatigue_expire_date.isValid()) {
            ch->setJumpFatigueExpireDate(jump_fatigue_expire_date);
            nChanges++;
        }
        if (last_jump_date.isValid()) {
            ch->setLastJumpDate(last_jump_date);
            nChanges++;
        }
    }

    if (nChanges > 0) {
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::FATIGUE);
    }
    return nChanges; // >0 : there was an update
}


} // namespace EM
