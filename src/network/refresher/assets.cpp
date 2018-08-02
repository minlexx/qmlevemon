#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"
#include "db/db.h"
#include "qmlevemon_app.h"


namespace EM {


int PeriodicalRefresherWorker::refresh_assets(Character *ch)
{
    int num_changes = 0;

    if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::ASSETS)) {
        qCDebug(logRefresher) << " no need to refresh wallet history for" << ch->toString();
        return num_changes;  // no update needed, too early
    }
    // check if tokens needs refresing
    if (!this->check_refresh_token(ch)) {
        return num_changes;
    }

    CharacterAssetsModel assetsModel;
    // Db *db = globalAppDatabaseInstance();

    QJsonArray replyArr;
    qCDebug(logRefresher) << " refreshing assets for" << ch->toString();
    if (m_api->get_character_assets(replyArr, ch->characterId(), ch->accessToken())) {
        for (const QJsonValue &jval: replyArr) {
            const QJsonObject jobj = jval.toObject();
            AssetEntry entry = AssetEntry::fromJson(jobj);
            assetsModel.internalData().push_back(std::move(entry));
        }
        ch->setAssetsModel(assetsModel);
        num_changes++;
    }

    if (num_changes > 0) {
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::ASSETS);
    }
    return num_changes;
}


} // namespaced EM
