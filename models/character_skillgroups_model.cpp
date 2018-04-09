#include <algorithm>
#include <QSet>
#include <QDebug>
#include <QLoggingCategory>
#include "character_skillgroups_model.h"

Q_LOGGING_CATEGORY(logCharSkillGroupsModel, "evemon.characterskillgroupsmodel")


EM::CharacterSkillGroupsModel::CharacterSkillGroupsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_mutex(QMutex::Recursive)
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
        ret = item.m_groupName;
        break;
    case Roles::GroupId:
        ret = item.m_groupId;
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
    case Roles::NumSkillsInQueue:
        ret = item.m_numSkillsInQueue;
        break;
    case Roles::NumSkillsInTraining:
        ret = item.m_numSkillsInTraining;
        break;
    }
    return ret;
}

void EM::CharacterSkillGroupsModel::setFromSkills(const QVector<EM::CharacterSkill> &skills)
{
    // collect different excluse group IDs
    QSet<quint64> gset;
    QHash<quint64, int> skillsInGroup; // <group_id, num_skills>
    QHash<quint64, quint64> skillPointsGroup; // <group_id, num_skillpoints>
    QHash<quint64, int> skillsQueued; // <group_id, num_skills>
    QHash<quint64, int> skillsInTraining; // <group_id, num_skills>

    // construct new information into a temporary data structure
    QVector<ModelData> newData;
    int numAdded = 0;

    for (const EM::CharacterSkill &sk: qAsConst(skills)) {
        const SkillGroup *skillGroup = sk.skillGroup();
        // unlikely that skill has no group, but just in case..
        if (Q_LIKELY(skillGroup)) {

            quint64 groupId = skillGroup->groupId();

            // add only group that was not added before
            if (!gset.contains(groupId)) {
                gset << groupId;
                newData.push_back(ModelData(groupId, skillGroup->groupName(), skillGroup->numSkillsInGroup()));
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

            // count queued skills in this group
            if (!skillsQueued.contains(groupId)) {
                skillsQueued.insert(groupId, 0);
            }
            if (sk.isInQueue()) {
                ++skillsQueued[groupId];
            }

            // count skills in training in this group
            if (!skillsInTraining.contains(groupId)) {
                skillsInTraining.insert(groupId, 0);
            }
            if (sk.positionInQueue() == 0) {
                ++skillsInTraining[groupId];
            }

        } else {
            qCDebug(logCharSkillGroupsModel) << "Could not find a skill group for skill:" << sk;
        }
    }

    std::sort(newData.begin(), newData.end(), std::less<ModelData>());

    // qCDebug(logCharSkillGroupsModel) << " added " << numAdded << "skill groups";

    // update modeldata with collected counters
    for (ModelData &md: newData) {
        md.m_skillsInGroupTrained = skillsInGroup[md.m_groupId];
        md.m_skillPointsInGroup = skillPointsGroup[md.m_groupId];
        md.m_numSkillsInQueue = skillsQueued[md.m_groupId];
        md.m_numSkillsInTraining = skillsInTraining[md.m_groupId];
    }

    this->mergeModelData(newData);
}

void EM::CharacterSkillGroupsModel::mergeModelData(const QVector<EM::CharacterSkillGroupsModel::ModelData> &newData)
{
    QMutexLocker lock(&m_mutex);

    // loop over new data; add all new items and update existing items
    for (const ModelData& md: newData) {

        // try to find existing model data to update
        bool updated = false;
        for (int i = 0; i < m_data.size(); i++) {
            if (m_data[i].m_groupId == md.m_groupId) {
                m_data[i] = md;
                updated = true;
                Q_EMIT dataChanged(index(i), index(i));
                break;
            }
        }
        if (updated) continue;

        // if we are here, new item did not update existing, we need to add it
        // find a place where to insert at, keeping m_data sorted
        bool inserted = false;
        for (int i = 0; i < m_data.size(); i++) {
            if (md < m_data[i]) {
                // insert here
                beginInsertRows(QModelIndex(), i, i);
                m_data.insert(i, md);
                inserted = true;
                endInsertRows();
                break;
            }
        }
        // if still not inserted, append to the end
        if (!inserted) {
            // insert at end
            beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
            m_data.append(md);
            endInsertRows();
        }
    }

    // loop over all data, removing those who are not present in new data
    for (int i = 0; i < m_data.size(); i++) {
        const ModelData &ourMd = m_data.at(i);
        bool found = false;
        for (const ModelData &md: newData) {
            if (ourMd.m_groupId == md.m_groupId) {
                found = true;
                break;
            }
        }
        if (found) continue;
        // oops! our data is not in new data!
        beginRemoveRows(QModelIndex(), i, i);
        m_data.remove(i);
        endRemoveRows();
        i--;
    }
}

bool EM::CharacterSkillGroupsModel::ModelData::operator<(const EM::CharacterSkillGroupsModel::ModelData &o) const
{
    return (m_groupName < o.m_groupName);
}

bool EM::CharacterSkillGroupsModel::ModelData::operator==(const EM::CharacterSkillGroupsModel::ModelData &o) const
{
    return (m_groupId == o.m_groupId)
            && (m_groupName == o.m_groupName)
            && (m_skillsInGroupTrained == o.m_skillsInGroupTrained)
            && (m_skillsInGroupTotal == o.m_skillsInGroupTotal)
            && (m_skillPointsInGroup == o.m_skillPointsInGroup)
            && (m_numSkillsInQueue == o.m_numSkillsInQueue)
            && (m_numSkillsInTraining == o.m_numSkillsInTraining);
}
