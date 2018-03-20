#include "character_skillqueue_model.h"

EM::CharacterSkillQueueModel::CharacterSkillQueueModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roles = {
        {SkillId, QByteArrayLiteral("skillId")},
        {SkillName, QByteArrayLiteral("skillName")},
    };
}

QHash<int, QByteArray> EM::CharacterSkillQueueModel::roleNames() const
{
    return m_roles;
}

int EM::CharacterSkillQueueModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    QMutexLocker lock(&m_mutex);
    return m_data.count();
}

QVariant EM::CharacterSkillQueueModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)
    return QVariant();
}

void EM::CharacterSkillQueueModel::addQueuedSkill(const EM::CharacterSkill &skill)
{
    Q_UNUSED(skill)
}
