#ifndef H_EM_CHARACTER_SKILL
#define H_EM_CHARACTER_SKILL

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include "skill_template.h"

class QDataStream;


namespace EM {


class CharacterSkill: public SkillTemplate
{
    Q_OBJECT
    Q_PROPERTY(int      trainedLevel       READ trainedLevel       WRITE setTrainedLevel       NOTIFY trainedLevelChanged)
    Q_PROPERTY(int      activeLevel        READ activeLevel        WRITE setActiveLevel        NOTIFY activeLevelChanged)
    //Q_PROPERTY(int      trainingLevel      READ trainingLevel      NOTIFY queueChanged)
    Q_PROPERTY(QString  trainingLevelRoman READ trainingLevelRoman NOTIFY activeLevelChanged)
    Q_PROPERTY(quint64  skillPointsInSkill READ skillPointsInSkill WRITE setSkillPointsInSkill NOTIFY skillPointsInSkillChanged)
    Q_PROPERTY(quint64  skillPointsInLevel READ skillPointsInLevel NOTIFY trainedLevelChanged)
    // properties from skill queue
    //Q_PROPERTY(bool       isInQueue        READ isInQueue          NOTIFY queueChanged)
    //Q_PROPERTY(int        positionInQueue  READ positionInQueue    NOTIFY queueChanged)
    //Q_PROPERTY(double     trainPercent     READ trainPercent       NOTIFY queueChanged)
    //Q_PROPERTY(QDateTime  trainStartDate   READ trainStartDate     NOTIFY queueChanged)
    //Q_PROPERTY(QDateTime  trainFinishDate  READ trainFinishDate    NOTIFY queueChanged)

public:
    CharacterSkill();
    CharacterSkill(const CharacterSkill &other);
    CharacterSkill(CharacterSkill &&other);
    CharacterSkill(const SkillTemplate *other);
    CharacterSkill& operator=(const CharacterSkill &other);
    CharacterSkill& operator=(CharacterSkill &&other);
    bool operator==(const CharacterSkill &other) const;

public Q_SLOTS:
    int trainedLevel() const;
    void setTrainedLevel(int lvl);

    int activeLevel() const;
    QString trainingLevelRoman() const;
    void setActiveLevel(int lvl);

    quint64 skillPointsInSkill() const;
    void setSkillPointsInSkill(quint64 sp);

    quint64 skillPointsInLevel() const;

    //bool isInQueue() const;
    //int positionInQueue() const;
    //double trainPercent() const;
    //QDateTime trainStartDate() const;
    //QDateTime trainFinishDate() const;
    //int trainingLevel() const;

Q_SIGNALS:
    void trainedLevelChanged();
    void activeLevelChanged();
    void skillPointsInSkillChanged();
    //void queueChanged();

protected:
    int m_trainedLevel = 0;
    int m_activeLevel = 0;
    quint64 m_skillPointsInSkill = 0;
    quint64 m_skillPointsInLevel = 0;
};


} // namespace


QDataStream& operator<<(QDataStream &stream, const EM::CharacterSkill &skill);
QDataStream& operator>>(QDataStream &stream, EM::CharacterSkill &skill);
QDebug operator<<(QDebug stream, const EM::CharacterSkill &skill);


#endif
