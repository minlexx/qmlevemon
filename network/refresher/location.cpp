#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"
#include "models/character.h"
#include "db/db.h"
#include "qmlevemon_app.h"


namespace EM {


int PeriodicalRefresherWorker::refresh_location(Character &ch) {
    if (!ch.updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::LOCATION)) {
        qCDebug(logRefresher) << " no need to refresh location for" << ch.toString();
        return 0;  // no update needed, too early
    }
    // check if tokens needs refresing
    if (!this->check_refresh_token(ch)) {
        return 0;
    }
    qCDebug(logRefresher) << " refreshing location for" << ch.toString();
    QJsonObject reply;

     // get current location

    if (!m_api->get_character_location(reply, ch.characterId(), ch.getAuthTokens().access_token)) {
        qCWarning(logRefresher) << "Get character location failed!";
        return 0; // req failed
    }
    if (QThread::currentThread()->isInterruptionRequested()) return 0;

    // qCDebug(logRefresher) << reply;
    // ^^ QJsonObject({"solar_system_id":31001668,"structure_id":1021551599501})

    quint64 ss_id = 0, station_id = 0, structure_id = 0, prev_station_id = 0, prev_structure_id = 0;
    ss_id = reply.value(QLatin1String("solar_system_id")).toVariant().toULongLong();
    if (reply.contains(QLatin1String("structure_id"))) {
        structure_id = reply.value(QLatin1String("structure_id")).toVariant().toULongLong();
    }
    if (reply.contains(QLatin1String("station_id"))) {
        station_id = reply.value(QLatin1String("station_id")).toVariant().toULongLong();
    }

    prev_station_id = ch.currentStationId();
    prev_structure_id = ch.currentStructureId();
    ch.setCurrentStructureId(structure_id);
    ch.setCurrentStationId(station_id);

    if ((structure_id > 0) && (prev_structure_id != structure_id)) {
        // resolve structure name
        if (m_api->get_universe_structure(reply, structure_id, ch.getAuthTokens().access_token)) {
            if (QThread::currentThread()->isInterruptionRequested()) return 0;
            QString structure_name = reply.value(QLatin1String("name")).toString();
            ch.setCurrentStructureName(structure_name);
        } else {
            qCWarning(logRefresher) << "Resolve structure name failed!";
        }
    } else if ((station_id > 0) && (prev_station_id != station_id)) {
        // resolve station name
        if (m_api->get_universe_station(reply, station_id)) {
            if (QThread::currentThread()->isInterruptionRequested()) return 0;
            QString station_name = reply.value(QLatin1String("name")).toString();
            ch.setCurrentStructureName(station_name);
        } else {
            qCWarning(logRefresher) << "Resolve station name failed!";
        }
    }
    if (QThread::currentThread()->isInterruptionRequested()) return 0;

    // deal with results
    // *IF* the solarsystem *really* changed:
    if (ch.currentSolarSystemId() != ss_id) {
        ch.setCurrentSolarSystemId(ss_id);
        // get solarsystem info (name, constellation_id, security)
        if (m_api->get_universe_system(reply, ss_id)) {
            if (QThread::currentThread()->isInterruptionRequested()) return 0;

            QString ss_name = reply.value(QLatin1String("name")).toString();
            quint64 constellation_id = reply.value(QLatin1String("constellation_id")).toVariant().toULongLong();
            float ss_security = reply.value(QLatin1String("security_status")).toVariant().toFloat();
            ch.setCurrentSolarSystemName(ss_name);
            ch.setCurrentSolarSystemSecurity(ss_security);
            // get constellation name, if changed
            if (constellation_id != ch.currentConstellationId()) {
                ch.setCurrentConstellationId(constellation_id);
                if (m_api->get_universe_constellation(reply, constellation_id)) {
                    if (QThread::currentThread()->isInterruptionRequested()) return 0;

                    QString const_name = reply.value(QLatin1String("name")).toString();
                    quint64 region_id = reply.value(QLatin1String("region_id")).toVariant().toULongLong();
                    ch.setCurrentConstellationName(const_name);
                    // get region name, if changed
                    if (region_id != ch.currentRegionId()) {
                        ch.setCurrentRegionId(region_id);
                        if (m_api->get_universe_region(reply, region_id)) {
                            ch.setCurrentRegionName(reply.value(QLatin1String("name")).toString());
                        }
                    }
                }
            }
        }
    }
    if (QThread::currentThread()->isInterruptionRequested()) return 0;

    // qCDebug(logRefresher) << "Read character location ok:";
    // qCDebug(logRefresher) << "     solarsystem:" << ch.currentSolarSystemId() << ch.currentSolarSystemName() << ch.currentSolarSystemSecurity();
    // qCDebug(logRefresher) << "     constellation:" << ch.currentConstellationId() << ch.currentConstellationName();
    // qCDebug(logRefresher) << "     region:" << ch.currentRegionId() << ch.currentRegionName();

    // get current ship
    if (!m_api->get_character_ship(reply, ch.characterId(), ch.getAuthTokens().access_token)) {
        // req failed
        return 0;
    }
    if (QThread::currentThread()->isInterruptionRequested()) return 0;

    QmlEvemonApp *gApp = globalAppInstance();
    if (!gApp) return 0;
    Db *db = gApp->database();
    quint64 ship_type_id = 0;
    QString ship_friendly_name = reply.value(QLatin1String("ship_name")).toString();
    ship_type_id = reply.value(QLatin1String("ship_type_id")).toVariant().toULongLong();
    // if ship really changed
    if (ch.currentShipFriendlyName() != ship_friendly_name) {
        ch.setCurrentShipFriendlyName(ship_friendly_name);
    }
    if (ch.currentShipTypeId() != ship_type_id) {
        ch.setCurrentShipTypeId(ship_type_id);
        // lookup ship type name;
        QString shipTypeName = db->typeName(ship_type_id);
        if (!shipTypeName.isEmpty()) {
            ch.setCurrentShipTypeName(shipTypeName);
        }
    }

    ch.setUpdateTimestamp(UpdateTimestamps::UTST::LOCATION);
    return 1;
}


} // namespace EM
