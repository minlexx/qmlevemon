#include "character_clones_model.h"


namespace EM {


CharacterClonesModel::CharacterClonesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roles = {
        {Roles::CloneId,            QByteArrayLiteral("cloneId")},
        {Roles::CloneName,          QByteArrayLiteral("cloneName")},
        {Roles::CloneLocation,      QByteArrayLiteral("cloneLocation")},
        {Roles::CloneImplantsModel, QByteArrayLiteral("cloneImplantsModel")},
    };
}

QHash<int, QByteArray> CharacterClonesModel::roleNames() const
{
    return m_roles;
}

int CharacterClonesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_clones.count();
}

QVariant CharacterClonesModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) {
        return ret;
    }
    const int row = index.row();
    if ((row < 0) || (row >= m_clones.size())) {
        return ret;
    }
    const CharacterClone &clone = m_clones.at(row);
    switch (role) {
    case Roles::CloneId:
        ret = clone.cloneId();
        break;
    case Roles::CloneName:
        ret = clone.cloneName();
        break;
    case Roles::CloneLocation:
        ret = clone.locationObj();
        break;
    case Roles::CloneImplantsModel:
        ret = clone.implantsModel();
        break;
    }
    return ret;
}

QVector<CharacterClone> &CharacterClonesModel::clones()
{
    return m_clones;
}

const QVector<CharacterClone> &CharacterClonesModel::clones() const
{
    return m_clones;
}

void CharacterClonesModel::setClones(const QVector<CharacterClone> &clns)
{
    // TODO: implement CharacterClonesModel::setClones()
    Q_UNUSED(clns)
}


} // namespace EM
