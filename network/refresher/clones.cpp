#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"
#include "models/character.h"
#include "models/character_implants_group.h"
#include "db/db.h"
#include "qmlevemon_app.h"

namespace EM {


int PeriodicalRefresherWorker::resresh_clones(Character *ch)
{
    if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::CLONES)) {
        qCDebug(logRefresher) << " no need to refresh clones for" << ch->toString();
        return 0;  // no update needed, too early
    }
    // check if tokens needs refresing
    if (!this->check_refresh_token(ch)) {
        return 0;
    }

    QmlEvemonApp *gApp = globalAppInstance();
    if (!gApp) return 0;
    Db *db = gApp->database();
    if (!db) return 0;

    int nChanges = 0;

    qCDebug(logRefresher) << " refreshing clones for" << ch->toString();
    QJsonObject reply;

    // 1. refresh clones
    if (m_api->get_character_clones(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
        qCDebug(logRefresher) << reply;
        // example response:
        //      {
        //        "jump_clones": [
        //          {
        //            "jump_clone_id": 37293234,
        //            "location_id": 60003466,
        //            "location_type": "station",
        //            "implants": [
        //              10212,
        //              10216,
        //              13258,
        //              13259,
        //              19684
        //            ],
        //            "name": "Active armor"
        //          },
        //          {
        //            "jump_clone_id": 36981517,
        //            "location_id": 1021551599501,
        //            "location_type": "structure",
        //            "implants": [],
        //            "name": ""
        //          }
        //        ],
        //        "last_clone_jump_date": "2018-03-08T18:43:04Z",
        //        "home_location": {
        //          "location_id": 60003916,
        //          "location_type": "station"
        //        }
        //      }

        nChanges++;
    }

    // if (QThread::currentThread()->isInterruptionRequested()) break; // break early

    // 2, refresh implants
    QJsonArray replyArr;
    if (m_api->get_character_implants(replyArr, ch->characterId(), ch->getAuthTokens().access_token)) {
        qCDebug(logRefresher) << replyArr;
        // example response : [ 22107, 22108, 22111, 22109, 22110, 13229, 13249 ]
        CharacterImplantsGroup currentCloneImps;
        for (const QJsonValue& jval: replyArr) {
            const quint64 implantTypeID = jval.toVariant().toULongLong();
            if (implantTypeID > 0) {
                // load type info from database
                const QJsonObject dbJson = db->typeInfo(implantTypeID);
                const QJsonArray dbJsonAttrs = db->typeAttributes(implantTypeID);
                // cosntruct implant inventory item
                const InvType implantItem = InvType::fromDatabaseJson(dbJson, dbJsonAttrs);
                // store it in implant set
                currentCloneImps.addImplant(std::move(implantItem));
            }
        }

        //qCDebug(logRefresher) << Q_FUNC_INFO << currentCloneImps;
        ch->currentClone()->setImplantsGroup(std::move(currentCloneImps));

        nChanges++;
    }

    // example request into eve_sde db:
    // SELECT it.typeName, ta.*, at.attributeName, at.description
    //  FROM dgmtypeattributes ta, dgmAttributeTypes at, invTypes it
    //  WHERE ta.typeid=13229 AND at.attributeid=ta.attributeid AND ta.typeID=it.typeID;

    if (nChanges > 0) {
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::CLONES);
    }
    return nChanges; // >0 : there was an update
}


} // namespace EM
