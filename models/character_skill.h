#ifndef H_EM_CHARACTER_SKILL
#define H_EM_CHARACTER_SKILL

#include <QObject>
#include "skill.h"


namespace EM {


class CharacterSkill: public SkillTemplate
{
    Q_OBJECT
    Q_PROPERTY(int     trainedLevel     READ trainedLevel   WRITE setTrainedLevel  NOTIFY trainedLevelChanged)
    Q_PROPERTY(int     activeLevel      READ activeLevel    WRITE setActiveLevel   NOTIFY activeLevelChanged)
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

Q_SIGNALS:
    void trainedLevelChanged();
    void activeLevelChanged();

protected:
    int m_trainedLevel = 0;
    int m_activeLevel = 0;
};


} // namespace


#endif
