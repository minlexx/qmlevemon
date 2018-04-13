#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"
#include "models/character.h"
#include "db/db.h"

namespace EM {


int PeriodicalRefresherWorker::resresh_clones(Character *ch)
{
    Q_UNUSED(ch)
    // if (QThread::currentThread()->isInterruptionRequested()) break; // break early
    return 0;
}


} // namespace EM
