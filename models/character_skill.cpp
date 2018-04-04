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
    static_cast<SkillTemplate *>(this)->operator=(other);
    m_trainedLevel        = other.m_trainedLevel;
    m_activeLevel         = other.m_activeLevel;
    m_skillPointsInSkill  = other.m_skillPointsInSkill;
    m_skillPointsInLevel  = other.m_skillPointsInLevel;
    // queue info
    m_positionInQueue     = other.m_positionInQueue;
    m_trainingLevel       = other.m_trainingLevel;
    m_trainPercent        = other.m_trainPercent;
    m_trainStartDate      = other.m_trainStartDate;
    m_trainFinishDate     = other.m_trainFinishDate;
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
    // queue info
    m_positionInQueue     = std::move(other.m_positionInQueue);
    m_trainingLevel       = std::move(other.m_trainingLevel);
    m_trainPercent        = std::move(other.m_trainPercent);
    m_trainStartDate      = std::move(other.m_trainStartDate);
    m_trainFinishDate     = std::move(other.m_trainFinishDate);
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


quint64 CharacterSkill::skillPointsInLevel() const { return m_skillPointsInLevel; }

bool CharacterSkill::isInQueue() const { return m_positionInQueue >= 0; }

int CharacterSkill::positionInQueue() const { return m_positionInQueue; }

int CharacterSkill::trainingLevel() const { return m_trainingLevel; }

QString CharacterSkill::trainingLevelRoman() const
{
    if (m_trainingLevel <= 0) {
        return QString();
    }
    QString ret;
    switch (m_trainingLevel) {
    case 1: ret = QLatin1String("I"); break;
    case 2: ret = QLatin1String("II"); break;
    case 3: ret = QLatin1String("III"); break;
    case 4: ret = QLatin1String("IV"); break;
    case 5: ret = QLatin1String("V"); break;
    default: ret = QString::number(m_trainingLevel, 10); break;
    }
    return ret;
}

double CharacterSkill::trainPercent() const { return m_trainPercent; }

QDateTime CharacterSkill::trainStartDate() const { return m_trainStartDate; }

QDateTime CharacterSkill::trainFinishDate() const { return m_trainFinishDate; }

void CharacterSkill::setQueueInfo(int pos, int trainLevel, double trainPercent,
                                  const QDateTime &startDt, const QDateTime &endDt)
{
    m_positionInQueue = pos;
    m_trainingLevel = trainLevel;
    m_trainPercent = trainPercent;
    m_trainStartDate = startDt;
    m_trainFinishDate = endDt;
    Q_EMIT queueInfoChanged();
}

void CharacterSkill::clearQueueInfo()
{
    m_positionInQueue = -1; // -1 - not in queue
    m_trainingLevel = 0;
    m_trainPercent = 0.0;
    m_trainStartDate = QDateTime();
    m_trainFinishDate = QDateTime();
}

void CharacterSkill::trainLevelUp()
{
    // setters properly emit all signals
    // and recalculate m_skillPointsInLevel
    setActiveLevel(m_activeLevel + 1);
    setTrainedLevel(m_trainedLevel + 1);
}


} // namespace EM



QDataStream &operator<<(QDataStream &stream, const EM::CharacterSkill &skill)
{
    // save SkillTemplate properties
    stream << skill.m_skillId;
    stream << skill.m_skillName;
    stream << skill.m_primaryAttribute;
    stream << skill.m_secondaryAttribute;
    stream << skill.m_skillTimeConstant;
    // CharacterSkill properties
    stream << skill.m_trainedLevel;
    stream << skill.m_activeLevel;
    stream << skill.m_skillPointsInSkill;
    stream << skill.m_skillPointsInLevel;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::CharacterSkill &skill)
{
    quint64 skillId = 0;
    QString skillName;

    // read SkillTemplate properties
    stream >> skillId;
    stream >> skillName;

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

    stream >> skill.m_primaryAttribute;
    stream >> skill.m_secondaryAttribute;
    stream >> skill.m_skillTimeConstant;
    // CharacterSkill properties
    stream >> skill.m_trainedLevel;
    stream >> skill.m_activeLevel;
    stream >> skill.m_skillPointsInSkill;
    stream >> skill.m_skillPointsInLevel;
    return stream;
}


QDebug operator<<(QDebug stream, const EM::CharacterSkill &skill)
{
    stream << "<CharacterSkill " << skill.skillName() << "/" << skill.skillId();
    stream << "/ lvl " << skill.activeLevel();
    //if (skill.trainingLevel() > 0) {
    //    stream << " training lvl:" << skill.trainingLevel();
    //}
    stream << "/ rank:" << skill.skillTimeConstant();
    stream << ">";
    return stream;
}
