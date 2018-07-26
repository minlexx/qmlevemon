#include <utility>
#include "skill_group.h"

namespace EM {


SkillGroup::SkillGroup():
    QObject(nullptr)
{
}


SkillGroup::SkillGroup(const SkillGroup& other):
    QObject(nullptr)
{
    (*this) = other;
}

SkillGroup::SkillGroup(SkillGroup &&other)
{
    (*this) = std::move(other);
}


SkillGroup& SkillGroup::operator=(const SkillGroup& other)
{
    if (this == &other) return (*this);
    m_groupId = other.m_groupId;
    m_groupName = other.m_groupName;
    return (*this);
}


SkillGroup& SkillGroup::operator=(SkillGroup&& other)
{
    m_groupId = std::move(other.m_groupId);
    m_groupName = std::move(other.m_groupName);
    return (*this);
}


bool SkillGroup::operator==(const SkillGroup &other) const
{
    if (this == &other) return true;
    return (m_groupId == other.m_groupId);
}


QString SkillGroup::groupName() const { return m_groupName; }

quint64 SkillGroup::groupId() const { return m_groupId; }

int SkillGroup::numSkillsInGroup() const { return m_numSkillsInGroup; }


void SkillGroup::setGroupName(const QString& groupName)
{
    if (m_groupName == groupName) return;
    m_groupName = groupName;
    Q_EMIT groupNameChanged();
}


void SkillGroup::setGroupId(quint64 groupId)
{
    if (m_groupId == groupId) return;
    m_groupId = groupId;
    Q_EMIT groupIdChanged();
}

void SkillGroup::setNumSkillsInGroup(int n)
{
    if (m_numSkillsInGroup == n) return;
    m_numSkillsInGroup = n;
    Q_EMIT numSkillsInGroupChanged();
}


} // namespace
