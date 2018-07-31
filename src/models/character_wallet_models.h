#ifndef H_WALLET_MODELS
#define H_WALLET_MODELS

#include <QObject>
#include <QDateTime>
#include "common_model_base.h"

QT_FORWARD_DECLARE_CLASS(QJsonObject)
QT_FORWARD_DECLARE_CLASS(QDataStream)

namespace EM {


class WalletJournalEntry
{
    Q_GADGET
public:
    WalletJournalEntry() = default;
    WalletJournalEntry(const WalletJournalEntry &) = default;
    WalletJournalEntry(WalletJournalEntry &&) = default;
    ~WalletJournalEntry() = default;
    WalletJournalEntry &operator=(const WalletJournalEntry &) = default;
    WalletJournalEntry &operator=(WalletJournalEntry &&) = default;
    bool operator==(const WalletJournalEntry &o) const;
    bool operator!=(const WalletJournalEntry &o) const;
    bool isValid() const;

public:
    enum ContextType {
        Unknown = 0, Structure, Station, MarketTransaction, Character, Corporation,
        Alliance, EveSystem, IndustryJob, Contract, Planet, SystemId, TypeId
    };

    static ContextType contextStypeFromString(const QString &st);
    static WalletJournalEntry fromJsonObject(const QJsonObject &jobj);

public:
    quint64 id = 0; // Unique journal reference ID
    double amount = 0.0; // The amount of ISK given or taken from the wallet as a result of the given transaction.
    double balance = 0.0; // Wallet balance after transaction occurred
    quint64 context_id = 0; // An ID that gives extra context to the particular transaction.
                      // Because of legacy reasons the context is completely different per ref_type and means different things.
                      // It is also possible to not have a context_id
    int context_id_type = 0; // The type of the given context_id if present
                     // [ structure_id, station_id, market_transaction_id, character_id, corporation_id,
                     //   alliance_id, eve_system, industry_job_id, contract_id, planet_id, system_id, type_id ]
    QDateTime date; // Date and time of transaction
    QString description; // The reason for the transaction, mirrors what is seen in the client
    quint64 first_party_id = 0; // The id of the first party involved in the transaction.
          // This attribute has no consistency and is different or non existant for particular ref_types.
          // The description attribute will help make sense of what this attribute means.
          // For more info about the given ID it can be dropped into the /universe/names/ ESI route to determine its type and name
    quint64 second_party_id = 0;
    QString reason; // The user stated reason for the transaction. Only applies to some ref_types
    QString ref_type; // The transaction type for the given transaction. Different transaction types will populate different attributes.
          // Full list - https://github.com/esi/eve-glue/blob/master/eve_glue/wallet_journal_ref.py
    double tax = 0.0; // Tax amount received. Only applies to tax related transactions
    quint64 tax_receiver_id = 0; // The corporation ID receiving any tax paid. Only applies to tax related transactions
};


class WalletSingleTransaction
{
    Q_GADGET
public:
    WalletSingleTransaction() = default;
    WalletSingleTransaction(const WalletSingleTransaction &) = default;
    WalletSingleTransaction(WalletSingleTransaction &&) = default;
    ~WalletSingleTransaction() = default;
    WalletSingleTransaction &operator=(const WalletSingleTransaction &) = default;
    WalletSingleTransaction &operator=(WalletSingleTransaction &&) = default;
    bool operator==(const WalletSingleTransaction &o) const;
    bool operator!=(const WalletSingleTransaction &o) const;

    static WalletSingleTransaction fromJsonObject(const QJsonObject &jobj);

public:
    quint64 client_id = 0;
    QDateTime date;
    bool is_buy = false;
    bool is_personal = false;
    quint64 journal_ref_id = 0;
    quint64 location_id = 0;
    qint64 quantity = 0;
    quint64 transaction_id = 0;
    quint64 type_id = 0;
    double unit_price = 0.0;
    // for names resolving
    QString client_name;
    QString location_name;
    QString type_name;
};


class CharacterWalletJournal: public CommonModelBase<WalletJournalEntry>
{
    Q_OBJECT
    enum Roles {
        Id = Qt::UserRole + 1, Amount, Balance, ContextId, ContextIdType, Date, Description,
        FirstPartyId, SecondPartyId, Reason, RefType, Tax, TaxReceiverId
    };
public:
    CharacterWalletJournal(QObject *parent = nullptr);
public:  // only these methods need to be overriden
    QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // since this class does not add any data members,
    // base class's operators=, ==, stream save/load should work
};


class CharacterWalletTransactions: public CommonModelBase<WalletSingleTransaction>
{
    Q_OBJECT
    enum Roles {
        ClientId, ClientName, Date, IsBuy, IsPersonal, JournalRefId,
        LocationId, LocationName, Quantity, TransactionId,
        TypeId, TypeName, UnitPrice
    };
public:
    CharacterWalletTransactions(QObject *parent = nullptr);
public:  // only these methods need to be overriden
    QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};


} // namespace EM


QDataStream &operator<<(QDataStream &stream, const EM::WalletJournalEntry &o);
QDataStream &operator>>(QDataStream &stream, EM::WalletJournalEntry &o);
QDataStream &operator<<(QDataStream &stream, const EM::WalletSingleTransaction &o);
QDataStream &operator>>(QDataStream &stream, EM::WalletSingleTransaction &o);


#endif
