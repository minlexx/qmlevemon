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
        qCDebug(logRefresher) << " no need to refresh assets for" << ch->toString();
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
    int page = 0, total_pages = 0;
    if (m_api->get_character_assets(replyArr, ch->characterId(), ch->accessToken(), page, &total_pages)) {
        for (const QJsonValue &jval: replyArr) {
            const QJsonObject jobj = jval.toObject();
            AssetEntry entry = AssetEntry::fromJson(jobj);
            assetsModel.internalData().push_back(std::move(entry));
        }
        // request extra pages if any
        if (total_pages > 1) {
            // first page was already requested
            for (page = 2; page <= total_pages; page++) {
                qCDebug(logRefresher) << " refreshing assets page" << page << "for" << ch->toString();
                if (m_api->get_character_assets(replyArr, ch->characterId(), ch->accessToken(), page)) {
                    for (const QJsonValue &jval: replyArr) {
                        const QJsonObject jobj = jval.toObject();
                        AssetEntry entry = AssetEntry::fromJson(jobj);
                        assetsModel.internalData().push_back(std::move(entry));
                    }
                }
            }
        }
        qCDebug(logRefresher) << "     total assets fetched:" << assetsModel.rowCount();
        ch->setAssetsModel(assetsModel);
        num_changes++;
    }

    if (num_changes > 0) {
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::ASSETS);
    }
    return num_changes;
}


} // namespaced EM
