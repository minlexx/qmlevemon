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
#include <QDebug>

#include "periodical_refresher.h"
#include "db/db.h"
#include "models/character.h"
#include "models/character_model.h"
#include "models/character_skill.h"
#include "models/model_manager.h"
#include "models/character_model.h"
#include "models/skill_tree_model.h"
#include "eve_api/eve_api.h"
#include "qmlevemon_app.h"

#include "refresher/periodical_refresher_worker.h"


Q_LOGGING_CATEGORY(logRefresher, "evemon.refresher")


namespace EM {


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
    QObject::connect(this, &PeriodicalRefresher::doRequestMailBody,
                     m_worker, &PeriodicalRefresherWorker::requestCharacterMailBody,
                     Qt::QueuedConnection);

    CharacterModel *characterModel = ModelManager::instance()->characterModel();
    QObject::connect(m_worker, &PeriodicalRefresherWorker::characterUpdated,
                     characterModel, &CharacterModel::updateCharacter,
                     Qt::QueuedConnection);
    QObject::connect(m_worker, &PeriodicalRefresherWorker::mailBodyDownloaded,
                     this, &PeriodicalRefresher::mailBodyDownloaded,
                     Qt::QueuedConnection);
    QObject::connect(m_worker, &PeriodicalRefresherWorker::networkError,
                     this, &PeriodicalRefresher::networkError,
                     Qt::QueuedConnection);

    m_thread.start(QThread::LowPriority);
    m_refreshTimer.start();
    // initial refresh
    QMetaObject::invokeMethod(m_worker, "initialDelayedRefresh", Qt::QueuedConnection);
    // QTimer::singleShot(3000, m_worker, &PeriodicalRefresherWorker::refresh);
    // ^^ this can cause crash inside QSingleShotTimer event handler inside QObject code, if
    //    application is closed right after the timer is fired; it is unsafe to
    //    connect a timer created in one thread to a slot in object in other thread.
}


PeriodicalRefresher::~PeriodicalRefresher()
{
    stopGracefully();
}

void PeriodicalRefresher::setRefreshIterval(int msec)
{
    m_refreshTimer.stop();
    m_refreshTimer.setInterval(msec);
    m_refreshTimer.start();
}

void PeriodicalRefresher::stopGracefully()
{
    m_refreshTimer.stop();
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

void PeriodicalRefresher::requestMailBody(Character *ch, quint64 mailId)
{
    Q_EMIT doRequestMailBody(ch, mailId);
}

bool PeriodicalRefresher::isMailDownloadInProgress() const
{
    return m_worker->isMailDownloadInProgress();
}

quint64 PeriodicalRefresher::refreshingCharacterId() const
{
    return m_worker->refreshingCharacterId();
}


} // namespace
