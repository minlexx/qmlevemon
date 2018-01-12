#ifndef H_SKILL
#define H_SKILL

#include <QObject>
#include "skill_group.h"


namespace EM {


class SkillTemplate: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString     skillName        READ skillName      WRITE setSkillName  NOTIFY skillNameChanged)
    Q_PROPERTY(quint64     skillId          READ skillId        WRITE setSkillId    NOTIFY skillIdChanged)
    Q_PROPERTY(const SkillGroup *skillGroup READ skillGroup     WRITE setSkillGroup NOTIFY skillGroupChanged)
    Q_PROPERTY(QString     skillGroupName   READ skillGroupName NOTIFY skillGroupNameChanged)

public:
    SkillTemplate();
    SkillTemplate(const SkillTemplate& other);
    SkillTemplate& operator=(const SkillTemplate& other);
    SkillTemplate& operator=(SkillTemplate&& other);

    QString skillName() const;
    quint64 skillId() const;
    const SkillGroup *skillGroup() const;
    QString skillGroupName() const;

public Q_SLOTS:
    void setSkillName(const QString& name);
    void setSkillId(quint64 id);
    void setSkillGroup(const SkillGroup *group);

Q_SIGNALS:
    void skillNameChanged();
    void skillIdChanged();
    void skillGroupChanged();
    void skillGroupNameChanged();

protected:
    QString m_skillName;
    quint64 m_skillId = 0;
    const SkillGroup *m_skillGroup = nullptr;
};


} // namespace

#endif
