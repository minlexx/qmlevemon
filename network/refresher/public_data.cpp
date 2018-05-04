#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"
#include "db/db.h"
#include "qmlevemon_app.h"


namespace EM {


int PeriodicalRefresherWorker::refresh_public_data(Character *ch) {
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

    // QMetaObject::invokeMethod(ch, "setCharacterName", Qt::BlockingQueuedConnection, Q_ARG(QString, name));
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


} // namesapce EM
