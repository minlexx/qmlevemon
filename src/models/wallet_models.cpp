#include "wallet_models.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>


namespace EM {


bool WalletJournalEntry::operator==(const WalletJournalEntry &o) const
{
    return (id == o.id);
}

bool WalletJournalEntry::operator!=(const WalletJournalEntry &o) const
{
    return !((*this) == o);
}

bool WalletJournalEntry::isValid() const
{
    return (id > 0);
}

WalletJournalEntry::ContextType WalletJournalEntry::contextStypeFromString(const QString &st)
{
    // [ structure_id, station_id, market_transaction_id, character_id, corporation_id,
    //   alliance_id, eve_system, industry_job_id, contract_id, planet_id, system_id, type_id ]
    if (st == QStringLiteral("structure_id"))          return ContextType::Structure;
    if (st == QStringLiteral("station_id"))            return ContextType::Station;
    if (st == QStringLiteral("market_transaction_id")) return ContextType::MarketTransaction;
    if (st == QStringLiteral("character_id"))          return ContextType::Character;
    if (st == QStringLiteral("corporation_id"))        return ContextType::Corporation;
    if (st == QStringLiteral("alliance_id"))           return ContextType::Alliance;
    if (st == QStringLiteral("eve_system"))            return ContextType::EveSystem;
    if (st == QStringLiteral("industry_job_id"))       return ContextType::IndustryJob;
    if (st == QStringLiteral("contract_id"))           return ContextType::Contract;
    if (st == QStringLiteral("planet_id"))             return ContextType::Planet;
    if (st == QStringLiteral("system_id"))             return ContextType::SystemId;
    if (st == QStringLiteral("type_id"))               return ContextType::TypeId;
    return ContextType::Unknown;
}

WalletJournalEntry WalletJournalEntry::fromJsonSobject(const QJsonObject &jobj)
{
    WalletJournalEntry ret;
    ret.id = jobj.value(QLatin1String("id")).toVariant().toULongLong();
    ret.amount = jobj.value(QLatin1String("amount")).toDouble();
    ret.balance = jobj.value(QLatin1String("balance")).toDouble();
    ret.context_id = jobj.value(QLatin1String("context_id")).toVariant().toULongLong();
    ret.context_id_type = WalletJournalEntry::contextStypeFromString(jobj.value(QLatin1String("context_id_type")).toString());
    ret.date = jobj.value(QLatin1String("date")).toVariant().toDateTime();
    ret.description = jobj.value(QLatin1String("description")).toString();
    ret.first_party_id = jobj.value(QLatin1String("first_party_id")).toVariant().toULongLong();
    ret.reason = jobj.value(QLatin1String("reason")).toString();
    ret.ref_type = jobj.value(QLatin1String("ref_type")).toString();
    ret.second_party_id = jobj.value(QLatin1String("second_party_id")).toVariant().toULongLong();
    ret.tax = jobj.value(QLatin1String("tax")).toDouble();
    ret.tax_receiver_id = jobj.value(QLatin1String("tax_receiver_id")).toVariant().toULongLong();
    return ret;
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
