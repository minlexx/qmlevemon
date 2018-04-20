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
        qCDebug(logRefresher) << reply;
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

        QDateTime dtLastCloneJump = reply.value(QLatin1String("last_clone_jump_date")).toVariant().toDateTime();
        QJsonObject homeLocationObject = reply.value(QLatin1String("home_location")).toObject();
        QJsonArray jjump_clones = reply.value(QLatin1String("jump_clones")).toArray();
        // parse home location
        if (!homeLocationObject.isEmpty()) {
            quint64 locationId = homeLocationObject.value(QLatin1String("location_id")).toVariant().toULongLong();
            QString locationType = homeLocationObject.value(QLatin1String("location_type")).toString();

            // check if character's home station has changed;
            // if not, do not send new ESI requests
            if (ch->currentClone()->location()->locationId() != locationId) {
                // need to query ESI about new location
                EveLocation loc;
                if (locationType == QLatin1String("station")) {
                    QJsonObject jstation;
                    if (m_api->get_universe_station(jstation, locationId)) {
                        qCDebug(logRefresher) << "station info:" << jstation;
                        loc = EveLocation::fromESIUniverseJson(jstation);

                    }
                } else if (locationType == QLatin1String("structure")) {
                    QJsonObject jstructure;
                    if (m_api->get_universe_structure(jstructure, locationId, ch->getAuthTokens().access_token)) {
                        qCDebug(logRefresher) << "structure info:" << jstructure;
                        loc = EveLocation::fromESIUniverseJson(jstructure);
                    }
                }
                // forcefully set location id/type, because factory does not do that
                loc.setLocationId(locationId);
                loc.setType(locationType);
                qCDebug(logRefresher) << "location info:" << loc;
                ch->setHomeLocation(loc);
            }
        }

        ch->setLastCloneJumpDate(dtLastCloneJump);

        // parse jump clones
        for (const QJsonValue &jval: jjump_clones) {
            const QJsonObject jobj = jval.toObject();
            QString name = jobj.value(QLatin1String("name")).toString();
            quint64 jumpCloneId = jobj.value(QLatin1String("jump_clone_id")).toVariant().toULongLong();
            quint64 locationId = jobj.value(QLatin1String("location_id")).toVariant().toULongLong();
            QString locationType = jobj.value(QLatin1String("location_type")).toString();
            QJsonArray implants = jobj.value(QLatin1String("implants")).toArray();

            bool needs_update = true;
            //const CharacterClone *existingClone = ch->findCloneById(jumpCloneId);
            //if (existingClone) {
            //    if (existingClone->location()->locationId() == locationId) {
            //        needs_update = false;
            //    }
            //}

            if (!needs_update) {
                // this clone is already known in chafacter, and is located
                // in the same location. No need to send extra requests
                continue;
            }
        }

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


} // namespace EM
