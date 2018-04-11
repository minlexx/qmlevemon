#ifndef H_EM_CHARACTER_SKILLQUEUE_MODEL
#define H_EM_CHARACTER_SKILLQUEUE_MODEL

#include <QObject>
#include <QAbstractListModel>
#include <QMutex>
#include <QByteArray>
#include <QVector>

#include "character_skill.h"
#include "character_skillqueue.h"


namespace EM {


class CharacterSkillQueueModel: public QAbstractListModel
{
    Q_OBJECT

    enum Roles {
        // properties from skill template
        SkillId = Qt::UserRole + 1,
        SkillName,
        SkillGroupName,
        SkillGroupId,
        PrimaryAttribute,
        SecondaryAttribute,
        SkillTimeConstant,
        // properties from skill
        TrainedLevel,
        ActiveLevel,
        SkillPointsInSkill,
        SkillPointsInLevel,
        SkillPointsForNextLevel,
        // properties from skill queue
        IsInQueue,
        PositionInQueue,
        TrainingLevel,
        TrainingLevelRoman,
        TrainPercent,
        TrainStartDate,
        TrainFinishDate,
        // calculated properties
        SkillPointsPerHour,
        TrainingTimeLeft,
        LadderPercentStart,
        LadderPercentEnd
    };

public:
    explicit CharacterSkillQueueModel(QObject *parent = nullptr);

public:  // reimplmented interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
    // other funcs
    void setModelData(const QVector<CharacterSkill> skills, const CharacterSkillQueue& queue);

private:
    QHash<int, QByteArray> m_roles;
    QVector<CharacterSkill> m_data;
    mutable QMutex m_mutex;
};


}


#endif
