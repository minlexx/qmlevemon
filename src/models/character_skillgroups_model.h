#ifndef H_EM_CHARACTER_SKILLGROUPS_MODEL
#define H_EM_CHARACTER_SKILLGROUPS_MODEL

#include <QObject>
#include <QAbstractListModel>
#include <QHash>
#include <QByteArray>
#include <QMutex>

#include "character_skill.h"

namespace EM {

class CharacterSkillGroupsModel: public QAbstractListModel
{
    Q_OBJECT

    enum Roles {
        GroupName = Qt::UserRole + 1,
        GroupId,
        SkillsInGroupTrained,
        SkillsInGroupTotal,
        SkillPointsInGroup,
        NumSkillsInQueue,
        NumSkillsInTraining
    };

    class ModelData {
    public:
        ModelData() {}
        ModelData(quint64 id, const QString &name, int numSkillInGroup):
            m_groupId(id), m_groupName(name), m_skillsInGroupTotal(numSkillInGroup)  {}
        ModelData(const ModelData &) = default;
        ModelData(ModelData &&) = default;
        ModelData &operator=(const ModelData &) = default;
        ModelData &operator=(ModelData &&) = default;
        bool operator<(const ModelData &o) const;
        bool operator==(const ModelData &o) const;

    public:
        quint64 m_groupId = 0;
        QString m_groupName;
        int m_skillsInGroupTrained = 0;
        int m_skillsInGroupTotal = 0;
        quint64 m_skillPointsInGroup = 0;
        int m_numSkillsInQueue = 0;
        int m_numSkillsInTraining = 0;
    };

public:
    explicit CharacterSkillGroupsModel(QObject *parent = nullptr);

public:  // reimplmented interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
    void setFromSkills(const QVector<CharacterSkill> &skills);

protected:
    void mergeModelData(const QVector<ModelData> &newData);

private:
    QHash<int, QByteArray> m_roles;
    QVector<ModelData> m_data;
    mutable QMutex m_mutex;
};

}

#endif
