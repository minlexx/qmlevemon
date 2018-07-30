#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"
#include "models/character_wallet_models.h"

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
        qCDebug(logRefresher) << "json: " << replyArr;
        for (const QJsonValue &jval: replyArr) {
            const QJsonObject jobj = jval.toObject();
            WalletJournalEntry entry = WalletJournalEntry::fromJsonObject(jobj);
            journalModel.internalData().push_back(std::move(entry));
        }
        ch->setWalletJournal(journalModel);
        num_changes++;
    }

    qCDebug(logRefresher) << " refreshing wallet transactions for" << ch->toString();
    quint64 from_id = 0;
    if (m_api->get_character_wallet_transactions(replyArr, ch->characterId(), ch->getAuthTokens().access_token, from_id)) {
        qCDebug(logRefresher) << "json: " << replyArr;
        for (const QJsonValue &jval: replyArr) {
            const QJsonObject jobj = jval.toObject();
            WalletSingleTransaction entry = WalletSingleTransaction::fromJsonObject(jobj);
            // TODO: postprocess entry
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
