#ifndef H_CHARACTER_SKILLQUEUE_INFO
#define H_CHARACTER_SKILLQUEUE_INFO

#include <QString>
#include <QDateTime>
#include <QDataStream>

namespace EM {


class CharacterSkillQueueInfo
{
public:
    CharacterSkillQueueInfo();
    CharacterSkillQueueInfo(const CharacterSkillQueueInfo &other) = default;
    CharacterSkillQueueInfo(CharacterSkillQueueInfo &&other) = default;
    CharacterSkillQueueInfo &operator=(const CharacterSkillQueueInfo &other) = default;
    CharacterSkillQueueInfo &operator=(CharacterSkillQueueInfo &&other) = default;

public:
    int trainingLevel = 0;
    int queuePosition = -1;
    quint64 levelStartSp = 0;
    quint64 levelEndSp = 0;
    QDateTime startDate;
    QDateTime finishDate;
};

}

QDataStream& operator<<(QDataStream &stream, const EM::CharacterSkillQueueInfo &info);
QDataStream& operator>>(QDataStream &stream, EM::CharacterSkillQueueInfo &info);

#endif
