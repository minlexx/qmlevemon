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
    Db *db = globalAppDatabaseInstance();
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
    // if not in cache, send request
    const EveLocation loc = send_location_request(locationId, locationType, accessToken);
    // save in cache, only if network request was successful
    //   prevent empty locations to get into the cache :(
    if ((loc.typeId() != 0) && (!loc.name().isEmpty()) && (loc.solarSystemId() != 0)) {
        if (db) {
            db->saveCachedLocation(locationId, loc.toJson());
        }
    }
    return loc;
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
        if (m_api->get_corporations_names(jarr, {corpId})) {
            for (const QJsonValue &jval: jarr) {
                const QJsonObject jobj = jval.toObject();

                const quint64 value_id   = jobj.value(QLatin1String("corporation_id")).toVariant().toULongLong();
                const QString value_name = jobj.value(QLatin1String("corporation_name")).toString();

                if (value_id == corpId) {
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
        if (m_api->get_characters_names(jarr, {charId})) {
            for (const QJsonValue &jval: jarr) {
                const QJsonObject jobj = jval.toObject();

                const quint64 value_id   = jobj.value(QLatin1String("character_id")).toVariant().toULongLong();
                const QString value_name = jobj.value(QLatin1String("character_name")).toString();

                if (value_id == charId) {
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
        if (m_api->get_alliances_names(jarr, {allyId})) {
            for (const QJsonValue &jval: jarr) {
                const QJsonObject jobj = jval.toObject();

                const quint64 value_id   = jobj.value(QLatin1String("alliance_id")).toVariant().toULongLong();
                const QString value_name = jobj.value(QLatin1String("alliance_name")).toString();

                if (value_id == allyId) {
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
