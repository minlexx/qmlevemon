#ifndef CHARACTER_CLONES_MODEL_H
#define CHARACTER_CLONES_MODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QHash>
#include <QByteArray>
#include <QMutex>
#include "character_clone.h"
#include "common_model_base.h"


namespace EM {


class CharacterClonesModel : public CommonModelBase<CharacterClone>
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
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
    void setClones(const QVector<CharacterClone> &clns);
};


} // namespace

#endif // CHARACTER_CLONES_MODEL_H
