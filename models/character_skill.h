#ifndef H_EM_CHARACTER_SKILL
#define H_EM_CHARACTER_SKILL

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include "skill_template.h"

class QDataStream;

namespace EM { class CharacterSkill; }

QDataStream& operator<<(QDataStream &stream, const EM::CharacterSkill &skill);
QDataStream& operator>>(QDataStream &stream, EM::CharacterSkill &skill);


namespace EM {


class CharacterSkill: public SkillTemplate
{
    Q_OBJECT
    Q_PROPERTY(int      trainedLevel         READ trainedLevel       WRITE setTrainedLevel       NOTIFY trainedLevelChanged)
    Q_PROPERTY(int      activeLevel          READ activeLevel        WRITE setActiveLevel        NOTIFY activeLevelChanged)
    Q_PROPERTY(quint64  skillPointsInSkill   READ skillPointsInSkill WRITE setSkillPointsInSkill NOTIFY skillPointsInSkillChanged)
    Q_PROPERTY(quint64  skillPointsInLevel   READ skillPointsInLevel NOTIFY trainedLevelChanged)
    // properties from skill queue
    Q_PROPERTY(bool       isInQueue          READ isInQueue          NOTIFY queueInfoChanged)
    Q_PROPERTY(int        positionInQueue    READ positionInQueue    NOTIFY queueInfoChanged)
    Q_PROPERTY(int        trainingLevel      READ trainingLevel      NOTIFY queueInfoChanged)
    Q_PROPERTY(QString    trainingLevelRoman READ trainingLevelRoman NOTIFY queueInfoChanged)
    Q_PROPERTY(double     trainPercent       READ trainPercent       NOTIFY queueInfoChanged)
    Q_PROPERTY(QDateTime  trainStartDate     READ trainStartDate     NOTIFY queueInfoChanged)
    Q_PROPERTY(QDateTime  trainFinishDate    READ trainFinishDate    NOTIFY queueInfoChanged)

public:
    CharacterSkill();
    CharacterSkill(const CharacterSkill &other);
    CharacterSkill(CharacterSkill &&other);
    CharacterSkill(const SkillTemplate *other);
    CharacterSkill& operator=(const CharacterSkill &other);
    CharacterSkill& operator=(CharacterSkill &&other);
    bool operator==(const CharacterSkill &other) const;

public Q_SLOTS:
    int       trainedLevel() const;
    void      setTrainedLevel(int lvl);

    int       activeLevel() const;
    void      setActiveLevel(int lvl);

    quint64   skillPointsInSkill() const;
    void      setSkillPointsInSkill(quint64 sp);

    quint64   skillPointsInLevel() const;

    // properties from skill queue
    bool      isInQueue() const;
    int       positionInQueue() const;
    int       trainingLevel() const;
    QString   trainingLevelRoman() const;
    double    trainPercent() const;
    QDateTime trainStartDate() const;
    QDateTime trainFinishDate() const;
    void      setQueueInfo(bool inQueue, int pos, int trainLevel, double trainPercent, const QDateTime &startDt, const QDateTime &endDt);

Q_SIGNALS:
    void trainedLevelChanged();
    void activeLevelChanged();
    void skillPointsInSkillChanged();
    void queueInfoChanged();

protected:
    int m_trainedLevel = 0;
    int m_activeLevel = 0;
    quint64 m_skillPointsInSkill = 0;
    quint64 m_skillPointsInLevel = 0;

    // members to store info about training queue
    bool m_isInQueue = false;
    int m_positionInQueue = -1;
    int m_trainingLevel = 0;
    double m_trainPercent = 0.0;
    QDateTime m_trainStartDate;
    QDateTime m_trainFinishDate;

    friend QDataStream& (::operator<<)(QDataStream &stream, const EM::CharacterSkill &skill);
    friend QDataStream& (::operator>>)(QDataStream &stream, EM::CharacterSkill &skill);
};


} // namespace


QDataStream& operator<<(QDataStream &stream, const EM::CharacterSkill &skill);
QDataStream& operator>>(QDataStream &stream, EM::CharacterSkill &skill);
QDebug operator<<(QDebug stream, const EM::CharacterSkill &skill);


#endif
