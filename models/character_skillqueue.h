#ifndef H_CHARACTER_SKILLQUEUE_INFO
#define H_CHARACTER_SKILLQUEUE_INFO

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QDataStream>


// forward-declare classes in a namespace
namespace EM {
    class CharacterSkillQueueItem;
    class CharacterSkillQueue;
}  // namespace EM


// forward-declare QDataStream operators for CharacterSkillQueue for friend to work
QDataStream& operator<<(QDataStream& stream, const EM::CharacterSkillQueue& queue);
QDataStream& operator>>(QDataStream& stream, EM::CharacterSkillQueue& queue);


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


class CharacterSkillQueue
{
    friend QDataStream& (::operator<<)(QDataStream& stream, const CharacterSkillQueue& queue);
    friend QDataStream& (::operator>>)(QDataStream& stream, CharacterSkillQueue &queue);

public:
    CharacterSkillQueue();
    CharacterSkillQueue(const CharacterSkillQueue &other) = default;
    CharacterSkillQueue(CharacterSkillQueue &&other) = default;
    CharacterSkillQueue &operator=(const CharacterSkillQueue &other) = default;
    CharacterSkillQueue &operator=(CharacterSkillQueue &&other) = default;

public:
    void clear();
    void addItem(const EM::CharacterSkillQueueItem &item);
    void addItem(EM::CharacterSkillQueueItem &&item);
    void calc();
    quint64 currentTrainingSkillId() const;
    QDateTime queueFinishDate() const;

protected:
    QVector<EM::CharacterSkillQueueItem> m_queue;
};


}  // namespace EM


QDataStream& operator<<(QDataStream& stream, const EM::CharacterSkillQueueItem& info);
QDataStream& operator>>(QDataStream& stream, EM::CharacterSkillQueueItem& info);


#endif
