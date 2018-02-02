#ifndef H_EM_CHARACTER_SKILL
#define H_EM_CHARACTER_SKILL

#include <QObject>
#include "skill.h"


namespace EM {


class CharacterSkill: public SkillTemplate
{
    Q_OBJECT
    Q_PROPERTY(int     trainedLevel       READ trainedLevel       WRITE setTrainedLevel       NOTIFY trainedLevelChanged)
    Q_PROPERTY(int     activeLevel        READ activeLevel        WRITE setActiveLevel        NOTIFY activeLevelChanged)
    Q_PROPERTY(quint64 skillPointsInSkill READ skillPointsInSkill WRITE setSkillPointsInSkill NOTIFY skillPointsInSkillChanged)

public:
    CharacterSkill();
    CharacterSkill(const CharacterSkill& other);
    CharacterSkill& operator=(const CharacterSkill& other);
    CharacterSkill& operator=(CharacterSkill&& other);
    bool operator==(const CharacterSkill& other) const;

public Q_SLOTS:
    int trainedLevel() const;
    void setTrainedLevel(int lvl);

    int activeLevel() const;
    void setActiveLevel(int lvl);

    quint64 skillPointsInSkill() const;
    void setSkillPointsInSkill(quint64 sp);

Q_SIGNALS:
    void trainedLevelChanged();
    void activeLevelChanged();
    void skillPointsInSkillChanged();

protected:
    int m_trainedLevel = 0;
    int m_activeLevel = 0;
    quint64 m_skillPointsInSkill = 0;
};


} // namespace


#endif
