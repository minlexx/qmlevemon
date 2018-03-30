#include "character_skillqueue.h"


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

void CharacterSkillQueue::addItem(const EM::CharacterSkillQueueItem &item)
{
    m_queue.push_back(item);
}

void CharacterSkillQueue::addItem(CharacterSkillQueueItem &&item)
{
    m_queue.push_back(item);
}

void CharacterSkillQueue::calc()
{
    //
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
