#include <QColor>
#include "models/character.h"
#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"   // for logRefresher
#include "eve_api/eve_api.h"  // to be able to use Eve API


namespace EM {


int PeriodicalRefresherWorker::refresh_mail(Character *ch)
{
    int num_updates = 0;

    if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::MAIL)) {
        qCDebug(logRefresher) << " no need to refresh mail for" << ch->toString();
        return num_updates;  // no update needed, too early
    }
    // check if tokens needs refresing
    if (!this->check_refresh_token(ch)) {
        return num_updates;
    }

    qCDebug(logRefresher) << " refreshing mail for" << ch->toString();

    QJsonArray replyArr;
    QJsonObject reply;

    // load mailing lists
    QVector<MailRecipient> mailRecipients;
    if (m_api->get_character_mailing_lists(replyArr, ch->characterId(), ch->getAuthTokens().access_token)) {
        num_updates++;
        for (const QJsonValue &jval: replyArr) {
            const QJsonObject &jobj = jval.toObject();
            const quint64 mailing_list_id = jobj.value(QLatin1String("mailing_list_id")).toVariant().toULongLong();
            const QString mailing_list_name = jobj.value(QLatin1String("name")).toString();
            MailRecipient rcpt;
            rcpt.type = MailRecipient::Type::MailingList;
            rcpt.id = mailing_list_id;
            rcpt.name = mailing_list_name;
            mailRecipients.push_back(std::move(rcpt));
        }
    }

    // load mail labels
    CharacterMailLabels mailLabels;
    if (m_api->get_character_mail_labels(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
        num_updates++;
        int total_unread_count = reply.value(QLatin1String("total_unread_count")).toVariant().toInt();
        QJsonArray jmailLabels = reply.value(QLatin1String("labels")).toArray();
        qCDebug(logRefresher) << "  " << total_unread_count << "unread mails!";
        for (const QJsonValue &jval: jmailLabels) {
            const QJsonObject &jobj = jval.toObject();
            const quint64 label_id = jobj.value(QLatin1String("label_id")).toVariant().toULongLong();
            const QString label_name = jobj.value(QLatin1String("name")).toString();
            const QColor color(jobj.value(QLatin1String("color")).toVariant().toString());
            int unread_count = 0;
            if (jobj.contains(QLatin1String("unread_count"))) {
                unread_count = jobj.value(QLatin1String("name")).toVariant().toInt();
            }
            MailLabel lbl;
            lbl.id = label_id;
            lbl.name = label_name;
            lbl.color = color;
            lbl.unread_count = unread_count;
            mailLabels.internalData().push_back(std::move(lbl));
        }
    }

    // load mail headers list
    if (m_api->get_character_mail_headers(replyArr, ch->characterId(), ch->getAuthTokens().access_token)) {
        num_updates++;
    }

    if (num_updates > 0) {
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::MAIL);
    }
    return num_updates;
}


} // namespace EM
