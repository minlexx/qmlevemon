#ifndef H_CHARACTER_SKILLQUEUE_INFO
#define H_CHARACTER_SKILLQUEUE_INFO

#include <QString>
#include <QDateTime>
#include <QDataStream>

namespace EM {


class CharacterSkillQueueItem
{
public:
    CharacterSkillQueueItem();
    CharacterSkillQueueItem(const CharacterSkillQueueItem &other) = default;
    CharacterSkillQueueItem(CharacterSkillQueueItem &&other) = default;
    CharacterSkillQueueItem &operator=(const CharacterSkillQueueItem &other) = default;
    CharacterSkillQueueItem &operator=(CharacterSkillQueueItem &&other) = default;

public:
    quint64 skillId = 0;
    int trainingLevel = 0;
    int queuePosition = -1; // -1 = not in queue
    quint64 levelStartSp = 0;
    quint64 levelEndSp = 0;
    quint64 trainingStartSp = 0;
    QDateTime startDate;
    QDateTime finishDate;
};

}

QDataStream& operator<<(QDataStream &stream, const EM::CharacterSkillQueueItem &info);
QDataStream& operator>>(QDataStream &stream, EM::CharacterSkillQueueItem &info);

#endif
