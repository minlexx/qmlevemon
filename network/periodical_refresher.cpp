#include <QAtomicInteger>
#include <QTimer>
#include <QHash>
#include <QList>
#include <QUrl>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QLoggingCategory>
#include <QDebug>

#include "periodical_refresher.h"
#include "db/db.h"
#include "models/character.h"
#include "models/character_model.h"
#include "models/character_skill.h"
#include "models/model_manager.h"
#include "models/skill_tree_model.h"
#include "eve_api/eve_api.h"
#include "qmlevemon_app.h"


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
        Q_EMIT m_owner->networkActivityChanged();
    }


    void update_server_status() {
        QJsonObject jobj;
        bool req_ok = m_api->get_server_status(jobj);
        if (QThread::currentThread()->isInterruptionRequested()) return;
        if (req_ok) {
            // parse reply
            int prev_server_players = m_server_players;
            int server_players = 0;
            QString server_version;
            QDateTime server_start_dt;
            //
            if (jobj.contains(QLatin1String("players"))) {
                server_players = jobj.value(QLatin1String("players")).toVariant().toInt();
            }
            if (jobj.contains(QLatin1String("server_version"))) {
                server_version = jobj.value(QLatin1String("server_version")).toString();
            }
            if (jobj.contains(QLatin1String("start_time"))) {
                server_start_dt = QDateTime::fromString(
                            jobj.value(QLatin1String("start_time")).toString(), Qt::ISODate);
            }
            qCDebug(logRefresher) << "Server status:" << server_players
                                  << server_version << server_start_dt;
            // change atomic integer and emit signal only if value has really changed
            if (prev_server_players != server_players) {
                m_server_players = server_players;
                Q_EMIT m_owner->serverPlayersOnlineChanged();
            }
        }
    }


    /**
     * @brief check_refresh_token - check if EVE OAuth tokens need refresh, refresh if needed
     * @param ch - Character pointer whose tokens to refresh
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
        if (QThread::currentThread()->isInterruptionRequested()) return 0;

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

        name = obj.value(QLatin1String("name")).toString();
        QString sgender = obj.value(QLatin1String("gender")).toString();
        gender = 1;
        if (sgender == QLatin1String("male")) gender = 0;
        birthday = QDateTime::fromString(obj.value(QLatin1String("birthday")).toString(), Qt::ISODate);
        corp_id = obj.value(QLatin1String("corporation_id")).toVariant().toULongLong();
        if (obj.contains(QLatin1String("alliance_id"))) {
            ally_id = obj.value(QLatin1String("alliance_id")).toVariant().toULongLong();
        }
        race_id = obj.value(QLatin1String("race_id")).toVariant().toULongLong();
        bloodline_id = obj.value(QLatin1String("bloodline_id")).toVariant().toULongLong();
        ancestry_id = obj.value(QLatin1String("ancestry_id")).toVariant().toULongLong();
        sec_status = obj.value(QLatin1String("security_status")).toVariant().toFloat();
        bio = obj.value(QLatin1String("description")).toString();

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
        QmlEvemonApp *gApp = globalAppInstance();
        if (!gApp) return 0;
        Db *db = gApp->database();
        ch->setRaceName(db->raceName(ch->raceId()));
        ch->setBloodlineName(db->bloodlineName(ch->bloodlineId()));
        ch->setAncestryName(db->ancestryName(ch->ancestryId()));

        // fetch corpporation name
        QJsonObject corpReply;
        if (m_api->get_corporation_public_data(corpReply, ch->corporationId())) {
            if (QThread::currentThread()->isInterruptionRequested()) return 0;
            QString corp_name, corp_ticker;
            // ESI had changed corporation_name => name
            if (corpReply.contains(QLatin1String("corporation_name"))) {
                corp_name = corpReply.value(QLatin1String("corporation_name")).toString();
            } else if (corpReply.contains(QLatin1String("name"))) {
                corp_name = corpReply.value(QLatin1String("name")).toString();
            }
            corp_ticker = corpReply.value(QLatin1String("ticker")).toString();
            if (corpReply.contains(QLatin1String("alliance_id"))) {
                ally_id = corpReply.value(QLatin1String("alliance_id")).toVariant().toULongLong();
                ch->setAllianceId(ally_id);
            }
            //
            ch->setCorporationName(corp_name);
            ch->setCorporationTicker(corp_ticker);

            // aliiance id, alliance name
            if (ch->allianceId() > 0) {
                QJsonObject allyReply;
                if (m_api->get_alliance_public_data(allyReply, ch->allianceId())) {
                    if (QThread::currentThread()->isInterruptionRequested()) return 0;
                    QString ally_name, ally_ticker;
                    ally_name = allyReply.value(QLatin1String("alliance_name")).toString();
                    ally_ticker = allyReply.value(QLatin1String("ticker")).toString();
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
        if (QThread::currentThread()->isInterruptionRequested()) return 0;

        quint64 ss_id = 0, station_id = 0, structure_id = 0, prev_station_id = 0, prev_structure_id = 0;
        ss_id = reply.value(QLatin1String("solar_system_id")).toVariant().toULongLong();
        if (reply.contains(QLatin1String("structure_id"))) {
            structure_id = reply.value(QLatin1String("structure_id")).toVariant().toULongLong();
        }
        if (reply.contains(QLatin1String("station_id"))) {
            station_id = reply.value(QLatin1String("station_id")).toVariant().toULongLong();
        }

        prev_station_id = ch->currentStationId();
        prev_structure_id = ch->currentStructureId();
        ch->setCurrentStructureId(structure_id);
        ch->setCurrentStationId(station_id);

        if ((structure_id > 0) && (prev_structure_id != structure_id)) {
            // resolve structure name
            if (m_api->get_universe_structure(reply, structure_id, ch->getAuthTokens().access_token)) {
                if (QThread::currentThread()->isInterruptionRequested()) return 0;
                QString structure_name = reply.value(QLatin1String("name")).toString();
                ch->setCurrentStructureName(structure_name);
            }
        } else if ((station_id > 0) && (prev_station_id != station_id)) {
            // resolve station name
            if (m_api->get_universe_station(reply, station_id)) {
                if (QThread::currentThread()->isInterruptionRequested()) return 0;
                QString station_name = reply.value(QLatin1String("name")).toString();
                ch->setCurrentStructureName(station_name);
            }
        }
        if (QThread::currentThread()->isInterruptionRequested()) return 0;

        // deal with results
        // *IF* the solarsystem *really* changed:
        if (ch->currentSolarSystemId() != ss_id) {
            ch->setCurrentSolarSystemId(ss_id);
            // get solarsystem info (name, constellation_id, security)
            if (m_api->get_universe_system(reply, ss_id)) {
                if (QThread::currentThread()->isInterruptionRequested()) return 0;

                QString ss_name = reply.value(QLatin1String("name")).toString();
                quint64 constellation_id = reply.value(QLatin1String("constellation_id")).toVariant().toULongLong();
                float ss_security = reply.value(QLatin1String("security_status")).toVariant().toFloat();
                ch->setCurrentSolarSystemName(ss_name);
                ch->setCurrentSolarSystemSecurity(ss_security);
                // get constellation name, if changed
                if (constellation_id != ch->currentConstellationId()) {
                    ch->setCurrentConstellationId(constellation_id);
                    if (m_api->get_universe_constellation(reply, constellation_id)) {
                        if (QThread::currentThread()->isInterruptionRequested()) return 0;

                        QString const_name = reply.value(QLatin1String("name")).toString();
                        quint64 region_id = reply.value(QLatin1String("region_id")).toVariant().toULongLong();
                        ch->setCurrentConstellationName(const_name);
                        // get region name, if changed
                        if (region_id != ch->currentRegionId()) {
                            ch->setCurrentRegionId(region_id);
                            if (m_api->get_universe_region(reply, region_id)) {
                                ch->setCurrentRegionName(reply.value(QLatin1String("name")).toString());
                            }
                        }
                    }
                }
            }
        }
        if (QThread::currentThread()->isInterruptionRequested()) return 0;

        // get current ship
        if (!m_api->get_character_ship(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
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
        if (ch->currentShipFriendlyName() != ship_friendly_name) {
            ch->setCurrentShipFriendlyName(ship_friendly_name);
        }
        if (ch->currentShipTypeId() != ship_type_id) {
            ch->setCurrentShipTypeId(ship_type_id);
            // lookup ship type name;
            QString shipTypeName = db->typeName(ship_type_id);
            if (!shipTypeName.isEmpty()) {
                ch->setCurrentShipTypeName(shipTypeName);
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
        QJsonArray replyArr;

        // refresh character attributes
        qCDebug(logRefresher) << " refreshing attributes for" << ch->toString();
        if (m_api->get_character_attributes(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
            if (QThread::currentThread()->isInterruptionRequested()) return 0;

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
            int charisma = reply.value(QLatin1String("charisma")).toInt();
            int intelligence = reply.value(QLatin1String("intelligence")).toInt();
            int memory = reply.value(QLatin1String("memory")).toInt();
            int perception = reply.value(QLatin1String("perception")).toInt();
            int willpower = reply.value(QLatin1String("willpower")).toInt();
            int bonus_remaps = reply.value(QLatin1String("bonus_remaps")).toInt();
            QString slast_remap_date = reply.value(QLatin1String("last_remap_date")).toString();
            QString sremap_cooldown_date = reply.value(QLatin1String("accrued_remap_cooldown_date")).toString();
            QDateTime last_remap_date = QDateTime::fromString(slast_remap_date, Qt::ISODate);
            QDateTime remap_cooldown_date = QDateTime::fromString(sremap_cooldown_date, Qt::ISODate);
            if (ch->attributeCharisma() != charisma) {
                ch->setAttributeCharisma(charisma);
            }
            if (ch->attributeIntelligence() != intelligence) {
                ch->setAttributeIntelligence(intelligence);
            }
            if (ch->attributeMemory() != memory) {
                ch->setAttributeMemory(memory);
            }
            if (ch->attributePerception() != perception) {
                ch->setAttributePerception(perception);
            }
            if (ch->attributeWillpower() != willpower) {
                ch->setAttributeWillpower(willpower);
            }
            if (ch->numBonusRemaps() != bonus_remaps) {
                ch->setNumBonusRemaps(bonus_remaps);
            }
            if (ch->lastRemapDate() != last_remap_date) {
                ch->setLastRemapDate(last_remap_date);
            }
            if (ch->remapCooldownDate() != remap_cooldown_date) {
                ch->setRemapCooldownDate(remap_cooldown_date);
            }
        }
        if (QThread::currentThread()->isInterruptionRequested()) return 0;

        // refresh character skills
        SkillTreeModel *skillTree = ModelManager::instance()->skillTreeModel();
        // get ALL character's skills
        qCDebug(logRefresher) << " refreshing skills for" << ch->toString();
        if (m_api->get_character_skills(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
            if (QThread::currentThread()->isInterruptionRequested()) return 0;

            bool isAlphaClone = false;
            quint64 totalSp = reply.value(QLatin1String("total_sp")).toVariant().toULongLong();
            ch->setTotalSp(totalSp);
            QJsonArray jskills = reply.value(QLatin1String("skills")).toArray();
            for (auto jskillvalue: qAsConst(jskills)) {
                // qCDebug(logRefresher) << jskill;
                // {"active_skill_level":5, "skill_id":30547, "skillpoints_in_skill":256000, "trained_skill_level":5}
                const QJsonObject& jskill = jskillvalue.toObject();
                CharacterSkill chSkill;
                chSkill.setSkillId(jskill.value(QLatin1String("skill_id")).toVariant().toULongLong());
                chSkill.setActiveLevel(jskill.value(QLatin1String("active_skill_level")).toInt());
                chSkill.setTrainedLevel(jskill.value(QLatin1String("trained_skill_level")).toInt());
                chSkill.setSkillPointsInSkill(jskill.value(QLatin1String("skillpoints_in_skill")).toVariant().toULongLong());
                // detect alpha clone
                // if any skill has activeLevel < trainedLevel, this is alpha
                if (chSkill.activeLevel() < chSkill.trainedLevel()) {
                    isAlphaClone = true;
                }
            }
            // forcefully update character's alpha clone status
            ch->setIsAlphaClone(isAlphaClone);
        }

        // refresh character skillqueue
        qCDebug(logRefresher) << " refreshing skill queue for" << ch->toString();
        if (m_api->get_character_skillqueue(replyArr, ch->characterId(), ch->getAuthTokens().access_token)) {
            if (QThread::currentThread()->isInterruptionRequested()) return 0;

            // qCDebug(logRefresher) << replyArr;
            // QJsonArray([{"finish_date":"2018-02-15T18:45:55Z",
            //              "finished_level":5,
            //              "level_end_sp":1280000,
            //              "level_start_sp":226275,
            //              "queue_position":0,
            //              "skill_id":20315,
            //              "start_date":"2018-01-28T11:42:48Z",
            //              "training_start_sp":226275 },
        }

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
        if (QThread::currentThread()->isInterruptionRequested()) return 0;

        ch->setIskAmount(isk);
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::WALLET);
        return 1;
    }

protected:
    PeriodicalRefresher *m_owner;
    QAtomicInteger<int> m_active;
    QAtomicInteger<int> m_server_players;
    //
    EveApi *m_api;
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
    stopGracefully();
}

void PeriodicalRefresher::stopGracefully()
{
    if (m_thread.isRunning()) {
        qCDebug(logRefresher) << "BG Refresher stopping...";
        m_thread.requestInterruption();
        m_thread.quit();
        m_thread.wait();
        qCDebug(logRefresher) << "BG Refresher stopped.";
    }
}


bool PeriodicalRefresher::networkActivity() const
{
    return m_worker->isNetworkActive();
}


int PeriodicalRefresher::serverPlayersOnline() const
{
    return m_worker->serverPlayersOnline();
}


void PeriodicalRefresher::forceRefreshNow()
{
    Q_EMIT refresh();
}


} // namespace


#include "periodical_refresher.moc"
