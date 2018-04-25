#include "character_clones_model.h"


namespace EM {


CharacterClonesModel::CharacterClonesModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_mutex(QMutex::Recursive)
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
    QMutexLocker lock(&m_mutex);
    return m_roles;
}

int CharacterClonesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    QMutexLocker lock(&m_mutex);
    return m_clones.count();
}

QVariant CharacterClonesModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) {
        return ret;
    }
    QMutexLocker lock(&m_mutex);
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
    case Roles::CloneLocation: {
            QObject *loc = const_cast<QObject *>(clone.locationObj());
            ret = QVariant::fromValue(loc);
            //qDebug() << Q_FUNC_INFO << "Returning location:" << (*loc);
        } break;
    case Roles::CloneImplantsModel: {
            QObject *impsModel = const_cast<QObject *>(clone.implantsModel());
            ret = QVariant::fromValue(impsModel);
        } break;
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
    QMutexLocker lock(&m_mutex);
    // replace first existing rows
    int iRow = 0;
    for (const CharacterClone &clone: clns) {
        if (iRow < m_clones.size()) {
            m_clones.replace(iRow, clone);
        }
        iRow++;
    }
    if (clns.size() <= m_clones.size()) {
        Q_EMIT dataChanged(index(0), index(clns.size() - 1));
        if (clns.size() > m_clones.size()) {
            // remove extra rows left
            beginRemoveRows(QModelIndex(), clns.size(), m_clones.size() - 1);
            m_clones.remove(clns.size(), m_clones.size() - clns.size());
            endRemoveRows();
        }
    } else {
        if (m_clones.size() > 0) {
            Q_EMIT dataChanged(index(0), index(m_clones.size() - 1));
        }
        // insert newer rows from new data to the end of existing
        beginInsertRows(QModelIndex(), m_clones.size(), clns.size() - 1);
        for (int i = m_clones.size(); i < clns.size(); i++) {
            m_clones.append(clns[i]);
        }
        endInsertRows();
    }
}


} // namespace EM
