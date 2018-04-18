#ifndef CHARACTERIMPLANTSGROUP_H
#define CHARACTERIMPLANTSGROUP_H

#include <QObject>
#include <QByteArray>
#include <QAbstractListModel>
#include <QVector>
#include <QHash>
#include <QDataStream>
#include <QDebug>
#include "invtype.h"


namespace EM { class CharacterImplantsGroup; }

QDataStream& operator<<(QDataStream &stream, const EM::CharacterImplantsGroup &impGroup);
QDataStream& operator>>(QDataStream &stream, EM::CharacterImplantsGroup &impGroup);
QDebug operator<<(QDebug stream, const EM::CharacterImplantsGroup &impGroup);


namespace EM {


class CharacterImplantsGroup : public QAbstractListModel // public QObject
{
    Q_OBJECT

    enum Roles {
        DisplayName = Qt::DisplayRole,
        TypeId = Qt::UserRole + 1,
        ImplantSlot,
        IntelligenceBonus,
        MemoryBonus,
        PerceptionBonus,
        WillpowerBonus,
        CharismaBonus
    };

public:
    CharacterImplantsGroup(QObject *parent = nullptr);
    CharacterImplantsGroup(const CharacterImplantsGroup &other);
    CharacterImplantsGroup(CharacterImplantsGroup &&other);
    CharacterImplantsGroup &operator=(const CharacterImplantsGroup &other);
    CharacterImplantsGroup &operator=(CharacterImplantsGroup &&other);

public:  // reimplemented QAbstractListModel interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
    void addImplant(const InvType &imp);
    void addImplant(InvType &&imp);
    void clearImplants();
    int count() const;
    const InvType &at(int i) const;

private:
    QHash<int, QByteArray> m_roles;
    QVector<InvType> m_implants;
    // NOTE: I hope, this model does not need a mutex here, because it is
    // intended to be used from inside Character class, which is already
    // guarded by mutex in CharacterModel

    friend QDataStream& (::operator<<)(QDataStream &stream, const CharacterImplantsGroup &impGroup);
    friend QDataStream& (::operator>>)(QDataStream &stream, CharacterImplantsGroup &impGroup);
    friend QDebug (::operator<<)(QDebug stream, const EM::CharacterImplantsGroup &impGroup);
};


} // namespace


#endif // CHARACTERIMPLANTSGROUP_H
