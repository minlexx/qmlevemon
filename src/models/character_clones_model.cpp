#include "character_clones_model.h"


namespace EM {


CharacterClonesModel::CharacterClonesModel(QObject *parent)
    : CommonModelBase<CharacterClone>(parent)
{
}


// base class's operator=() resets model, but our method setClones() does very nice model update
// without resetting whole model
CharacterClonesModel &CharacterClonesModel::operator=(const CharacterClonesModel &o)
{
    setClones(o.m_data);
    return *this;
}

CharacterClonesModel &CharacterClonesModel::operator=(CharacterClonesModel &&o)
{
    setClones(std::move(o.m_data));
    return *this;
}


QHash<int, QByteArray> CharacterClonesModel::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        {Roles::CloneId,            QByteArrayLiteral("cloneId")},
        {Roles::CloneName,          QByteArrayLiteral("cloneName")},
        {Roles::CloneLocation,      QByteArrayLiteral("cloneLocation")},
        {Roles::CloneImplantsModel, QByteArrayLiteral("cloneImplantsModel")},
    };
    return roles;
}

QVariant CharacterClonesModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    const CharacterClone *clone = validateIndexAndGetData(index);
    if (!clone) return ret;
    switch (role) {
    case Roles::CloneId:
        ret = clone->cloneId();
        break;
    case Roles::CloneName:
        ret = clone->cloneName();
        break;
    case Roles::CloneLocation: {
            QObject *loc = const_cast<QObject *>(clone->locationObj());
            ret = QVariant::fromValue(loc);
            //qDebug() << Q_FUNC_INFO << "Returning location:" << (*loc);
        } break;
    case Roles::CloneImplantsModel: {
            QObject *impsModel = const_cast<QObject *>(clone->implantsModel());
            ret = QVariant::fromValue(impsModel);
        } break;
    }
    return ret;
}

void CharacterClonesModel::setClones(const QVector<CharacterClone> &clns)
{
    // replace first existing rows
    int iRow = 0;
    for (const CharacterClone &clone: clns) {
        if (iRow < m_data.size()) {
            m_data.replace(iRow, clone);
        }
        iRow++;
    }
    if (clns.size() <= m_data.size()) {
        Q_EMIT dataChanged(index(0), index(clns.size() - 1));
        if (clns.size() > m_data.size()) {
            // remove extra rows left
            beginRemoveRows(QModelIndex(), clns.size(), m_data.size() - 1);
            m_data.remove(clns.size(), m_data.size() - clns.size());
            endRemoveRows();
        }
    } else {
        if (m_data.size() > 0) {
            Q_EMIT dataChanged(index(0), index(m_data.size() - 1));
        }
        // insert newer rows from new data to the end of existing
        beginInsertRows(QModelIndex(), m_data.size(), clns.size() - 1);
        for (int i = m_data.size(); i < clns.size(); i++) {
            m_data.append(clns[i]);
        }
        endInsertRows();
    }
}


} // namespace EM
