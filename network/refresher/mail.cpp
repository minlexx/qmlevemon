#include <QColor>
#include "models/character.h"
#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"   // for logRefresher
#include "eve_api/eve_api.h"  // to be able to use Eve API
#include "qmlevemon_app.h"
#include "db/db_sqlite.h"


namespace EM {


void PeriodicalRefresherWorker::resolve_mail_recipients(
        QVector<MailRecipient> &recipients,
        const QVector<MailRecipient> &mailingLists)
{
    QmlEvemonApp *gApp = globalAppInstance();
    Db *db = nullptr;
    if (gApp) {
        db = gApp->database();
    }

    for (MailRecipient &rcpt: recipients) {
        if (!rcpt.name.isEmpty()) {
            continue;
        }

        // For mailing lists - we know them
        if (rcpt.type == MailRecipient::MailingList) {
            // get recipient name from mailing lists
            for (const MailRecipient &ml: mailingLists) {
                if (ml.id == rcpt.id) {
                    rcpt.name = ml.name;
                    break;
                }
            }
        } else {
            // Try to resolve recipient name from DB cache
            QString nameFromCache;
            if (db) {
                switch (rcpt.type) {
                case MailRecipient::Character:   nameFromCache = db->findCachedCharacterName(rcpt.id); break;
                case MailRecipient::Corporation: nameFromCache = db->findCachedCorporationName(rcpt.id); break;
                case MailRecipient::Alliance:    nameFromCache = db->findCachedAllianceName(rcpt.id); break;
                default: break;
                }
            }
            if (nameFromCache.isEmpty()) {
                // cache lookup failed, send API request
                QVector<quint64> ids{rcpt.id};
                QJsonArray replyArr;
                bool api_ok = false;

                switch (rcpt.type) {
                case MailRecipient::Character:   api_ok = m_api->get_characters_names(replyArr, ids); break;
                case MailRecipient::Corporation: api_ok = m_api->get_corporations_names(replyArr, ids); break;
                case MailRecipient::Alliance:    api_ok = m_api->get_alliances_names(replyArr, ids); break;
                default: break;
                }

                if (api_ok) {
                    for (const QJsonValue &jval: replyArr) {
                        const QJsonObject &jobj = jval.toObject();
                        quint64 value_id = 0;
                        QString value_name;
                        switch (rcpt.type) {
                        case MailRecipient::Character: {
                                value_id   = jobj.value(QLatin1String("character_id")).toVariant().toULongLong();
                                value_name = jobj.value(QLatin1String("character_name")).toString();
                            } break;
                        case MailRecipient::Corporation: {
                                value_id   = jobj.value(QLatin1String("corporation_id")).toVariant().toULongLong();
                                value_name = jobj.value(QLatin1String("corporation_name")).toString();
                            } break;
                        case MailRecipient::Alliance: {
                                value_id   = jobj.value(QLatin1String("alliance_id")).toVariant().toULongLong();
                                value_name = jobj.value(QLatin1String("alliance_name")).toString();
                            } break;
                        default: break;
                        }

                        if (value_id == rcpt.id) {
                            // found needed id in reply
                            nameFromCache = value_name;

                            // save it in cache
                            if (db) {
                                switch (rcpt.type) {
                                case MailRecipient::Character:   db->saveCachedCharacterName(rcpt.id, value_name); break;
                                case MailRecipient::Corporation: db->saveCachedCorporationName(rcpt.id, value_name); break;
                                case MailRecipient::Alliance:    db->saveCachedAllianceName(rcpt.id, value_name); break;
                                default: break;
                                }
                            }
                        }
                    }
                }
            }

            // finally remember lokked up name
            if (!nameFromCache.isEmpty()) {
                rcpt.name = nameFromCache;
            }
        }
    }
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

            // save mail label in model
            mailLabels.internalData().push_back(std::move(lbl));
        }

        // save mail labels in character
        ch->setMailLabels(mailLabels);
    }

    // load mail headers list
    CharacterMails mails;
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
            mail.id_from = from_id;
            mail.is_read = is_read;
            mail.subject = subject;
            mail.timestamp = timestamp;

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
            for (const QJsonValue &jval_r: jlabels) {
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

            // resolve mail recipient names
            resolve_mail_recipients(mail.recipients, mailingLists);

            // save mail in model
            mails.internalData().push_back(std::move(mail));
        }

        // save mails in character
        ch->setMails(mails);
    }

    if (num_updates > 0) {
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::MAIL);
    }
    return num_updates;
}


} // namespace EM
