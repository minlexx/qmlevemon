#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h" // logging category
#include "eve_api/eve_api.h"


namespace EM {


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
    if (!m_api->get_character_notifications(replyArr, ch->characterId(), ch->getAuthTokens().access_token)) {
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
        // store it in model
        notifications.internalData().append(std::move(ntf));
    }

    ch->setMailNotifications(notifications);
    ch->setUpdateTimestamp(UpdateTimestamps::UTST::NOTIFICATIONS);

    return 1;
}


} // namespace EM
