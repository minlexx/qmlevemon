#include <QColor>
#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"   // for logRefresher
#include "eve_api/eve_api.h"  // to be able to use Eve API
#include "qmlevemon_app.h"
#include "db/db_sqlite.h"


namespace EM {


void PeriodicalRefresherWorker::resolve_single_mail_recipient(
        MailRecipient &rcpt,
        const QVector<MailRecipient> &mailingLists)
{
    if (rcpt.type == MailRecipient::None) {
        // recipient type is unknown, this is for mail sender
        // where the type of sender is not specified in API
        // we should guess what the type is. It can be a mailing list

        for (const MailRecipient &ml: mailingLists) {
            if (ml.id == rcpt.id) {
                rcpt.type = MailRecipient::MailingList;
                rcpt.name = ml.name;
                if (rcpt.name.isEmpty()) {
                    // unknown mailing list, this can happen if character WAS in this ML
                    // in the past, but now he's not in this list, so we can't know its name.
                    rcpt.name = this->tr("Uknown mailing list", "API parser");
                }
                // resolved
                return;
            }
        }

        // if we are here, this is not mailing list; then we assume that this is a character,
        // but can we have corporation or alliance as sender?
        rcpt.type = MailRecipient::Character;
    }

    // For mailing lists - we know them
    if (rcpt.type == MailRecipient::MailingList) {
        // get recipient name from mailing lists
        for (const MailRecipient &ml: mailingLists) {
            if (ml.id == rcpt.id) {
                rcpt.name = ml.name;
                if (rcpt.name.isEmpty()) {
                    // unknown mailing list, this can happen if character WAS in this ML
                    // in the past, but now he's not in this list, so we can't know its name.
                    rcpt.name = this->tr("Uknown mailing list", "API parser");
                }
                break;
            }
        }
    } else {
        // Try to resolve recipient name using our resolver
        QString nameFromCache;
        switch (rcpt.type) {
        case MailRecipient::Character:   nameFromCache = resolve_character_name(rcpt.id);   break;
        case MailRecipient::Corporation: nameFromCache = resolve_corporation_name(rcpt.id); break;
        case MailRecipient::Alliance:    nameFromCache = resolve_alliance_name(rcpt.id);    break;
        default: break;
        }

        // finally remember looked up name
        if (!nameFromCache.isEmpty()) {
            rcpt.name = nameFromCache;
        }
    }
}


void PeriodicalRefresherWorker::resolve_mail_recipients(
        QVector<MailRecipient> &recipients,
        const QVector<MailRecipient> &mailingLists)
{
    for (MailRecipient &rcpt: recipients) {
        if (!rcpt.name.isEmpty()) {
            continue;
        }
        resolve_single_mail_recipient(rcpt, mailingLists);
    }
}

Mail PeriodicalRefresherWorker::requestMailBody(const Character *ch, quint64 mailId)
{
    Mail ret;

    const QVector<MailRecipient> &mailingLists = ch->mailingLists();
    const QVector<MailLabel> &mailLabels = ch->mailLabels()->internalData();

    // 1. try from cache
    Db *db = globalAppDatabaseInstance();
    if (db) {
        const QJsonObject mailJson = db->loadMailBody(ch->characterId(), mailId);
        if (!mailJson.isEmpty()) {
            ret = Mail::fromJson(mailJson);
            ret.resolveLabels(mailLabels);
            resolve_single_mail_recipient(ret.from, mailingLists);
            resolve_mail_recipients(ret.recipients, mailingLists);
            ret.id = mailId;
            return ret;
        }
    }

    // 2. try from network, eve api
    QJsonObject mailJson;
    qCDebug(logRefresher) << "  requesting mail body for:" << mailId;
    if (m_api->get_character_mail_id(mailJson, ch->characterId(), ch->getAuthTokens().access_token, mailId)) {
        ret = Mail::fromJson(mailJson);
        ret.resolveLabels(mailLabels);
        // quick fix for sender type: if sender is in recipients list,
        //       inherit its recipient type for sender
        for (const auto &rcpt: ret.recipients) {
            if (rcpt.id == ret.from.id) {
                ret.from.type = rcpt.type;
                break;
            }
        }
        resolve_single_mail_recipient(ret.from, mailingLists);
        resolve_mail_recipients(ret.recipients, mailingLists);
        ret.id = mailId;

        // save mail body into cache
        if (db) {
            db->saveMailBody(ch->characterId(), mailId, mailJson);
        }
    }
    return ret;
}


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
    QVector<MailRecipient> mailingLists;
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
            mailingLists.push_back(std::move(rcpt));
        }

        // save mailing lists in character
        ch->setMailingLists(mailingLists);
    }

    // load mail labels
    CharacterMailLabels mailLabels;
    int api_unread_count = 0;
    if (m_api->get_character_mail_labels(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
        num_updates++;
        api_unread_count = reply.value(QLatin1String("total_unread_count")).toVariant().toInt();
        QJsonArray jmailLabels = reply.value(QLatin1String("labels")).toArray();
        for (const QJsonValue &jval: jmailLabels) {
            const QJsonObject &jobj = jval.toObject();
            const quint64 label_id = jobj.value(QLatin1String("label_id")).toVariant().toULongLong();
            const QString label_name = jobj.value(QLatin1String("name")).toString();
            const QColor color(jobj.value(QLatin1String("color")).toVariant().toString());
            const int unread_count = jobj.value(QLatin1String("unread_count")).toInt(0);
            MailLabel lbl;
            lbl.id = label_id;
            lbl.name = label_name;
            lbl.color = color;
            lbl.unread_count = unread_count;

            // save mail label in model
            mailLabels.internalData().push_back(std::move(lbl));
        }

        // add mailing lists to mail labels (?)
        for (const MailRecipient &ml: mailingLists) {
            MailLabel ml_lbl;
            ml_lbl.id = ml.id;
            ml_lbl.color = QColor(255, 255, 255);
            ml_lbl.name = ml.name;
            ml_lbl.unread_count = 0;
            // save mailing list label in model
            mailLabels.internalData().push_back(std::move(ml_lbl));
        }

        // save mail labels in character
        ch->setMailLabels(mailLabels);
    }

    // load mail headers list
    CharacterMails mails;
    int actual_unread_count = 0;
    if (m_api->get_character_mail_headers(replyArr, ch->characterId(), ch->getAuthTokens().access_token)) {
        num_updates++;
        for (const QJsonValue &jval: replyArr) {
            const QJsonObject &jobj = jval.toObject();
            //
            const quint64 mail_id = jobj.value(QLatin1String("mail_id")).toVariant().toULongLong();
            const quint64 from_id = jobj.value(QLatin1String("from")).toVariant().toULongLong();
            const QString subject = jobj.value(QLatin1String("subject")).toString();
            const QDateTime timestamp = jobj.value(QLatin1String("timestamp")).toVariant().toDateTime();
            const bool is_read = jobj.value(QLatin1String("is_read")).toBool(false); // may be absent, default false
            const QJsonArray jlabels = jobj.value(QLatin1String("labels")).toArray();
            const QJsonArray jrecipients = jobj.value(QLatin1String("recipients")).toArray();

            Mail mail;
            mail.id = mail_id;
            mail.from.id = from_id;
            mail.from.type = MailRecipient::None; // we don't know exactly sender type; it can be mailing list
            mail.is_read = is_read;
            mail.subject = subject;
            mail.timestamp = timestamp;

            actual_unread_count += (is_read == false ? 1 : 0);

            // fill labels
            QVector<quint64> vlabels;
            for (const QJsonValue &jval: jlabels) {
                const quint64 label_id = jval.toVariant().toULongLong();
                vlabels.push_back(label_id);
            }

            mail.labels = vlabels;
            // autofill labels strings
            mail.resolveLabels(mailLabels.internalData());

            // fill recipients
            QVector<MailRecipient> mailRecipients;
            for (const QJsonValue &jval_r: jrecipients) {
                const QJsonObject &jobj_r = jval_r.toObject();
                //
                const quint64 recipient_id = jobj_r.value(QLatin1String("recipient_id")).toVariant().toULongLong();
                const QString recipient_type_s = jobj_r.value(QLatin1String("recipient_type")).toString();
                const MailRecipient::Type recipient_type = MailRecipient::typeFromString(recipient_type_s);
                //
                MailRecipient rcpt;
                rcpt.id = recipient_id;
                rcpt.type = recipient_type;
                //rcpt.name - ? // to be resolved later

                mailRecipients.push_back(std::move(rcpt));
            }

            mail.recipients = mailRecipients;

            // quick fix for sender type: if sender is in recipients list,
            //       inherit its recipient type for sender.
            // TODO: fix this when API will have this field explicitly (from_type or sender_type)
            for (const auto &rcpt: mail.recipients) {
                if (rcpt.id == mail.from.id) {
                    mail.from.type = rcpt.type;
                    break;
                }
            }
            // resolve sender
            resolve_single_mail_recipient(mail.from, mailingLists);

            // resolve mail recipient names
            resolve_mail_recipients(mail.recipients, mailingLists);

            // save mail in model
            mails.internalData().push_back(std::move(mail));
        }

        // save mails in character
        // qCDebug(logRefresher) << "   Parsed mails: " << mails.internalData();
        ch->setMails(mails);
    }

    if (api_unread_count != actual_unread_count) {
        qCDebug(logRefresher) << "  " << api_unread_count << "unread mails from API,"
                              << actual_unread_count << "counted manually from headers.";
    }

    if (num_updates > 0) {
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::MAIL);
    }
    return num_updates;
}


} // namespace EM
