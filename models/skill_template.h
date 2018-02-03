#ifndef H_SKILL
#define H_SKILL

#include <QObject>
#include "skill_group.h"


namespace EM {


enum class CharacterAttributeID {
    INVALID = 0,
    CHARISMA = 164,
    INTELLIGENCE = 165,
    MEMORY = 166,
    PERCEPTION = 167,
    WILLPOWER = 168
};


CharacterAttributeID charAttributeFromID(int id);
QString charAttributeName(CharacterAttributeID attr);


class SkillTemplate: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString   skillName           READ skillName           WRITE setSkillName  NOTIFY skillNameChanged)
    Q_PROPERTY(quint64   skillId             READ skillId             WRITE setSkillId    NOTIFY skillIdChanged)
    Q_PROPERTY(const SkillGroup *skillGroup  READ skillGroup          WRITE setSkillGroup NOTIFY skillGroupChanged)
    Q_PROPERTY(QString   skillGroupName      READ skillGroupName      NOTIFY skillGroupNameChanged)
    Q_PROPERTY(quint64   skillGroupId        READ skillGroupId        NOTIFY skillGroupIdChanged)
    Q_PROPERTY(int       primaryAttribute    READ primaryAttribute    WRITE setPrimaryAttribute   NOTIFY primaryAttributeChanged)
    Q_PROPERTY(int       secondaryAttribute  READ secondaryAttribute  WRITE setSecondaryAttribute NOTIFY secondaryAttributeChanged)
    Q_PROPERTY(float     skillTimeConstant   READ skillTimeConstant   WRITE setSkillTimeConstant  NOTIFY skillTimeConstantChanged)

public:
    SkillTemplate();
    SkillTemplate(const SkillTemplate& other);
    SkillTemplate(SkillTemplate&& other);
    SkillTemplate& operator=(const SkillTemplate& other);
    SkillTemplate& operator=(SkillTemplate&& other);
    bool operator==(const SkillTemplate& other) const;

    QString skillName() const;
    quint64 skillId() const;
    const SkillGroup *skillGroup() const;
    QString skillGroupName() const;
    quint64 skillGroupId() const;
    int primaryAttribute() const;
    int secondaryAttribute() const;
    float skillTimeConstant() const;

public Q_SLOTS:
    void setSkillName(const QString& name);
    void setSkillId(quint64 id);
    void setSkillGroup(const SkillGroup *group);
    void setPrimaryAttribute(int att);
    void setSecondaryAttribute(int att);
    void setSkillTimeConstant(float difficulty);

Q_SIGNALS:
    void skillNameChanged();
    void skillIdChanged();
    void skillGroupChanged();
    void skillGroupNameChanged();
    void skillGroupIdChanged();
    void primaryAttributeChanged();
    void secondaryAttributeChanged();
    void skillTimeConstantChanged();

protected:
    QString m_skillName;
    quint64 m_skillId = 0;
    const SkillGroup *m_skillGroup = nullptr;
    int m_primaryAttribute = 0;
    int m_secondaryAttribute = 0;
    float m_skillTimeConstant = 0.0f;
};


} // namespace

#endif
