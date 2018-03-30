#include "character_skillqueue.h"

namespace EM {

CharacterSkillQueueItem::CharacterSkillQueueItem()
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
