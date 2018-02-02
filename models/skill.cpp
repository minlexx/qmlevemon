#include <utility>
#include "skill.h"

namespace EM {


SkillTemplate::SkillTemplate():
    QObject(nullptr)
{
}


SkillTemplate::SkillTemplate(const SkillTemplate& other):
    QObject(nullptr)
{
    (*this) = other;
}


SkillTemplate::SkillTemplate(SkillTemplate&& other)
{
    (*this) = std::move(other);
}


SkillTemplate& SkillTemplate::operator=(const SkillTemplate& other)
{
    if (this == &other) return (*this);
    m_skillId = other.m_skillId;
    m_skillName = other.m_skillName;
    m_skillGroup = other.m_skillGroup;
    return (*this);
}


SkillTemplate& SkillTemplate::operator=(SkillTemplate&& other)
{
    m_skillId = std::move(other.m_skillId);
    m_skillName = std::move(other.m_skillName);
    m_skillGroup = std::move(other.m_skillGroup);
    return (*this);
}


bool SkillTemplate::operator==(const SkillTemplate &other) const
{
    if (this == &other) return true;
    return (m_skillId == other.m_skillId);
}


QString SkillTemplate::skillName() const { return m_skillName; }

quint64 SkillTemplate::skillId() const { return m_skillId; }

const SkillGroup *SkillTemplate::skillGroup() const { return m_skillGroup; }


QString SkillTemplate::skillGroupName() const
{
    if (!m_skillGroup) return QString();
    return m_skillGroup->groupName();
}


quint64 SkillTemplate::skillGroupId() const
{
    if (!m_skillGroup) return 0;
    return m_skillGroup->groupId();
}


void SkillTemplate::setSkillName(const QString& name)
{
    if (m_skillName == name) return;
    m_skillName = name;
    Q_EMIT skillNameChanged();
}


void SkillTemplate::setSkillId(quint64 id)
{
    if (m_skillId == id) return;
    m_skillId = id;
    Q_EMIT skillIdChanged();
}


void SkillTemplate::setSkillGroup(const SkillGroup *group)
{
    if (m_skillGroup == group) return;
    m_skillGroup = group;
    Q_EMIT skillGroupChanged();
    Q_EMIT skillGroupNameChanged();
    Q_EMIT skillGroupIdChanged();
}


} // namespace
