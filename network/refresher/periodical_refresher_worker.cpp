#include <QThread>

#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"
#include "models/character.h"
#include "models/character_model.h"
#include "models/model_manager.h"


namespace EM {


PeriodicalRefresherWorker::PeriodicalRefresherWorker(PeriodicalRefresher *parent)
    : QObject(Q_NULLPTR),
    m_api(new EveApi(this))
{
    m_owner = parent;
    m_active = 0;
    m_server_players = 0;
}


PeriodicalRefresherWorker::~PeriodicalRefresherWorker()
{
    setNetworkActive(false);
}


void PeriodicalRefresherWorker::refresh()
{
    // already refreshing? prevent multiple threads enter into this function
    if (isNetworkActive()) {
        qCDebug(logRefresher) << "BG Refresh already active, skipping refresh now";
        return;
    }
    setNetworkActive(true);
    qCDebug(logRefresher) << "BG Refresh started";

    // update server status
    this->refresh_server_status();

    // update characters
    // get a copied list of characters and do all modifications to copies
    // after all data was updated, return modified copy to model
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

        // in the case of any updates were made to this character,
        // after all data was updated, return modified copy to model
        if (num_updates > 0) {
            // cmodel->updateCharacter(ch); // <<=- We cannot do that, because
            //  it would cause model to be updated from different thread
            //  and model qould emit its dataChanged() from the wrong thread.
            // It causes the foolowing problem:
            //  QObject::connect: Cannot queue arguments of type 'QQmlChangeSet'
            //  (Make sure 'QQmlChangeSet' is registered using qRegisterMetaType().)
            //  as Q_EMIT dataChanged() inside model would become QueuedConnection type
            //  and due to impossibility of queueing 'QQmlChangeSet' this change will be
            //  ignored and QML UI will not be updated.

            // That's why we need to emit a queued signal that only sends a pointer
            //  and slot should be handled in model, in main thread, so no thread
            //  ownership problem exists.
            Q_EMIT characterUpdated(ch);
            // ^^ slot connected to this signal will delete ch's memory
        } else {
            // character was not updated, so its memory will not be freed in slot,
            //  we need to manually delete it here
            delete ch;
        }

        if (QThread::currentThread()->isInterruptionRequested()) break;  // break early
    }

    qCDebug(logRefresher) << "BG Refresh stopped";
    setNetworkActive(false);
}

void PeriodicalRefresherWorker::initialDelayedRefresh()
{
    // fire refresh, but delayed for 3 seconds
    QTimer::singleShot(3000, this, &PeriodicalRefresherWorker::refresh);
}


bool PeriodicalRefresherWorker::isNetworkActive() const {
    int ret = m_active;
    return (ret != 0);
}


int PeriodicalRefresherWorker::serverPlayersOnline() const {
    return m_server_players;
}


void PeriodicalRefresherWorker::setNetworkActive(bool active)
{
    if (active) m_active = 1;
    else m_active = 0;
    Q_EMIT m_owner->networkActivityChanged();
}

/**
 * @brief check_refresh_token - check if EVE OAuth tokens need refresh, refresh if needed
 * @param ch - Character pointer whose tokens to refresh
 * @return true if refresh is not needed, ot refresh was OK. false on refresh error
 */
bool PeriodicalRefresherWorker::check_refresh_token(Character *ch) {
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


} // namespace EM
