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
    if (m_api->get_character_mailing_lists(replyArr, ch->characterId(), ch->getAuthTokens().access_token)) {
        num_updates++;
    }

    // load mail labels
    if (m_api->get_character_mail_labels(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
        num_updates++;
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
