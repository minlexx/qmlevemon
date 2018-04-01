#include <QDateTime>
#include <QLoggingCategory>
#include "character_skillqueue.h"


Q_LOGGING_CATEGORY(logSQ, "evemon.skillqueue")


namespace EM {


CharacterSkillQueueItem::CharacterSkillQueueItem()
{
    //
}

CharacterSkillQueue::CharacterSkillQueue()
{
    //
}

void CharacterSkillQueue::clear()
{
    m_queue.clear();
}

void CharacterSkillQueue::addItem(const CharacterSkillQueueItem &item)
{
    m_queue.push_back(item);
}

void CharacterSkillQueue::addItem(CharacterSkillQueueItem &&item)
{
    m_queue.push_back(item);
}

QDateTime CharacterSkillQueue::queueFinishDate() const
{
    if (m_queue.isEmpty()) {
        return QDateTime::currentDateTime();
    }
    // finish datetime of last skill in queue
    return m_queue.at(m_queue.size() -1).finishDate;
}

quint64 CharacterSkillQueue::currentTrainingSkillId() const
{
    if (m_queue.size() > 0) {
        return m_queue.at(0).skillId;
    }
    // queue is empty
    return 0;
}


void CharacterSkillQueue::calc()
{
    if (m_queue.size() < 1) {
        // nothing to calculate
        return;
    }

    // ESI API returns times in UTC timezone
    QDateTime dtCur = QDateTime::currentDateTimeUtc();

    QVector<CharacterSkillQueueItem>::iterator iter = m_queue.begin();
    // recalculate queue positions
    int qpos = 0;
    while(iter != m_queue.end()) {
        // remove all skills that are finished already
        if (iter->finishDate < dtCur) {
            qCDebug(logSQ) << "  removing finished skill from queue:" << iter->skillId << iter->finishDate << " < " << dtCur;
            iter = m_queue.erase(iter);
        }
        // overwrite queue pos
        iter->queuePosition = qpos;
        ++iter;
        ++qpos;
    }
}


} // namespace EM



QDataStream &operator<<(QDataStream &stream, const EM::CharacterSkillQueueItem &info)
{
    stream << info.skillId;
    stream << info.trainingLevel;
    stream << info.queuePosition;
    stream << info.levelStartSp;
    stream << info.levelEndSp;
    stream << info.trainingStartSp;
    stream << info.startDate;
    stream << info.finishDate;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::CharacterSkillQueueItem &info)
{
    stream >> info.skillId;
    stream >> info.trainingLevel;
    stream >> info.queuePosition;
    stream >> info.levelStartSp;
    stream >> info.levelEndSp;
    stream >> info.trainingStartSp;
    stream >> info.startDate;
    stream >> info.finishDate;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const EM::CharacterSkillQueue &queue)
{
    stream << queue.m_queue;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::CharacterSkillQueue &queue)
{
    stream >> queue.m_queue;
    return stream;
}
