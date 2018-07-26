#ifndef CHARACTER_CLONES_MODEL_H
#define CHARACTER_CLONES_MODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QHash>
#include <QByteArray>
#include <QMutex>
#include "character_clone.h"


namespace EM {


class CharacterClonesModel : public QAbstractListModel
{
    Q_OBJECT

    enum Roles {
        CloneName,
        CloneId,
        CloneLocation,
        CloneImplantsModel
    };

public:
    explicit CharacterClonesModel(QObject *parent = nullptr);

public:  // reimplmented interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
    QVector<CharacterClone> &clones();
    const QVector<CharacterClone> &clones() const;
    void setClones(const QVector<CharacterClone> &clns);

private:
    QHash<int, QByteArray>  m_roles;
    QVector<CharacterClone> m_clones;
    mutable QMutex m_mutex;
};


} // namespace

#endif // CHARACTER_CLONES_MODEL_H
