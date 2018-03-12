#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>
#include <QDateTime>

#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"


namespace EM {


void PeriodicalRefresherWorker::refresh_server_status() {
    QJsonObject jobj;
    bool req_ok = m_api->get_server_status(jobj);
    if (QThread::currentThread()->isInterruptionRequested()) return;
    if (req_ok) {
        // parse reply
        int prev_server_players = m_server_players;
        int server_players = 0;
        QString server_version;
        QDateTime server_start_dt;
        //
        if (jobj.contains(QLatin1String("players"))) {
            server_players = jobj.value(QLatin1String("players")).toVariant().toInt();
        }
        if (jobj.contains(QLatin1String("server_version"))) {
            server_version = jobj.value(QLatin1String("server_version")).toString();
        }
        if (jobj.contains(QLatin1String("start_time"))) {
            server_start_dt = QDateTime::fromString(
                        jobj.value(QLatin1String("start_time")).toString(), Qt::ISODate);
        }
        qCDebug(logRefresher) << "Server status:" << server_players
                              << server_version << server_start_dt;
        // change atomic integer and emit signal only if value has really changed
        if (prev_server_players != server_players) {
            m_server_players = server_players;
            Q_EMIT m_owner->serverPlayersOnlineChanged();
        }
    }
}


} // namespace EM
