#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"   // for logRefresher
#include "eve_api/eve_api.h"
#include "qmlevemon_app.h"
#include "db/db.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

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
    bool success = false;
    if (locationType == QLatin1String("station")) {
        QJsonObject jstation;
        if (m_api->get_universe_station(jstation, locationId)) {
            loc = EveLocation::fromJson(jstation);
            success = true;
        }
    } else if (locationType == QLatin1String("structure")) {
        QJsonObject jstructure;
        if (m_api->get_universe_structure(jstructure, locationId, accessToken)) {
            loc = EveLocation::fromJson(jstructure);
            success = true;
        }
    }
    if (success) {
        // forcefully set location id/type, because factory does not do that
        loc.setLocationId(locationId);
        loc.setType(locationType);
    }
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
    Db *db = globalAppDatabaseInstance();
    if (db) {
        const QJsonObject locationObj = db->loadCachedLocation(locationId);
        if (!locationObj.isEmpty()) {
            // loaded from cache OK
            EveLocation loc = EveLocation::fromJson(locationObj);
            loc.setLocationId(locationId);
            loc.setType(locationType);
            return loc;
        }
    }
    // if not in cache, send request
    const EveLocation loc = send_location_request(locationId, locationType, accessToken);
    // save in cache, only if network request was successful
    //   prevent empty locations to get into the cache :(
    //if ((loc.typeId() != 0) && (!loc.name().isEmpty()) && (loc.solarSystemId() != 0)) {
    if (!loc.isEmpty()) {
        if (db) {
            db->saveCachedLocation(locationId, loc.toJson());
        }
    }
    return loc;
}

EveLocation PeriodicalRefresherWorker::resolve_location_guess_type(quint64 locationId, const QByteArray &accessToken)
{
    // first try to query from cache
    Db *db = globalAppDatabaseInstance();
    if (db) {
        const QJsonObject locationObj = db->loadCachedLocation(locationId);
        if (!locationObj.isEmpty()) {
            // loaded from cache OK
            EveLocation loc = EveLocation::fromJson(locationObj);
            loc.setLocationId(locationId);
            // check that it has type filled in
            if (!loc.type().isEmpty()) {
                return loc;
            }
            // else, we need to make a new request anyway
            // also, clear Db from previously incorrectly saved entry
            db->deleteCachedLocation(locationId);
            // so it will be refreshed from network again
        }
    }

    QJsonArray replyArr;
    if (m_api->post_universe_names(replyArr, {locationId})) {
        QString found_type;
        for (const QJsonValue &jval: replyArr) {
            const QJsonObject jobj = jval.toObject();
            const QString value_category = jobj.value(QLatin1String("category")).toString();
            const quint64 value_id       = jobj.value(QLatin1String("id")).toVariant().toULongLong();
            const QString value_name     = jobj.value(QLatin1String("name")).toString();
            if (value_id == locationId) {
                found_type = value_category; // "station" / "structure" ?
            }
        }
        if (!found_type.isEmpty()) {
            // now we know the type, resolve location details
            return resolve_location(locationId, found_type, accessToken);
        } else {
            // fail :(
            qCWarning(logRefresher) << "resolve_location_guess_type(): We failed to resolve a location id: "
                                    << locationId << ": request OK, but not found.";
            return EveLocation();
        }
    } else {
        // fail :(
        qCWarning(logRefresher) << "resolve_location_guess_type(): We failed to resolve a location id: "
                                << locationId << ": request failed!";
        return EveLocation();
    }
}


/**
 * First tries to load cached data;
 * If not in cache, requests from ESI, saves into cache.
 * @brief PeriodicalRefresherWorker::resolve_character_name
 * @param charId
 * @return character name, or empty string on fail
 */
QString PeriodicalRefresherWorker::resolve_character_name(quint64 charId)
{
    QString charName;

    Db *db = globalAppDatabaseInstance();
    if (db) {
        charName = db->findCachedCharacterName(charId);
    }

    if (charName.isEmpty()) {
        QJsonArray jarr;
        if (m_api->post_universe_names(jarr, {charId})) {
            for (const QJsonValue &jval: jarr) {
                const QJsonObject jobj = jval.toObject();

                const QString value_category = jobj.value(QLatin1String("category")).toString();
                const quint64 value_id       = jobj.value(QLatin1String("id")).toVariant().toULongLong();
                const QString value_name     = jobj.value(QLatin1String("name")).toString();

                if ((value_id == charId) && (value_category == QLatin1String("character"))) {
                    charName = value_name;
                    break;
                }
            }

            if (!charName.isEmpty() && db) {
                db->saveCachedCharacterName(charId, charName);
            }
        }
    }

    return charName;
}



/**
 * First tries to load cached data;
 * If not in cache, requests from ESI, saves into cache.
 * @brief PeriodicalRefresherWorker::resolve_corporation_name
 * @param corpId
 * @return corporation name, or empty string on fail
 */
QString PeriodicalRefresherWorker::resolve_corporation_name(quint64 corpId)
{
    QString corpName;

    Db *db = globalAppDatabaseInstance();
    if (db) {
        corpName = db->findCachedCorporationName(corpId);
    }

    if (corpName.isEmpty()) {
        QJsonArray jarr;
        if (m_api->post_universe_names(jarr, {corpId})) {
            for (const QJsonValue &jval: jarr) {
                const QJsonObject jobj = jval.toObject();

                const QString value_category = jobj.value(QLatin1String("category")).toString();
                const quint64 value_id       = jobj.value(QLatin1String("id")).toVariant().toULongLong();
                const QString value_name     = jobj.value(QLatin1String("name")).toString();

                if ((value_id == corpId) && (value_category == QLatin1String("corporation"))) {
                    corpName = value_name;
                    break;
                }
            }

            if (!corpName.isEmpty() && db) {
                db->saveCachedCorporationName(corpId, corpName);
            }
        }
    }

    return corpName;
}


/**
 * First tries to load cached data;
 * If not in cache, requests from ESI, saves into cache.
 * @brief PeriodicalRefresherWorker::resolve_alliance_name
 * @param allyId
 * @return alliance name, or empty string on fail
 */
QString PeriodicalRefresherWorker::resolve_alliance_name(quint64 allyId)
{
    QString allyName;

    Db *db = globalAppDatabaseInstance();
    if (db) {
        allyName = db->findCachedAllianceName(allyId);
    }

    if (allyName.isEmpty()) {
        QJsonArray jarr;
        if (m_api->post_universe_names(jarr, {allyId})) {
            for (const QJsonValue &jval: jarr) {
                const QJsonObject jobj = jval.toObject();

                const QString value_category = jobj.value(QLatin1String("category")).toString();
                const quint64 value_id       = jobj.value(QLatin1String("id")).toVariant().toULongLong();
                const QString value_name     = jobj.value(QLatin1String("name")).toString();

                if ((value_id == allyId) && (value_category == QLatin1String("alliance"))) {
                    allyName = value_name;
                    break;
                }
            }

            if (!allyName.isEmpty() && db) {
                db->saveCachedAllianceName(allyId, allyName);
            }
        }
    }

    return allyName;
}


} // namespace EM
