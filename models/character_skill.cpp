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

CharacterSkill::CharacterSkill(CharacterSkill&& other)
{
    (*this) = std::move(other);
}

CharacterSkill &CharacterSkill::operator=(const CharacterSkill &other)
{
    if (this == &other) return (*this);
    SkillTemplate::operator=(other);
    m_trainedLevel = other.m_trainedLevel;
    m_activeLevel = other.m_activeLevel;
    return (*this);
}

CharacterSkill &CharacterSkill::operator=(CharacterSkill &&other)
{
    SkillTemplate::operator=(other);
    m_trainedLevel = std::move(other.m_trainedLevel);
    m_activeLevel = std::move(other.m_activeLevel);
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