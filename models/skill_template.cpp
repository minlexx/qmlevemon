#include <utility>
#include "skill_template.h"

namespace EM {


CharacterAttributeID charAttributeFromID(int id)
{
    switch (id) {
    case static_cast<int>(CharacterAttributeID::CHARISMA):     return CharacterAttributeID::CHARISMA; break;
    case static_cast<int>(CharacterAttributeID::INTELLIGENCE): return CharacterAttributeID::INTELLIGENCE; break;
    case static_cast<int>(CharacterAttributeID::MEMORY):       return CharacterAttributeID::MEMORY; break;
    case static_cast<int>(CharacterAttributeID::PERCEPTION):   return CharacterAttributeID::PERCEPTION; break;
    case static_cast<int>(CharacterAttributeID::WILLPOWER):    return CharacterAttributeID::WILLPOWER; break;
    }
    return CharacterAttributeID::INVALID;
}


QString charAttributeName(CharacterAttributeID attr)
{
    switch (attr) {
    case CharacterAttributeID::CHARISMA: return QStringLiteral("Charisma"); break;
    case CharacterAttributeID::INTELLIGENCE: QStringLiteral("Intelligence"); break;
    case CharacterAttributeID::MEMORY: return QStringLiteral("Memory"); break;
    case CharacterAttributeID::PERCEPTION: return QStringLiteral("Perception"); break;
    case CharacterAttributeID::WILLPOWER: return QStringLiteral("Willpower"); break;
    }
    return QStringLiteral("Invalid!");
}


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
    m_primaryAttribute = other.m_primaryAttribute;
    m_secondaryAttribute = other.m_secondaryAttribute;
    m_skillTimeConstant = other.m_skillTimeConstant;
    return (*this);
}


SkillTemplate& SkillTemplate::operator=(SkillTemplate&& other)
{
    m_skillId = std::move(other.m_skillId);
    m_skillName = std::move(other.m_skillName);
    m_skillGroup = std::move(other.m_skillGroup);
    m_primaryAttribute = std::move(other.m_primaryAttribute);
    m_secondaryAttribute = std::move(other.m_secondaryAttribute);
    m_skillTimeConstant = std::move(other.m_skillTimeConstant);
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

int SkillTemplate::primaryAttribute() const { return m_primaryAttribute; }

int SkillTemplate::secondaryAttribute() const { return m_secondaryAttribute; }

float SkillTemplate::skillTimeConstant() const { return m_skillTimeConstant; }


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


void SkillTemplate::setPrimaryAttribute(int att)
{
    if (att == m_primaryAttribute) return;
    m_primaryAttribute = att;
    Q_EMIT primaryAttributeChanged();
}


void SkillTemplate::setSecondaryAttribute(int att)
{
    if (att == m_secondaryAttribute) return;
    m_secondaryAttribute = att;
    Q_EMIT secondaryAttributeChanged();
}


void SkillTemplate::setSkillTimeConstant(float difficulty)
{
    if (difficulty == m_skillTimeConstant) return;
    m_skillTimeConstant = difficulty;
    Q_EMIT skillTimeConstantChanged();
}


} // namespace
