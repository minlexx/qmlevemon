#include <QAtomicInteger>
#include <QTimer>
#include <QHash>
#include <QList>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QLoggingCategory>
#include <QDebug>

#include "periodical_refresher.h"
#include "models/character.h"
#include "models/character_model.h"
#include "models/model_manager.h"
#include "eve_api/eve_api.h"


Q_LOGGING_CATEGORY(logRefresher, "evemon.refresher")


namespace EM {


class PeriodicalRefresherWorker: public QObject
{
    Q_OBJECT

public:
    PeriodicalRefresherWorker(PeriodicalRefresher *parent):
        QObject(Q_NULLPTR),
        m_api(new EveApi(this))
    {
        m_owner = parent;
        m_active = 0;
        m_server_players = 0;
    }

public Q_SLOTS:
    // this is run in a background thread in an async Qt signal-slot connection
    void refresh()
    {
        // already refreshing? prevent multiple threads enter into this function
        if (isNetworkActive()) {
            qCDebug(logRefresher) << "BG Refresh already active, skipping refresh now";
            return;
        }
        setNetworkActive(true);
        qCDebug(logRefresher) << "BG Refresh started";

        // maybe fill universe data, run only once per program start
        if (m_eve_races.isEmpty()) {
            this->fill_universe_data();
        }

        // update server status
        this->update_server_status();

        // update characters
        CharacterModel *cmodel = ModelManager::instance()->characterModel();
        QList<Character *> clist = cmodel->getCharacters();
        for (Character *ch: clist) {
            int num_updates = 0;

            // public data
            num_updates += this->refresh_public_data(ch);
            if (QThread::currentThread()->isInterruptionRequested()) break; // break early

            // skills
            num_updates += this->refresh_skills(ch);
            if (QThread::currentThread()->isInterruptionRequested()) break; // break early

            // location
            num_updates += this->refresh_location(ch);
            if (QThread::currentThread()->isInterruptionRequested()) break; // break early

            // wallet data
            num_updates += this->refresh_wallet(ch);
            if (QThread::currentThread()->isInterruptionRequested()) break; // break early

            // in the case of any updates were made to this character
            if (num_updates > 0) {
                cmodel->markCharacterAsUpdated(ch);
            }

            if (QThread::currentThread()->isInterruptionRequested()) break;  // break early
        }

        qCDebug(logRefresher) << "BG Refresh stopped";
        setNetworkActive(false);
    }

    bool isNetworkActive() const {
        int ret = m_active;
        return (ret != 0);
    }

    int serverPlayersOnline() const {
        return m_server_players;
    }


protected:
    void setNetworkActive(bool active)
    {
        if (active) m_active = 1;
        else m_active = 0;
        emit m_owner->networkActivityChanged();
    }


    void update_server_status() {
        QJsonObject jobj;
        bool req_ok = m_api->get_server_status(jobj);
        if (req_ok) {
            // parse reply
            int prev_server_players = m_server_players;
            int server_players = 0;
            QString server_version;
            QDateTime server_start_dt;
            //
            if (jobj.contains("players")) server_players = jobj.value("players").toVariant().toInt();
            if (jobj.contains("server_version")) server_version = jobj.value("server_version").toString();
            if (jobj.contains("start_time")) {
                server_start_dt = QDateTime::fromString(jobj.value("start_time").toString(), Qt::ISODate);
            }
            qCDebug(logRefresher) << "Server status:" << server_players
                                  << server_version << server_start_dt;
            // change atomic integer and emit signal only if value has really changed
            if (prev_server_players != server_players) {
                m_server_players = server_players;
                emit m_owner->serverPlayersOnlineChanged();
            }
        }
    }


    /**
     * @brief check_refresh_token - check if EVE OAuth tokens need refresh, refresh if needed
     * @param ch - EM::Character pointer whose tokens to refresh
     * @return true if refresh is not needed, ot refresh was OK. false on refresh error
     */
    bool check_refresh_token(Character *ch) {
        // not all functions require access_token, but for some...
        EveOAuthTokens tokens = ch->getAuthTokens();
        if (tokens.needsRefresh()) {
            qCDebug(logRefresher) << "  tokens for" << ch->toString()
                                  << " need refreshing...";
            if (eveapi_refresh_access_token(tokens)) {
                // qCDebug(logRefresher) << "  tokens refresh OK.";
                ch->setAuthTokens(tokens);
            } else {
                qCWarning(logRefresher) << "  tokens refreshing failed!";
                return false;
            }
        }
        return true;
    }


    int refresh_public_data(Character *ch) {
        if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::PUBLIC)) {
            qCDebug(logRefresher) << " no need to refresh public data for" << ch->toString();
            return 0;  // no update needed, too early
        }
        qCDebug(logRefresher) << " refreshing public data for" << ch->toString();
        QJsonObject obj;
        if (!m_api->get_character_public_info(obj, ch->characterId())) {
            return 0;
        }

        // parse reply
        // {
        //    "corporation_id": 1000107,
        //    "birthday": "2011-09-09T03:19:00Z",
        //    "name": "Lexx Min",
        //    "gender": "male",
        //    "race_id": 8,
        //    "bloodline_id": 7,
        //    "description": "<font ... ase, reborn</font>",
        //    "ancestry_id": 14,
        //    "security_status": 2.0520562182456263
        //  }

        QString name, bio;
        int gender = 0;
        QDateTime birthday;
        quint64 corp_id = 0, ally_id = 0, race_id = 0, bloodline_id = 0, ancestry_id = 0;
        float sec_status = 0.0f;

        name = obj.value("name").toString();
        QString sgender = obj.value("gender").toString();
        gender = 1;
        if (sgender == QLatin1String("male")) gender = 0;
        birthday = QDateTime::fromString(obj.value("birthday").toString(), Qt::ISODate);
        corp_id = obj.value("corporation_id").toVariant().toULongLong();
        if (obj.contains("alliance_id")) {
            ally_id = obj.value("alliance_id").toVariant().toULongLong();
        }
        race_id = obj.value("race_id").toVariant().toULongLong();
        bloodline_id = obj.value("bloodline_id").toVariant().toULongLong();
        ancestry_id = obj.value("ancestry_id").toVariant().toULongLong();
        sec_status = obj.value("security_status").toVariant().toFloat();
        bio = obj.value("description").toString();

        ch->setCharacterName(name);
        ch->setBio(bio);
        ch->setGender(gender);
        ch->setBirthday(birthday);
        ch->setCorporationId(corp_id);
        ch->setAllianceId(ally_id);
        ch->setRaceId(race_id);
        ch->setBloodlineId(bloodline_id);
        ch->setAncestryId(ancestry_id);
        ch->setSecurityStatus(sec_status);
        //
        // resolve names for race, bloodline, ancestry
        if (m_eve_races.contains(ch->raceId())) {
            ch->setRaceName(m_eve_races[ch->raceId()]);
        }
        if (m_eve_bloodlines.contains(ch->bloodlineId())) {
            ch->setBloodlineName(m_eve_bloodlines[ch->bloodlineId()]);
        }
        if (m_eve_ancestries.contains(ch->ancestryId())) {
            ch->setAncestryName(m_eve_ancestries[ch->ancestryId()]);
        } else {
            ch->setAncestryName(QString("Ancestry ID: %1").arg(ch->ancestryId()));
        }

        // fetch corpporation name
        QJsonObject corpReply;
        if (m_api->get_corporation_public_data(corpReply, ch->corporationId())) {
            QString corp_name, corp_ticker;
            corp_name = corpReply.value("corporation_name").toString();
            corp_ticker = corpReply.value("ticker").toString();
            if (corpReply.contains("alliance_id")) {
                ally_id = corpReply.value("alliance_id").toVariant().toULongLong();
                ch->setAllianceId(ally_id);
            }
            //
            ch->setCorporationName(corp_name);
            ch->setCorporationTicker(corp_ticker);

            // aliiance id, alliance name
            if (ch->allianceId() > 0) {
                QJsonObject allyReply;
                if (m_api->get_alliance_public_data(allyReply, ch->allianceId())) {
                    QString ally_name, ally_ticker;
                    ally_name = allyReply.value("alliance_name").toString();
                    ally_ticker = allyReply.value("ticker").toString();
                    ch->setAllianceName(ally_name);
                    ch->setAllianceTicker(ally_ticker);
                }
            }
        }

        ch->setUpdateTimestamp(UpdateTimestamps::UTST::PUBLIC);
        return 1;
    }

    int refresh_location(Character *ch) {
        if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::LOCATION)) {
            qCDebug(logRefresher) << " no need to refresh location for" << ch->toString();
            return 0;  // no update needed, too early
        }
        // check if tokens needs refresing
        if (!this->check_refresh_token(ch)) {
            return 0;
        }
        qCDebug(logRefresher) << " refreshing location for" << ch->toString();
        QJsonObject reply;

         // get current location

        if (!m_api->get_character_location(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
            return 0; // req failed
        }
        quint64 ss_id = 0, station_id = 0, structure_id = 0, prev_station_id = 0, prev_structure_id = 0;
        ss_id = reply.value("solar_system_id").toVariant().toULongLong();
        if (reply.contains("structure_id")) {
            structure_id = reply.value("structure_id").toVariant().toULongLong();
        }
        if (reply.contains("station_id")) {
            station_id = reply.value("station_id").toVariant().toULongLong();
        }

        prev_station_id = ch->currentStationId();
        prev_structure_id = ch->currentStructureId();
        ch->setCurrentStructureId(structure_id);
        ch->setCurrentStationId(station_id);

        if ((structure_id > 0) && (prev_structure_id != structure_id)) {
            // resolve structure name
            if (m_api->get_universe_structure(reply, structure_id, ch->getAuthTokens().access_token)) {
                QString structure_name = reply.value("name").toString();
                ch->setCurrentStructureName(structure_name);
            }
        } else if ((station_id > 0) && (prev_station_id != station_id)) {
            // resolve station name
            if (m_api->get_universe_station(reply, station_id)) {
                QString station_name = reply.value("name").toString();
                ch->setCurrentStructureName(station_name);
            }
        }

        // deal with results
        // *IF* the solarsystem *really* changed:
        if (ch->currentSolarSystemId() != ss_id) {
            ch->setCurrentSolarSystemId(ss_id);
            // get solarsystem info (name, constellation_id, security)
            if (m_api->get_universe_system(reply, ss_id)) {
                QString ss_name = reply.value("name").toString();
                quint64 constellation_id = reply.value("constellation_id").toVariant().toULongLong();
                float ss_security = reply.value("security_status").toVariant().toFloat();
                ch->setCurrentSolarSystemName(ss_name);
                ch->setCurrentSolarSystemSecurity(ss_security);
                // get constellation name, if changed
                if (constellation_id != ch->currentConstellationId()) {
                    ch->setCurrentConstellationId(constellation_id);
                    if (m_api->get_universe_constellation(reply, constellation_id)) {
                        QString const_name = reply.value("name").toString();
                        quint64 region_id = reply.value("region_id").toVariant().toULongLong();
                        ch->setCurrentConstellationName(const_name);
                        // get region name, if changed
                        if (region_id != ch->currentRegionId()) {
                            ch->setCurrentRegionId(region_id);
                            if (m_api->get_universe_region(reply, region_id)) {
                                ch->setCurrentRegionName(reply.value("name").toString());
                            }
                        }
                    }
                }
            }
        }

        // get current ship
        if (!m_api->get_character_ship(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
            // req failed
            return 0;
        }

        quint64 ship_type_id = 0;
        QString ship_friendly_name = reply.value("ship_name").toString();
        ship_type_id = reply.value("ship_type_id").toVariant().toULongLong();
        // if ship really changed
        if (ch->currentShipFriendlyName() != ship_friendly_name) {
            ch->setCurrentShipFriendlyName(ship_friendly_name);
        }
        if (ch->currentShipTypeId() != ship_type_id) {
            ch->setCurrentShipTypeId(ship_type_id);
            // lookup ship type name; first fast lookup in cache?
            if (m_eve_typeids.contains(ship_type_id)) {
                ch->setCurrentShipTypeName(m_eve_typeids[ship_type_id]);
            } else {
                // make a request :(
                if (m_api->get_universe_typeid(reply, ship_type_id)) {
                    QString ship_type_name = reply.value("name").toString();
                    if (!ship_type_name.isEmpty()) {
                        m_eve_typeids[ship_type_id] = ship_type_name;
                        ch->setCurrentShipTypeName(ship_type_name);
                    }
                }
            }
        }

        ch->setUpdateTimestamp(UpdateTimestamps::UTST::LOCATION);
        return 1;
    }

    int refresh_skills(Character *ch) {
        if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::SKILLS)) {
            qCDebug(logRefresher) << " no need to refresh skills for" << ch->toString();
            return 0;  // no update needed, too early
        }
        // check if tokens needs refresing
        if (!this->check_refresh_token(ch)) {
            return 0;
        }

        int num_updates = 0;
        QJsonObject reply;

        // refresh character attributes
        if (m_api->get_character_attributes(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
            num_updates++;
            /* { // example reply
              "charisma": 29,
              "intelligence": 31,
              "memory": 30,
              "perception": 39,
              "willpower": 38,
              "bonus_remaps": 1,
              "last_remap_date": "2015-04-18T14:35:25Z",
              "accrued_remap_cooldown_date": "2016-04-17T14:35:25Z"
            } */
            // TODO:
        }

        // refresh character skills
        // refresh character skillqueue

        ch->setUpdateTimestamp(UpdateTimestamps::UTST::SKILLS);
        return num_updates;
    }

    int refresh_wallet(Character *ch) {
        if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::WALLET)) {
            qCDebug(logRefresher) << " no need to refresh wallet for" << ch->toString();
            return 0;  // no update needed, too early
        }
        // check if tokens needs refresing
        if (!this->check_refresh_token(ch)) {
            return 0;
        }
        qCDebug(logRefresher) << " refreshing wallet for" << ch->toString();
        float isk = 0.0f;
        if (!m_api->get_character_wallet(isk, ch->characterId(), ch->getAuthTokens().access_token)) {
            return 0; // req failed
        }
        ch->setIskAmount(isk);
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::WALLET);
        return 1;
    }


    void fill_universe_data() {
        qCDebug(logRefresher) << "start fill_universe_data";
        QJsonArray jarr;

        // fill races
        if (m_api->get_universe_races(jarr)) {
            for (const QJsonValueRef race_info: jarr) {
                if (race_info.isObject()) {
                    QJsonObject jobj = race_info.toObject();
                    quint64 race_id = jobj.value("race_id").toVariant().toULongLong();
                    QString name = jobj.value("name").toString();
                    if ((race_id > 0) && (!name.isEmpty())) {
                        m_eve_races.insert(race_id, name);
                        qCDebug(logRefresher) << "  added race" << race_id << name;
                    }
                }
            }
        }

        // fill bloodlines
        if (m_api->get_universe_bloodlines(jarr)) {
            for (const QJsonValueRef bloodline_info: jarr) {
                if (bloodline_info.isObject()) {
                    QJsonObject jobj = bloodline_info.toObject();
                    quint64 bloodline_id = jobj.value("bloodline_id").toVariant().toULongLong();
                    QString name = jobj.value("name").toString();
                    if ((bloodline_id > 0) && (!name.isEmpty())) {
                        m_eve_bloodlines.insert(bloodline_id, name);
                        qCDebug(logRefresher) << "  added bloodline" << bloodline_id << name;
                    }
                }
            }
        }

        // TODO: fill ancestries: ESI currently does not have ancestries endpoint.

        qCDebug(logRefresher) << "end fill_universe_data";
    }


protected:
    PeriodicalRefresher *m_owner;
    QAtomicInteger<int> m_active;
    QAtomicInteger<int> m_server_players;
    //
    EveApi *m_api;
    // eve universe data
    // those hases store data that can never change (staticdata)
    QHash<quint64, QString> m_eve_races;
    QHash<quint64, QString> m_eve_bloodlines;
    QHash<quint64, QString> m_eve_ancestries;
    QHash<quint64, QString> m_eve_typeids;
};


PeriodicalRefresher::PeriodicalRefresher(QObject *parent):
    QObject(parent),
    m_worker(new PeriodicalRefresherWorker(this))
{
    m_worker->moveToThread(&m_thread);
    m_refreshTimer.setSingleShot(false);
    m_refreshTimer.setInterval(60000);

    QObject::connect(&m_refreshTimer, &QTimer::timeout,
                     m_worker, &PeriodicalRefresherWorker::refresh,
                     Qt::QueuedConnection);
    QObject::connect(this, &PeriodicalRefresher::refresh,
                     m_worker, &PeriodicalRefresherWorker::refresh,
                     Qt::QueuedConnection);

    m_thread.start(QThread::LowPriority);
    m_refreshTimer.start();
    // initial refresh
    // QMetaObject::invokeMethod(m_worker, "refresh", Qt::QueuedConnection); // dirty hack
    QTimer::singleShot(3000, m_worker, &PeriodicalRefresherWorker::refresh);
}


PeriodicalRefresher::~PeriodicalRefresher()
{
    qCDebug(logRefresher) << "BG Refresher stopping...";
    m_thread.requestInterruption();
    m_thread.quit();
    m_thread.wait();
    qCDebug(logRefresher) << "BG Refresher stopped.";
}


bool PeriodicalRefresher::networkActivity() const
{
    return m_worker->isNetworkActive();
}


int PeriodicalRefresher::serverPlayersOnline() const
{
    return m_worker->serverPlayersOnline();
}


} // namespace


#include "periodical_refresher.moc"
