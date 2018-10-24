#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h" // logging category
#include "eve_api/eve_api.h"
#include "qmlevemon_app.h"
#include "db/db.h"


namespace EM {


static QString parse_notification_line_value(const QString &line)
{
    QString ret;
    const QStringList lst = line.split(QLatin1String(": "));
    if (lst.size() == 2) {
        ret = lst.at(1).trimmed();
    }
    return ret;
}


void PeriodicalRefresherWorker::postprocess_notification_text(QString &text, const QString &type)
{
    Db *db = globalAppDatabaseInstance();
    const QStringList lines = text.split(QLatin1Char('\n'));
    if (type == QStringLiteral("KillReportFinalBlow")) {
        // killMailHash: 71d96e14914d1ee81dc0ee0164a8f750282a7b90
        // killMailID: 68095595
        // victimID: 91168769
        // victimShipTypeID: 32880
        QString newText;
        quint64 victimId = 0;
        quint64 shipTypeId = 0;
        for (const QString &line: lines) {
            if (line.startsWith(QLatin1String("victimID: "))) {
                victimId = parse_notification_line_value(line).toULongLong();
            }
            if (line.startsWith(QLatin1String("victimShipTypeID: "))) {
                shipTypeId = parse_notification_line_value(line).toULongLong();
            }
        }
        if ((victimId > 0) && (shipTypeId > 0) && db) {
            const QString charName = resolve_character_name(victimId);
            const QString shipName = db->typeName(shipTypeId);
            newText = QString(QLatin1String("<b>%1</b>  lost a  <b>%2</b>")).arg(charName).arg(shipName);
        }

        if (!newText.isEmpty()) {
            text = newText;
        }
    } else if (type == QStringLiteral("KillReportVictim")) {
        // killMailHash: 71d96e14914d1ee81dc0ee0164a8f750282a7b90
        // killMailID: 68095595
        // victimShipTypeID: 32880
        QString newText;
        quint64 shipTypeId = 0;
        for (const QString &line: lines) {
            if (line.startsWith(QLatin1String("victimShipTypeID: "))) {
                shipTypeId = parse_notification_line_value(line).toULongLong();
            }
        }
        if ((shipTypeId > 0) && db) {
            const QString shipName = db->typeName(shipTypeId);
            newText = QString(QLatin1String("<b>You</b>  lost a  <b>%2</b>")).arg(shipName);
        }

        if (!newText.isEmpty()) {
            text = newText;
        }
    //} else if (type == QStringLiteral("")) {
        // allianceID: null
        // armorPercentage: 100.0
        // charID: 2113387544
        // corpLinkData:
        // - showinfo
        // - 2
        // - 98547434
        // corpName: TitanSaurus
        // hullPercentage: 100.0
        // shieldPercentage: 99.99681833350898
        // solarsystemID: 31001668
        // structureID: &id001 1022536113035
        // structureShowInfoData:
        // - showinfo
        // - 35832
        // - *id001
        // structureTypeID: 35832
    } //else if () {
}


int PeriodicalRefresherWorker::refresh_notifications(Character *ch)
{
    if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::NOTIFICATIONS)) {
        qCDebug(logRefresher) << " no need to refresh notifications for" << ch->toString();
        return 0;  // no update needed, too early
    }
    // check if tokens needs refresing
    if (!this->check_refresh_token(ch)) {
        return 0;
    }

    qCDebug(logRefresher) << " refreshing notifications for" << ch->toString();
    QJsonArray replyArr;
    if (!m_api->get_character_notifications(replyArr, ch->characterId(), ch->accessToken())) {
        return 0; // req failed
    }
    if (QThread::currentThread()->isInterruptionRequested()) return 0;

    //  [
    //    {
    //      "notification_id": 791111667,
    //      "sender_id": 1000125,
    //      "sender_type": "corporation",
    //      "text": "challenge_name_id: 532976\nmax_progress: 1\npoints_awarded: 10\nprogress_text: 532977\n",
    //      "timestamp": "2018-03-29T18:07:00Z",
    //      "type": "SeasonalChallengeCompleted"
    //    },
    //  for alliance:
    //      "sender_id": 1614483120,
    //      "sender_type": "alliance",

    CharacterNotifications notifications;
    for (const QJsonValue &jv: replyArr) {
        const QJsonObject jobj = jv.toObject();
        // parse object
        Notification ntf;
        ntf.notificationId = jobj.value(QLatin1String("notification_id")).toVariant().toULongLong();
        ntf.notificationType = jobj.value(QLatin1String("type")).toString();
        ntf.senderId = jobj.value(QLatin1String("sender_id")).toVariant().toULongLong();
        ntf.senderType = jobj.value(QLatin1String("sender_type")).toString();
        ntf.timestamp = jobj.value(QLatin1String("timestamp")).toVariant().toDateTime();
        ntf.text = jobj.value(QLatin1String("text")).toString();
        ntf.is_read = jobj.value(QLatin1String("is_read")).toBool();

        // introduce a hack to fix a strange behaviour: somehow kill reports notifications are always
        // counted as unread in API, so forcefully mark kill reports as read. (client displayes separate UI for that)
        if ((ntf.notificationType == QStringLiteral("KillReportFinalBlow"))
                || (ntf.notificationType == QStringLiteral("KillReportVictim"))) {
            ntf.is_read = true;
        }
        // treat NPCStandingsLost as always read too (client displayes separate UI for that)
        if (ntf.notificationType == QStringLiteral("NPCStandingsLost")) {
            ntf.is_read = true;
        }

        // resolve sender
        ntf.senderDisplayName = QString(QLatin1String("%1 / %2")).arg(ntf.senderType).arg(ntf.senderId);
        if (ntf.senderType == QStringLiteral("corporation")) {
            QString corpName = this->resolve_corporation_name(ntf.senderId);
            if (!corpName.isEmpty()) {
                ntf.senderDisplayName = corpName;
            }
        } else if (ntf.senderType == QStringLiteral("character")) {
            QString charName = this->resolve_character_name(ntf.senderId);
            if (!charName.isEmpty()) {
                ntf.senderDisplayName = charName;
            }
        } else if (ntf.senderType == QStringLiteral("alliance")) {
            QString allianceName = this->resolve_alliance_name(ntf.senderId);
            if (!allianceName.isEmpty()) {
                ntf.senderDisplayName = allianceName;
            }
        }

        postprocess_notification_text(ntf.text, ntf.notificationType);

        // store it in model
        notifications.internalData().append(std::move(ntf));
    }

    ch->setMailNotifications(notifications);
    ch->setUpdateTimestamp(UpdateTimestamps::UTST::NOTIFICATIONS);

    return 1;
}


} // namespace EM
