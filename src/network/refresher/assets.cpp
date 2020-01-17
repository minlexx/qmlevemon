#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>
#include <QSet>

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
    CharacterAssetsLocationsModel assetsLocationsModel;
    Db *db = globalAppDatabaseInstance();

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

        // postprocess all assets
        QVector<AssetEntry> &assets = assetsModel.internalData();
        QSet<quint64> location_ids_to_resolve; // to store unique values
        for (AssetEntry &entry: assets) {
            // resolve type name
            if (db) {
                entry.type_name = db->typeName(entry.type_id);
            }
            // location name
            //  we can easily resolve location name if it is a station
            if (entry.location_type == AssetLocationType::Station) {
                EveLocation loc = resolve_location_guess_type(entry.location_id, ch->accessToken());
                if (!loc.isEmpty()) {
                    entry.location_name = loc.name();
                }
            } else if (entry.location_type == AssetLocationType::Other) {
                // not all location ids are resolvable in this case
                if (isAssetFitted(entry.location_flag) || isAssetInsideContainer(entry.location_flag)) {
                    // probably means that this item is inside another container
                    // add its location id to list of ids to resolve
                    location_ids_to_resolve << entry.location_id;
                    // we will collect them and resolve later in one call
                } else {
                    // location_type == Other and location_flag == Hangar means item is in Citadel hangar
                    EveLocation loc = resolve_location(entry.location_id, QLatin1String("structure"), ch->accessToken());
                    if (!loc.isEmpty()) {
                        entry.location_name = loc.name();
                    }
                }
            } else if (entry.location_type == AssetLocationType::SolarSystem) {
                // TODO: I don't have items in assets labelled as located in "solarsystem"
                // what to do?
                qCWarning(logRefresher) << " !!! entry.location_type == AssetLocationType::SolarSystem !!!";
            }
        }

        // resolve locations names
        if (!location_ids_to_resolve.isEmpty()) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
            const QVector<quint64> locations_ids_vec = location_ids_to_resolve.toList().toVector();
#else
            // since Qt 5.14 we can use more efficient range constructors
            const QVector<quint64> locations_ids_vec{location_ids_to_resolve.begin(), location_ids_to_resolve.end()};
#endif
            qCDebug(logRefresher) << "   Need to resolve" << locations_ids_vec.size() << " loc names";
            if (m_api->post_character_assets_names(replyArr, ch->characterId(), ch->accessToken(), locations_ids_vec)) {
                // qCDebug(logRefresher) << "locations_ids: " << replyArr;
                // fill all resolved locations names
                for (const QJsonValue &jval: replyArr) {
                    const QJsonObject jobj = jval.toObject();
                    const quint64 item_id = jobj.value(QLatin1String("item_id")).toVariant().toULongLong();
                    const QString resolved_name = jobj.value(QLatin1String("name")).toString();
                    //
                    if (!resolved_name.isEmpty()) {
                        // for each returned resolved location name set received name into container
                        // very long loop, in my character case loops over 1700 asset items
                        for (AssetEntry &entry: assets) {
                            if (entry.location_id == item_id) {
                                entry.location_name = resolved_name;
                            }
                            if (entry.item_id == item_id) {
                                // Tengu (Ship_name)
                                entry.type_name.append(QStringLiteral(" ("));
                                entry.type_name.append(resolved_name);
                                entry.type_name.append(QStringLiteral(")"));
                            }
                        }
                    }
                }

                // <debug - remove this>
//                for (const AssetEntry &entry: qAsConst(assets)) {
//                    if (entry.location_name.isEmpty()) {
//                        qCDebug(logRefresher) << "    item " << entry.item_id << entry.type_name
//                                              << " location not resolved; "
//                                              << "location_id=" << entry.location_id
//                                              << "location_type=" << static_cast<int>(entry.location_type)
//                                              << "location_flag=" << static_cast<int>(entry.location_flag);
//                        if (locations_ids_vec.contains(entry.location_id)) {
//                            qCDebug(logRefresher) << "        but location_id is in locations_ids_vec to resolve! WTF?";
//                        } else {
//                            qCDebug(logRefresher) << "        location_id is not in locations_ids_vec to resolve";
//                        }
//                    }
//                }
                // end <debug - remove this>
            }
        }

        assetsLocationsModel.fillLocationsFromAssets(assetsModel.internalData());

        ch->setAssetsModel(assetsModel);
        ch->setAssetsLocations(assetsLocationsModel);

        num_changes++;
    }

    if (num_changes > 0) {
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::ASSETS);
    }
    return num_changes;
}


} // namespaced EM
