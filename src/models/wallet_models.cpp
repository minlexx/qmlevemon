#include "wallet_models.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>
#include <QDataStream>


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


CharacterWalletJournal::CharacterWalletJournal(QObject *parent): CommonModelBase(parent)
{
}

QHash<int, QByteArray> CharacterWalletJournal::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        {Qt::DisplayRole,       QByteArrayLiteral("refType")},
        {Roles::Amount,         QByteArrayLiteral("amount")},
        {Roles::Balance,        QByteArrayLiteral("body")},
        {Roles::ContextId,      QByteArrayLiteral("contextId")},
        {Roles::ContextIdType,  QByteArrayLiteral("contextIdType")},
        {Roles::Date,           QByteArrayLiteral("date")},
        {Roles::Description,    QByteArrayLiteral("description")},
        {Roles::FirstPartyId,   QByteArrayLiteral("firstPartyId")},
        {Roles::SecondPartyId,  QByteArrayLiteral("secondPartyId")},
        {Roles::Reason,         QByteArrayLiteral("reason")},
        {Roles::RefType,        QByteArrayLiteral("refType")},
        {Roles::Tax,            QByteArrayLiteral("tax")},
        {Roles::TaxReceiverId,  QByteArrayLiteral("taxReceiverId")},
    };
    return roles;
}

QVariant CharacterWalletJournal::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    bool valid = true;
    const WalletJournalEntry *entry = validateIndexAndGetData(index, valid);
    if (!valid) {
        return ret;
    }
    switch (role) {
    case Qt::DisplayRole:
    case Roles::RefType: ret = entry->ref_type; break;
    case Roles::Amount: ret = entry->amount; break;
    case Roles::Balance: ret = entry->balance; break;
    case Roles::ContextId: ret = entry->context_id; break;
    case Roles::ContextIdType: ret = entry->context_id; break;
    case Roles::Date: ret = entry->date; break;
    case Roles::Description: ret = entry->description; break;
    case Roles::FirstPartyId: ret = entry->first_party_id; break;
    case Roles::SecondPartyId: ret = entry->second_party_id; break;
    case Roles::Reason: ret = entry->reason; break;
    case Roles::Tax: ret = entry->tax; break;
    case Roles::TaxReceiverId: ret = entry->tax_receiver_id; break;
    }
    return ret;
}

CharacterWalletTransactions::CharacterWalletTransactions(QObject *parent)
    : CommonModelBase(parent)
{
}

QHash<int, QByteArray> CharacterWalletTransactions::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        {Roles::ClientId,       QByteArrayLiteral("clientId")},
        {Roles::Date,           QByteArrayLiteral("date")},
        {Roles::IsBuy,          QByteArrayLiteral("isBuy")},
        {Roles::IsPersonal,     QByteArrayLiteral("isPersonal")},
        {Roles::JournalRefId,   QByteArrayLiteral("journalRefId")},
        {Roles::LocationId,     QByteArrayLiteral("locationId")},
        {Roles::Quantity,       QByteArrayLiteral("quantity")},
        {Roles::TransactionId,  QByteArrayLiteral("transactionId")},
        {Roles::TypeId,         QByteArrayLiteral("typeId")},
        {Roles::UnitPrice,      QByteArrayLiteral("unitPrice")},
    };
    return roles;
}

QVariant CharacterWalletTransactions::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    bool valid = true;
    const WalletSingleTransaction *entry = validateIndexAndGetData(index, valid);
    if (!valid) {
        return ret;
    }
    switch (role) {
    case Roles::ClientId: ret = entry->client_id; break;
    case Roles::Date: ret = entry->date; break;
    case Roles::IsBuy: ret = entry->is_buy; break;
    case Roles::IsPersonal: ret = entry->is_personal; break;
    case Roles::JournalRefId: ret = entry->journal_ref_id; break;
    case Roles::LocationId: ret = entry->location_id; break;
    case Roles::Quantity: ret = entry->quantity; break;
    case Roles::TransactionId: ret = entry->transaction_id; break;
    case Roles::TypeId: ret = entry->type_id; break;
    case Roles::UnitPrice: ret = entry->unit_price; break;
    }
    return ret;
}



} // namespace EM



QDataStream &operator<<(QDataStream &stream, const EM::WalletJournalEntry &o)
{
    stream << o.id;
    stream << o.amount;
    stream << o.balance;
    stream << o.context_id;
    stream << o.context_id_type;
    stream << o.date;
    stream << o.description;
    stream << o.first_party_id;
    stream << o.second_party_id;
    stream << o.reason;
    stream << o.ref_type;
    stream << o.tax;
    stream << o.tax_receiver_id;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::WalletJournalEntry &o)
{
    stream >> o.id;
    stream >> o.amount;
    stream >> o.balance;
    stream >> o.context_id;
    stream >> o.context_id_type;
    stream >> o.date;
    stream >> o.description;
    stream >> o.first_party_id;
    stream >> o.second_party_id;
    stream >> o.reason;
    stream >> o.ref_type;
    stream >> o.tax;
    stream >> o.tax_receiver_id;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const EM::WalletSingleTransaction &o)
{
    stream << o.client_id;
    stream << o.date;
    stream << o.is_buy;
    stream << o.is_personal;
    stream << o.journal_ref_id;
    stream << o.location_id;
    stream << o.quantity;
    stream << o.transaction_id;
    stream << o.type_id;
    stream << o.unit_price;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::WalletSingleTransaction &o)
{
    stream >> o.client_id;
    stream >> o.date;
    stream >> o.is_buy;
    stream >> o.is_personal;
    stream >> o.journal_ref_id;
    stream >> o.location_id;
    stream >> o.quantity;
    stream >> o.transaction_id;
    stream >> o.type_id;
    stream >> o.unit_price;
    return stream;
}
