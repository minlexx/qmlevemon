#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"


namespace EM {


int PeriodicalRefresherWorker::refresh_wallet(Character *ch) {
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


} // namespace EM

