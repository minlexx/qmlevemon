#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"   // for logRefresher
#include "eve_api/eve_api.h"
#include "qmlevemon_app.h"
#include "db/db.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

namespace EM {


QString PeriodicalRefresherWorker::resolve_corporation_name(quint64 corpId)
{
    QString corpName;

    Db *db = globalAppDatabaseInstance();
    if (db) {
        corpName = db->findCachedCorporationName(corpId);
    }

    if (corpName.isEmpty()) {
        QJsonArray jarr;
        if (m_api->get_corporations_names(jarr, {corpId})) {
            for (const QJsonValue &jval: jarr) {
                const QJsonObject jobj = jval.toObject();

                const quint64 value_id   = jobj.value(QLatin1String("corporation_id")).toVariant().toULongLong();
                const QString value_name = jobj.value(QLatin1String("corporation_name")).toString();

                if (value_id == corpId) {
                    corpName = value_name;
                    break;
                }
            }

            if (!corpName.isEmpty() && db) {
                db->saveCachedCorporationName(corpId, corpName);
            }
        }
    }

    return corpName;
}

QString PeriodicalRefresherWorker::resolve_character_name(quint64 charId)
{
    QString charName;

    Db *db = globalAppDatabaseInstance();
    if (db) {
        charName = db->findCachedCharacterName(charId);
    }

    if (charName.isEmpty()) {
        QJsonArray jarr;
        if (m_api->get_characters_names(jarr, {charId})) {
            for (const QJsonValue &jval: jarr) {
                const QJsonObject jobj = jval.toObject();

                const quint64 value_id   = jobj.value(QLatin1String("character_id")).toVariant().toULongLong();
                const QString value_name = jobj.value(QLatin1String("character_name")).toString();

                if (value_id == charId) {
                    charName = value_name;
                    break;
                }
            }

            if (!charName.isEmpty() && db) {
                db->saveCachedCharacterName(charId, charName);
            }
        }
    }

    return charName;
}

QString PeriodicalRefresherWorker::resolve_alliance_name(quint64 allyId)
{
    QString allyName;

    Db *db = globalAppDatabaseInstance();
    if (db) {
        allyName = db->findCachedAllianceName(allyId);
    }

    if (allyName.isEmpty()) {
        QJsonArray jarr;
        if (m_api->get_alliances_names(jarr, {allyId})) {
            for (const QJsonValue &jval: jarr) {
                const QJsonObject jobj = jval.toObject();

                const quint64 value_id   = jobj.value(QLatin1String("alliance_id")).toVariant().toULongLong();
                const QString value_name = jobj.value(QLatin1String("alliance_name")).toString();

                if (value_id == allyId) {
                    allyName = value_name;
                    break;
                }
            }

            if (!allyName.isEmpty() && db) {
                db->saveCachedAllianceName(allyId, allyName);
            }
        }
    }

    return allyName;
}


} // namespace EM
