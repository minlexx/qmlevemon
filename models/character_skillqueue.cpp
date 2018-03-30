#include "character_skillqueue_info.h"

namespace EM {

CharacterSkillQueueInfo::CharacterSkillQueueInfo()
{
    //
}

} // namespace EM

QDataStream &operator<<(QDataStream &stream, const EM::CharacterSkillQueueInfo &info)
{
    stream << info.trainingLevel;
    stream << info.queuePosition;
    stream << info.levelStartSp;
    stream << info.levelEndSp;
    stream << info.trainingStartSp;
    stream << info.startDate;
    stream << info.finishDate;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::CharacterSkillQueueInfo &info)
{
    stream >> info.trainingLevel;
    stream >> info.queuePosition;
    stream >> info.levelStartSp;
    stream >> info.levelEndSp;
    stream >> info.trainingStartSp;
    stream >> info.startDate;
    stream >> info.finishDate;
    return stream;
}
