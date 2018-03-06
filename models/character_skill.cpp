#include <utility>
#include <QDataStream>

#include "character_skill.h"
#include "model_manager.h"
#include "skill_tree_model.h"


namespace EM {


CharacterSkill::CharacterSkill()
    : SkillTemplate()
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
     static_cast<SkillTemplate *>(this)->operator=(*other);
}

CharacterSkill &CharacterSkill::operator=(const CharacterSkill &other)
{
    if (this == &other) return (*this);
    // SkillTemplate::operator=(other);
    static_cast<SkillTemplate *>(this)->operator=(other);
    m_trainedLevel = other.m_trainedLevel;
    m_activeLevel = other.m_activeLevel;
    m_skillPointsInSkill = other.m_skillPointsInSkill;
    return (*this);
}

CharacterSkill &CharacterSkill::operator=(CharacterSkill &&other)
{
    //SkillTemplate::operator=(std::move(other));
    static_cast<SkillTemplate *>(this)->operator=(std::move(other));
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
    quint64 skillId = 0;
    QString skillName;
    // SkillTemplate properties
    stream >> skillId;
    stream >> skillName;
    stream >> ui64;      // group id
    stream >> s;         // group name

    // fill in skill group
    EM::SkillTreeModel *skillTree = EM::ModelManager::instance()->skillTreeModel();
    if (skillTree) {
        const EM::SkillTemplate *tmpl = skillTree->findSkill(skillId);
        if (tmpl) {
            skill = EM::CharacterSkill(tmpl); // create from template
            // ^^ fills group, attributes, difficulty
        }
    }
    // just in case...
    skill.setSkillId(skillId);
    skill.setSkillName(skillName);

    stream >> i;     skill.setPrimaryAttribute(i);
    stream >> i;     skill.setSecondaryAttribute(i);
    stream >> f;     skill.setSkillTimeConstant(f);
    // CharacterSkill properties
    stream >> i;     skill.setTrainedLevel(i);
    stream >> i;     skill.setActiveLevel(i);
    stream >> ui64;  skill.setSkillPointsInSkill(ui64);
    return stream;
}


QDebug operator<<(QDebug stream, const EM::CharacterSkill &skill)
{
    stream << "[CharacterSkill " << skill.skillName() << "/" << skill.skillId();
    stream << " / diff: " << skill.skillTimeConstant();
    stream << "]";
    return stream;
}
