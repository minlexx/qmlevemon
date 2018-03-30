#include "character_skillqueue_model.h"

EM::CharacterSkillQueueModel::CharacterSkillQueueModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roles = {
        {Qt::DisplayRole,    QByteArrayLiteral("display")},
        {SkillId,            QByteArrayLiteral("skillId")},
        {SkillName,          QByteArrayLiteral("skillName")},
        {SkillGroupName,     QByteArrayLiteral("skillGroupName")},
        {SkillGroupId,       QByteArrayLiteral("skillGroupId")},
        {PrimaryAttribute,   QByteArrayLiteral("primaryAttribute")},
        {SecondaryAttribute, QByteArrayLiteral("secondaryAttribute")},
        {SkillTimeConstant,  QByteArrayLiteral("skillTimeConstant")},
        //
        {TrainedLevel,       QByteArrayLiteral("trainedLevel")},
        {ActiveLevel,        QByteArrayLiteral("activeLevel")},
        {TrainingLevel,      QByteArrayLiteral("trainingLevel")},
        {TrainingLevelRoman, QByteArrayLiteral("trainingLevelRoman")},
        {SkillPointsInSkill, QByteArrayLiteral("skillPointsInSkill")},
        {SkillPointsInLevel, QByteArrayLiteral("skillPointsInLevel")},
        //
        {IsInQueue,          QByteArrayLiteral("isInQueue")},
        {PositionInQueue,    QByteArrayLiteral("positionInQueue")},
        {TrainPercent,       QByteArrayLiteral("trainPercent")},
        {TrainStartDate,     QByteArrayLiteral("trainStartDate")},
        {TrainFinishDate,    QByteArrayLiteral("trainFinishDate")},
    };
}

QHash<int, QByteArray> EM::CharacterSkillQueueModel::roleNames() const
{
    return m_roles;
}

int EM::CharacterSkillQueueModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    QMutexLocker lock(&m_mutex);
    return m_data.count();
}

QVariant EM::CharacterSkillQueueModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) {
        return ret;
    }

    QMutexLocker lock(&m_mutex);

    int row = index.row();
    if ((row <= 0) || (row >= m_data.size())) {
        return ret;
    }

    const CharacterSkill &skill = m_data.at(row);
    switch(role) {
    case Qt::DisplayRole:     ret = skill.skillName(); break;
    case SkillId:             ret = skill.skillId(); break;
    case SkillName:           ret = skill.skillName(); break;
    case SkillGroupName:      ret = skill.skillGroupName(); break;
    case SkillGroupId:        ret = skill.skillGroupId(); break;
    case PrimaryAttribute:    ret = skill.primaryAttribute(); break;
    case SecondaryAttribute:  ret = skill.secondaryAttribute(); break;
    case SkillTimeConstant:   ret = skill.skillTimeConstant(); break;
    case TrainedLevel:        ret = skill.trainedLevel(); break;
    case ActiveLevel:         ret = skill.activeLevel(); break;
    //case TrainingLevel:       ret = skill.trainingLevel(); break;
    case TrainingLevelRoman:  ret = skill.trainingLevelRoman(); break;
    case SkillPointsInSkill:  ret = skill.skillPointsInSkill(); break;
    case SkillPointsInLevel:  ret = skill.skillPointsInLevel(); break;
    //case IsInQueue:           ret = skill.isInQueue(); break;
    //case PositionInQueue:     ret = skill.positionInQueue(); break;
    //case TrainPercent:        ret = skill.trainPercent(); break;
    //case TrainStartDate:      ret = skill.trainStartDate(); break;
    //case TrainFinishDate:     ret = skill.trainFinishDate(); break;
    }

    return ret;
}

void EM::CharacterSkillQueueModel::setSkillQueue(const QVector<EM::CharacterSkill> &queue)
{
    {
        QMutexLocker lock(&m_mutex);
        beginResetModel();
        m_data = queue;
    }
    endResetModel();
}
