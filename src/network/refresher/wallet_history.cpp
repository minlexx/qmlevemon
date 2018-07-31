#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"
#include "models/character_wallet_models.h"
#include "models/eve_location.h"
#include "db/db.h"
#include "qmlevemon_app.h"

namespace EM {

int PeriodicalRefresherWorker::refresh_wallet_history(Character *ch)
{
    int num_changes = 0;
    if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::WALLET_JOURNAL)) {
        qCDebug(logRefresher) << " no need to refresh wallet history for" << ch->toString();
        return num_changes;  // no update needed, too early
    }
    // check if tokens needs refresing
    if (!this->check_refresh_token(ch)) {
        return num_changes;
    }

    CharacterWalletJournal journalModel;
    CharacterWalletTransactions transactionsModel;

    QJsonArray replyArr;
    int page = 0;
    qCDebug(logRefresher) << " refreshing wallet journal for" << ch->toString();
    if (m_api->get_character_wallet_journal(replyArr, ch->characterId(), ch->getAuthTokens().access_token, page)) {
        for (const QJsonValue &jval: replyArr) {
            const QJsonObject jobj = jval.toObject();
            WalletJournalEntry entry = WalletJournalEntry::fromJsonObject(jobj);
            journalModel.internalData().push_back(std::move(entry));
        }
        ch->setWalletJournal(journalModel);
        num_changes++;
    }

    Db *db = globalAppDatabaseInstance();

    qCDebug(logRefresher) << " refreshing wallet transactions for" << ch->toString();
    quint64 from_id = 0;
    if (m_api->get_character_wallet_transactions(replyArr, ch->characterId(), ch->getAuthTokens().access_token, from_id)) {
        for (const QJsonValue &jval: replyArr) {
            const QJsonObject jobj = jval.toObject();
            // parse json
            WalletSingleTransaction entry = WalletSingleTransaction::fromJsonObject(jobj);

            // Postprocess entry: we need to resolve IDs to names
            // location
            EveLocation loc;
            loc = resolve_location_guess_type(entry.location_id, ch->getAuthTokens().access_token);
            if (!loc.isEmpty()) {
                entry.location_name = loc.name();
            }

            // client
            entry.client_name = resolve_character_name(entry.client_id);

            // type
            if (db) {
                entry.type_name = db->typeName(entry.type_id);
            }

            // save in model
            transactionsModel.internalData().push_back(std::move(entry));
        }
        ch->setWalletTransactions(transactionsModel);
        num_changes++;
    }

    if (num_changes > 0) {
        ch->setUpdateTimestamp(UpdateTimestamps::UTST::WALLET_JOURNAL);
    }

    return num_changes;
}

} // namespace EM
