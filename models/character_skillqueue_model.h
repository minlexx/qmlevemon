#ifndef H_EM_CHARACTER_SKILLQUEUE_MODEL
#define H_EM_CHARACTER_SKILLQUEUE_MODEL

#include <QObject>
#include <QAbstractListModel>
#include <QMutex>
#include <QByteArray>
#include <QVector>

#include "character_skill.h"


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
        TrainingLevel,
        TrainingLevelRoman,
        SkillPointsInSkill,
        SkillPointsInLevel,
        // properties from skill queue
        IsInQueue,
        PositionInQueue,
        TrainPercent,
        TrainStartDate,
        TrainFinishDate
    };

public:
    explicit CharacterSkillQueueModel(QObject *parent = nullptr);

public:  // reimplmented interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
    // other funcs
    void setSkillQueue(const QVector<CharacterSkill> &queue);

private:
    QHash<int, QByteArray> m_roles;
    QVector<CharacterSkill> m_data;
    mutable QMutex m_mutex;
};


}


#endif
