#include <QDateTime>
#include "character_skillqueue_model.h"

namespace EM {

CharacterSkillQueueModel::CharacterSkillQueueModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_mutex(QMutex::Recursive)
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
        {LadderPercentStart, QByteArrayLiteral("ladderPercentStart")},
        {LadderPercentEnd,   QByteArrayLiteral("ladderPercentEnd")},
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
    case LadderPercentStart:  ret = skill.ladderPercentStart(); break;
    case LadderPercentEnd:    ret = skill.ladderPercentEnd();   break;
    }

    return ret;
}

void CharacterSkillQueueModel::setModelData(const QVector<CharacterSkill> skills, const CharacterSkillQueue &queue)
{
    QVector<CharacterSkill> newData;
    // Loop over a skill queue in order, and add all skills in it to model
    // we expect skillqueue to be already compacted here, and skills vector prepared
    // after Character::calcSkillQueue()
    QDateTime dtCur = QDateTime::currentDateTime();
    double dtotalSecondsToQueueEnd = static_cast<double>(dtCur.secsTo(queue.queueFinishDate()));
    for (const CharacterSkillQueueItem &qitem: queue) {
        // find corresponding skill in skills vector
        for (const CharacterSkill &sk: skills) {
            if (sk.skillId() == qitem.skillId) {
                // create a copy of CharacterSkill object
                CharacterSkill queueSkill(sk);
                // and update it with new skill queue info parameters
                queueSkill.clearQueueInfo();
                double skillPointsTrainedSinceLevel = 0.0;
                double skillPointsInSkill = static_cast<double>(queueSkill.skillPointsInSkill());
                if (skillPointsInSkill > qitem.levelStartSp) {
                    skillPointsTrainedSinceLevel = skillPointsInSkill - static_cast<double>(qitem.levelStartSp);
                }
                double skillPointsNeededTotal = static_cast<double>(qitem.levelEndSp - qitem.levelStartSp);
                double trainPercent = skillPointsTrainedSinceLevel / skillPointsNeededTotal;

                double ladderStart = 0.0;
                if (qitem.queuePosition > 0) {
                    ladderStart = static_cast<double>(dtCur.secsTo(qitem.startDate)) / dtotalSecondsToQueueEnd;
                }
                double ladderEnd = static_cast<double>(dtCur.secsTo(qitem.finishDate)) / dtotalSecondsToQueueEnd;
                queueSkill.setQueueInfo(qitem.queuePosition, qitem.trainingLevel, trainPercent,
                                        qitem.startDate, qitem.finishDate,
                                        ladderStart, ladderEnd);
                newData.push_back(queueSkill);
                break;
            }
        }
    }

    mergeNewData(newData);
}

void CharacterSkillQueueModel::mergeNewData(const QVector<CharacterSkill> &newData)
{
    QMutexLocker lock(&m_mutex);

    // loop over new data; replace first existing data items with new ones
    for (int i = 0; i < newData.size(); i++) {
        const CharacterSkill &newSkill = newData.at(i);
        if (i < m_data.size()) {
            m_data.replace(i, newSkill);
        }
    }

    if (newData.size() <= m_data.size()) {
        // new data is shorter than existing or equal, and
        // first NEW_SIZE existing items were replaced
        //  new       {N N N N}
        //  existing  {E E E E E E E E}
        Q_EMIT dataChanged(index(0), index(newData.size() - 1));
        // now we need to remove extra existing data
        beginRemoveRows(QModelIndex(), newData.size(), m_data.size() - 1);
        m_data.remove(newData.size(), m_data.size() - newData.size());
        endRemoveRows();
    } else {
        // new data is larger than existing
        // first EXISTING_SIZE items were replaced
        //  new       {N N N N N N N N}
        //  existing  {E E E E}
        if (m_data.size() > 0) {
            Q_EMIT dataChanged(index(0), index(m_data.size() - 1));
        }
        // insert newer rows from new data to the end of existing
        beginInsertRows(QModelIndex(), m_data.size(), newData.size() - 1);
        for (int i = m_data.size(); i < newData.size(); i++) {
            m_data.append(newData[i]);
        }
        endInsertRows();
    }
}

} // namespace EM
