#include <QDataStream>
#include <utility>
#include "character_skill.h"


namespace EM {


CharacterSkill::CharacterSkill()
{
    //
}

CharacterSkill::CharacterSkill(const CharacterSkill &other)
{
    (*this) = other;
}

CharacterSkill::CharacterSkill(CharacterSkill &&other)
{
    (*this) = std::move(other);
}

CharacterSkill::CharacterSkill(const SkillTemplate *other)
{
    if (other == nullptr) return;
    static_cast<SkillTemplate>(*this) = (*other);
}

CharacterSkill &CharacterSkill::operator=(const CharacterSkill &other)
{
    if (this == &other) return (*this);
    SkillTemplate::operator=(other);
    m_trainedLevel = other.m_trainedLevel;
    m_activeLevel = other.m_activeLevel;
    m_skillPointsInSkill = other.m_skillPointsInSkill;
    return (*this);
}

CharacterSkill &CharacterSkill::operator=(CharacterSkill &&other)
{
    SkillTemplate::operator=(other);
    m_trainedLevel = std::move(other.m_trainedLevel);
    m_activeLevel = std::move(other.m_activeLevel);
    m_skillPointsInSkill = std::move(other.m_skillPointsInSkill);
    return (*this);
}

bool CharacterSkill::operator==(const CharacterSkill &other) const
{
    return ((m_skillId == other.m_skillId) &&
            (m_trainedLevel == other.m_trainedLevel) &&
            (m_activeLevel == other.m_activeLevel));
}

int CharacterSkill::trainedLevel() const { return m_trainedLevel; }

void CharacterSkill::setTrainedLevel(int lvl)
{
    if (lvl != m_trainedLevel) {
        m_trainedLevel = lvl;
        Q_EMIT trainedLevelChanged();
    }
}

int CharacterSkill::activeLevel() const { return m_activeLevel; }

void CharacterSkill::setActiveLevel(int lvl)
{
    if (lvl != m_activeLevel) {
        m_activeLevel = lvl;
        Q_EMIT activeLevelChanged();
    }
}

quint64 CharacterSkill::skillPointsInSkill() const { return m_skillPointsInSkill; }

void CharacterSkill::setSkillPointsInSkill(quint64 sp) {
    if (sp != m_skillPointsInSkill) {
        m_skillPointsInSkill = sp;
        Q_EMIT skillPointsInSkillChanged();
    }
}


}

QDataStream &operator<<(QDataStream &stream, const EM::CharacterSkill &skill)
{
    // SkillTemplate properties
    stream << skill.skillId();
    stream << skill.skillName();
    stream << skill.skillGroupId();
    stream << skill.skillGroupName();
    stream << skill.primaryAttribute();
    stream << skill.secondaryAttribute();
    stream << skill.skillTimeConstant();
    // CharacterSkill properties
    stream << skill.trainedLevel();
    stream << skill.activeLevel();
    stream << skill.skillPointsInSkill();
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::CharacterSkill &skill)
{
    int i = 0;
    quint64 ui64 = 0;
    float f = 0.0f;
    QString s;
    // SkillTemplate properties
    stream >> ui64;  skill.setSkillId(ui64);
    stream >> s;     skill.setSkillName(s);
    stream >> ui64;  // skill.setSkillGroupId();
    stream >> s;     // skill.setSkillGroupName(s);
    stream >> i;     skill.setPrimaryAttribute(i);
    stream >> i;     skill.setSecondaryAttribute(i);
    stream >> f;     skill.setSkillTimeConstant(f);
    // CharacterSkill properties
    stream >> i;     skill.setTrainedLevel(i);
    stream >> i;     skill.setActiveLevel(i);
    stream >> ui64;  skill.setSkillPointsInSkill(ui64);
    return stream;
}
