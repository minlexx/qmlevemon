#include <algorithm>
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
    m_roles.insert(Roles::SkillsInGroupTrained, QByteArrayLiteral("skillsInGroupTrained"));
    m_roles.insert(Roles::SkillsInGroupTotal, QByteArrayLiteral("skillsInGroupTotal"));
    m_roles.insert(Roles::SkillPointsInGroup, QByteArrayLiteral("skillPointsInGroup"));
    m_roles.insert(Roles::NumSkillsInQueue, QByteArrayLiteral("numSkillsInQueue"));
    m_roles.insert(Roles::NumSkillsInTraining, QByteArrayLiteral("numSkillsInTraining"));
}

//EM::CharacterSkillGroupsModel::CharacterSkillGroupsModel(const EM::CharacterSkillGroupsModel &other)
//    : QAbstractListModel(other.parent())
//{
//    (*this) = other;
//}

//EM::CharacterSkillGroupsModel::CharacterSkillGroupsModel(EM::CharacterSkillGroupsModel &&other)
//    : QAbstractListModel(other.parent())
//{
//    (*this) = std::move(other);
//}

//EM::CharacterSkillGroupsModel &EM::CharacterSkillGroupsModel::operator=(const EM::CharacterSkillGroupsModel &other)
//{
//    if (this == &other) return *this;
//    {
//        beginResetModel();
//        QMutexLocker lock(&m_mutex);
//        m_roles = other.m_roles;
//        m_data = other.m_data;
//    }
//    endResetModel();
//    return *this;
//}

//EM::CharacterSkillGroupsModel &EM::CharacterSkillGroupsModel::operator=(EM::CharacterSkillGroupsModel &&other)
//{
//    if (this == &other) return *this;
//    {
//        beginResetModel();
//        QMutexLocker lock(&m_mutex);
//        m_roles = std::move(other.m_roles);
//        m_data = std::move(other.m_data);
//    }
//    endResetModel();
//    return *this;
//}

QHash<int, QByteArray> EM::CharacterSkillGroupsModel::roleNames() const
{
    return m_roles;
}

int EM::CharacterSkillGroupsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
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
    case Roles::SkillsInGroupTrained:
        ret = item.m_skillsInGroupTrained;
        break;
    case Roles::SkillsInGroupTotal:
        ret = item.m_skillsInGroupTotal;
        break;
    case Roles::SkillPointsInGroup:
        ret = item.m_skillPointsInGroup;
        break;
    // TODO: currently not done, implement skill queues first
    case Roles::NumSkillsInQueue:
        ret = item.m_numSkillsInQueue;
        break;
    // TODO: currently not done, implement skill queues first
    case Roles::NumSkillsInTraining:
        ret = item.m_numSkillsInTraining;
        break;
    }
    return ret;
}

void EM::CharacterSkillGroupsModel::setFromSkills(const QVector<EM::CharacterSkill> &skills)
{
    beginResetModel();

    {
        QMutexLocker lock(&m_mutex);

        // collect different excluse group IDs
        QSet<quint64> gset;
        QHash<quint64, int> skillsInGroup; // <group_id, num_skills>
        QHash<quint64, quint64> skillPointsGroup; // <group_id, num_skillpoints>
        m_data.clear();
        int numAdded = 0;
        for (const EM::CharacterSkill &sk: qAsConst(skills)) {
            const SkillGroup *skillGroup = sk.skillGroup();
            // unlikely that skill has no group, but just in case..
            if (Q_LIKELY(skillGroup)) {

                quint64 groupId = skillGroup->groupId();

                // add only group that was not added before
                if (!gset.contains(groupId)) {
                    gset << groupId;
                    m_data.push_back(ModelData(groupId, skillGroup->groupName(), skillGroup->numSkillsInGroup()));
                    numAdded++;
                }

                // count skills in this group
                if (!skillsInGroup.contains(groupId)) {
                    skillsInGroup.insert(groupId, 1);
                } else {
                    ++skillsInGroup[groupId];
                }

                // count skillpoints in this group
                if (!skillPointsGroup.contains(groupId)) {
                    skillPointsGroup.insert(groupId, 0);
                }
                skillPointsGroup[groupId] += sk.skillPointsInSkill();
            } else {
                qCDebug(logCharSkillGroupsModel) << "Could not find a skill group for skill:" << sk;
            }
        }
        std::sort(m_data.begin(), m_data.end(), std::less<ModelData>());

        // qCDebug(logCharSkillGroupsModel) << " added " << numAdded << "skill groups";

        // update modeldata with collected counters
        for (ModelData &md: m_data) {
            md.m_skillsInGroupTrained = skillsInGroup[md.m_id];
            md.m_skillPointsInGroup = skillPointsGroup[md.m_id];
        }
    }

    endResetModel();
}

bool EM::CharacterSkillGroupsModel::ModelData::operator<(const EM::CharacterSkillGroupsModel::ModelData &o) const
{
    return (m_name < o.m_name);
}
