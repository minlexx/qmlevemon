#include <QSet>
#include <QDebug>
#include <QLoggingCategory>
#include "character_skillgroups_model.h"

Q_LOGGING_CATEGORY(logCharSkillGroupsModel, "evemon.characterskillgroupsmodel")


EM::CharacterSkillGroupsModel::CharacterSkillGroupsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roles.insert(Qt::DisplayRole, QByteArrayLiteral("display"));
    m_roles.insert(Roles::GroupName, QByteArrayLiteral("groupName"));
    m_roles.insert(Roles::GroupId, QByteArrayLiteral("groupId"));
}

QHash<int, QByteArray> EM::CharacterSkillGroupsModel::roleNames() const
{
    return m_roles;
}

int EM::CharacterSkillGroupsModel::rowCount(const QModelIndex &parent) const
{
    QMutexLocker lock(&m_mutex);
    return m_data.size();
}

QVariant EM::CharacterSkillGroupsModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) return ret;

    QMutexLocker lock(&m_mutex);
    if ((index.row() < 0) || (index.row() >= m_data.size())) return ret;

    const ModelData& item = m_data.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case Roles::GroupName:
        ret = item.m_name;
        break;
    case Roles::GroupId:
        ret = item.m_id;
        break;
    }
    return ret;
}

void EM::CharacterSkillGroupsModel::setFromSkills(const QVector<EM::CharacterSkill> &skills)
{
    beginResetModel();

    {
        QMutexLocker lock(&m_mutex);
        QSet<quint64> gset; // collect different excluse group IDs
        m_data.clear();
        int numAdded = 0;
        for (const EM::CharacterSkill &sk: qAsConst(skills)) {
            const SkillGroup *skillGroup = sk.skillGroup();
            // unlikely that skill has no group, but just in case..
            if (Q_LIKELY(skillGroup)) {
                // add only group that was not added before
                if (!gset.contains(skillGroup->groupId())) {
                    gset << skillGroup->groupId();
                    m_data.push_back(std::move(ModelData(skillGroup->groupId(), skillGroup->groupName())));
                    numAdded++;
                }
            }
        }

        // qCDebug(logCharSkillGroupsModel) << " added " << numAdded << "skill groups";
    }

    endResetModel();
}
