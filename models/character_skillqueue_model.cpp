#include "character_skillqueue_model.h"

namespace EM {

CharacterSkillQueueModel::CharacterSkillQueueModel(QObject *parent)
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
        {SkillPointsInSkill, QByteArrayLiteral("skillPointsInSkill")},
        {SkillPointsInLevel, QByteArrayLiteral("skillPointsInLevel")},
        {SkillPointsForNextLevel, QByteArrayLiteral("skillPointsForNextLevel")},
        //
        {IsInQueue,          QByteArrayLiteral("isInQueue")},
        {PositionInQueue,    QByteArrayLiteral("positionInQueue")},
        {TrainingLevel,      QByteArrayLiteral("trainingLevel")},
        {TrainingLevelRoman, QByteArrayLiteral("trainingLevelRoman")},
        {TrainPercent,       QByteArrayLiteral("trainPercent")},
        {TrainStartDate,     QByteArrayLiteral("trainStartDate")},
        {TrainFinishDate,    QByteArrayLiteral("trainFinishDate")},
        //
        {SkillPointsPerHour, QByteArrayLiteral("skillPointsPerHour")},
        {TrainingTimeLeft,   QByteArrayLiteral("trainingTimeLeft")},
    };
}

QHash<int, QByteArray> CharacterSkillQueueModel::roleNames() const
{
    return m_roles;
}

int CharacterSkillQueueModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    QMutexLocker lock(&m_mutex);
    return m_data.count();
}

QVariant CharacterSkillQueueModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) {
        return ret;
    }

    QMutexLocker lock(&m_mutex);

    int row = index.row();
    if ((row < 0) || (row >= m_data.size())) {
        return ret;
    }

    const CharacterSkill &skill = m_data.at(row);
    switch(role) {
    case Qt::DisplayRole:     ret = skill.skillName();          break;
    case SkillId:             ret = skill.skillId();            break;
    case SkillName:           ret = skill.skillName();          break;
    case SkillGroupName:      ret = skill.skillGroupName();     break;
    case SkillGroupId:        ret = skill.skillGroupId();       break;
    case PrimaryAttribute:    ret = skill.primaryAttribute();   break;
    case SecondaryAttribute:  ret = skill.secondaryAttribute(); break;
    case SkillTimeConstant:   ret = skill.skillTimeConstant();  break;
    case TrainedLevel:        ret = skill.trainedLevel();       break;
    case ActiveLevel:         ret = skill.activeLevel();        break;
    case SkillPointsInSkill:  ret = skill.skillPointsInSkill(); break;
    case SkillPointsInLevel:  ret = skill.skillPointsInLevel(); break;
    case SkillPointsForNextLevel: ret = skill.skillPointsForNextLevel(); break;
    case IsInQueue:           ret = skill.isInQueue();          break;
    case PositionInQueue:     ret = skill.positionInQueue();    break;
    case TrainingLevel:       ret = skill.trainingLevel();      break;
    case TrainingLevelRoman:  ret = skill.trainingLevelRoman(); break;
    case TrainPercent:        ret = skill.trainPercent();       break;
    case TrainStartDate:      ret = skill.trainStartDate();     break;
    case TrainFinishDate:     ret = skill.trainFinishDate();    break;
    case SkillPointsPerHour:  ret = skill.skillPointsPerHour(); break;
    case TrainingTimeLeft:    ret = skill.trainingTimeLeft();   break;
    }

    return ret;
}

void CharacterSkillQueueModel::setModelData(const QVector<CharacterSkill> skills, const CharacterSkillQueue &queue)
{
    {
        QMutexLocker lock(&m_mutex);
        beginResetModel();
        m_data.clear();
        // fill m_data - loop over a skill queue in order, and add all skills in it to model
        // we expect skillqueue to be already compacted here, and skills vector prepared
        // after Character::calcSkillQueue()
        for (const CharacterSkillQueueItem &qitem: queue) {
            // find corresponding skill in skills vector
            for (const CharacterSkill &sk: skills) {
                if (sk.skillId() == qitem.skillId) {
                    // create a copy of CharacterSkill object
                    CharacterSkill queueSkill(sk);
                    // and update it with new skill queue info parameters
                    queueSkill.clearQueueInfo();
                    double skillPointsTrainedSinceLevel = static_cast<double>(queueSkill.skillPointsInSkill() - qitem.levelStartSp);
                    double skillPointsNeededTotal = static_cast<double>(qitem.levelEndSp - qitem.levelStartSp);
                    double trainPercent = skillPointsTrainedSinceLevel / skillPointsNeededTotal;
                    queueSkill.setQueueInfo(qitem.queuePosition, qitem.trainingLevel, trainPercent,
                                            qitem.startDate, qitem.finishDate);
                    m_data.push_back(queueSkill);
                    break;
                }
            }
        }
    }
    endResetModel();
}

} // namespace EM
