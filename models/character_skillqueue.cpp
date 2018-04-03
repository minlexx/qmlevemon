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


// container-like interface
QVector<CharacterSkillQueueItem>::iterator CharacterSkillQueue::begin()
{
    return m_queue.begin();
}

QVector<CharacterSkillQueueItem>::const_iterator CharacterSkillQueue::begin() const
{
    return m_queue.begin();
}

QVector<CharacterSkillQueueItem>::iterator CharacterSkillQueue::end()
{
    return m_queue.end();
}

QVector<CharacterSkillQueueItem>::const_iterator CharacterSkillQueue::end() const
{
    return m_queue.end();
}

int CharacterSkillQueue::size() const
{
    return m_queue.size();
}

QVector<CharacterSkillQueueItem>::iterator CharacterSkillQueue::erase(
        QVector<CharacterSkillQueueItem>::iterator ibegin,
        QVector<CharacterSkillQueueItem>::iterator iend)
{
    return m_queue.erase(ibegin, iend);
}

QVector<CharacterSkillQueueItem>::iterator CharacterSkillQueue::erase(
        QVector<CharacterSkillQueueItem>::iterator pos)
{
    return m_queue.erase(pos);
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
