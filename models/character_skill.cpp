#include <utility>
#include <QDataStream>

#include "character_skill.h"
#include "model_manager.h"
#include "skill_tree_model.h"
#include "formulas.h"


namespace EM {


CharacterSkill::CharacterSkill()
    : SkillTemplate()
{
    //
}

CharacterSkill::CharacterSkill(const CharacterSkill &other)
    : SkillTemplate(other)
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
    m_trainedLevel        = other.m_trainedLevel;
    m_activeLevel         = other.m_activeLevel;
    m_skillPointsInSkill  = other.m_skillPointsInSkill;
    m_skillPointsInLevel  = other.m_skillPointsInLevel;
    m_qinfo               = other.m_qinfo;
    return (*this);
}

CharacterSkill &CharacterSkill::operator=(CharacterSkill &&other)
{
    //SkillTemplate::operator=(std::move(other));
    static_cast<SkillTemplate *>(this)->operator=(std::move(other));
    m_trainedLevel        = std::move(other.m_trainedLevel);
    m_activeLevel         = std::move(other.m_activeLevel);
    m_skillPointsInSkill  = std::move(other.m_skillPointsInSkill);
    m_skillPointsInLevel  = std::move(other.m_skillPointsInLevel);
    m_qinfo               = std::move(other.m_qinfo);
    return (*this);
}

bool CharacterSkill::operator==(const CharacterSkill &other) const
{
    return ((m_skillId == other.m_skillId) &&
            (m_trainedLevel == other.m_trainedLevel) &&
            (m_activeLevel == other.m_activeLevel) &&
            (m_skillPointsInSkill == other.m_skillPointsInSkill)
           );
}

int CharacterSkill::trainedLevel() const { return m_trainedLevel; }

void CharacterSkill::setTrainedLevel(int lvl)
{
    if (lvl != m_trainedLevel) {
        m_trainedLevel = lvl;
        // calculate skillpoints total needed for this skill level
        int skill_rank = static_cast<int>(skillTimeConstant());
        m_skillPointsInLevel = skill_points_needed_for_skill_level(skill_rank, trainedLevel());
        Q_EMIT trainedLevelChanged();
    }
}

int CharacterSkill::activeLevel() const { return m_activeLevel; }

QString CharacterSkill::trainingLevelRoman() const
{
    int training_level = m_activeLevel + 1;
    if (training_level > 5) {
        training_level = 5;
    }
    QString ret;
    switch (training_level) {
    case 1: ret = QLatin1String("I"); break;
    case 2: ret = QLatin1String("II"); break;
    case 3: ret = QLatin1String("III"); break;
    case 4: ret = QLatin1String("IV"); break;
    case 5: ret = QLatin1String("V"); break;
    }
    return ret;
}

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


quint64 CharacterSkill::skillPointsInLevel() const
{
    return m_skillPointsInLevel;
}

CharacterSkillQueueInfo CharacterSkill::queueInfo() const
{
    return m_qinfo;
}

void CharacterSkill::setQueueInfo(const CharacterSkillQueueInfo &qinfo)
{
    m_qinfo = qinfo;
    Q_EMIT queueChanged();
}

bool CharacterSkill::isInQueue() const
{
    return (m_qinfo.queuePosition >= 0);
}

int CharacterSkill::positionInQueue() const
{
    return m_qinfo.queuePosition;
}

double CharacterSkill::trainPercent() const
{
    double start_sp = static_cast<double>(m_qinfo.levelStartSp);
    double end_sp = static_cast<double>(m_qinfo.levelEndSp);
    double cur_sp = static_cast<double>(m_skillPointsInSkill);
    double dist_passed = cur_sp - start_sp;
    double dist_total = end_sp - start_sp;
    return dist_passed / dist_total;
}

QDateTime CharacterSkill::trainStartDate() const
{
    return m_qinfo.startDate;
}

QDateTime CharacterSkill::trainFinishDate() const
{
    return m_qinfo.finishDate;
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
    // stream << skill.skillPointsInLevel(); // <-- this field is calculated from trainedLevel, no need to save
    stream << skill.queueInfo();
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
    EM::CharacterSkillQueueInfo qinfo;

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
    stream >> i;     skill.setTrainedLevel(i);  // also calculates skillPointsInLevel
    stream >> i;     skill.setActiveLevel(i);
    stream >> ui64;  skill.setSkillPointsInSkill(ui64);
    stream >> qinfo; skill.setQueueInfo(qinfo);
    return stream;
}


QDebug operator<<(QDebug stream, const EM::CharacterSkill &skill)
{
    stream << "<CharacterSkill " << skill.skillName() << "/" << skill.skillId();
    stream << " lvl " << skill.activeLevel();
    stream << " / rank: " << skill.skillTimeConstant();
    stream << ">";
    return stream;
}
