#include "wallet_models.h"


namespace EM {


WalletJournalEntry::ContextType WalletJournalEntry::contextStypeFromString(const QString &st)
{
    // [ structure_id, station_id, market_transaction_id, character_id, corporation_id,
    //   alliance_id, eve_system, industry_job_id, contract_id, planet_id, system_id, type_id ]
    if (st == QStringLiteral("structure_id")) return ContextType::Structure;
    if (st == QStringLiteral("station_id")) return ContextType::Station;
    if (st == QStringLiteral("market_transaction_id")) return ContextType::MarketTransaction;
    if (st == QStringLiteral("character_id")) return ContextType::Character;
    if (st == QStringLiteral("corporation_id")) return ContextType::Corporation;
    if (st == QStringLiteral("alliance_id")) return ContextType::Alliance;
    if (st == QStringLiteral("eve_system")) return ContextType::EveSystem;
    if (st == QStringLiteral("industry_job_id")) return ContextType::IndustryJob;
    if (st == QStringLiteral("contract_id")) return ContextType::Contract;
    if (st == QStringLiteral("planet_id")) return ContextType::Planet;
    if (st == QStringLiteral("system_id")) return ContextType::SystemId;
    if (st == QStringLiteral("type_id")) return ContextType::TypeId;
    return ContextType::Unknown;
}


bool WalletSingleTransaction::operator==(const EM::WalletSingleTransaction &o) const
{
    return transaction_id == o.transaction_id;
}


bool WalletSingleTransaction::operator!=(const WalletSingleTransaction &o) const
{
    return !((*this) == o);
}



} // namespace EM
