#ifndef H_SKILL_GROUP
#define H_SKILL_GROUP

#include <QObject>


namespace EM {


class SkillGroup: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName NOTIFY groupNameChanged)
    Q_PROPERTY(quint64 groupId   READ groupId   WRITE setGroupId   NOTIFY groupIdChanged)
    Q_PROPERTY(int numSkillsInGroup READ numSkillsInGroup WRITE setNumSkillsInGroup NOTIFY numSkillsInGroupChanged)

public:
    SkillGroup();
    SkillGroup(const SkillGroup& other);
    SkillGroup(SkillGroup&& other);
    SkillGroup& operator=(const SkillGroup& other);
    SkillGroup& operator=(SkillGroup&& other);
    bool operator==(const SkillGroup& other) const;

    QString groupName() const;
    quint64 groupId() const;
    int numSkillsInGroup() const;

public Q_SLOTS:
    void setGroupName(const QString& groupName);
    void setGroupId(quint64 groupId);
    void setNumSkillsInGroup(int n);

Q_SIGNALS:
    void groupNameChanged();
    void groupIdChanged();
    void numSkillsInGroupChanged();

protected:
    QString m_groupName;
    quint64 m_groupId = 0;
    int m_numSkillsInGroup = 0;
};


}


#endif
