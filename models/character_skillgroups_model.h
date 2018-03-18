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
            m_id(id), m_name(name), m_skillsInGroupTotal(numSkillInGroup)  {}
        ModelData(const ModelData &) = default;
        ModelData(ModelData &&) = default;
        ModelData &operator=(const ModelData &) = default;
        ModelData &operator=(ModelData &&) = default;
        bool operator<(const ModelData &o) const;

    public:
        quint64 m_id = 0;
        QString m_name;
        int m_skillsInGroupTrained = 0;
        int m_skillsInGroupTotal = 0;
        quint64 m_skillPointsInGroup = 0;
        int m_numSkillsInQueue = 0;
        int m_numSkillsInTraining = 0;
    };

public:
    explicit CharacterSkillGroupsModel(QObject *parent = nullptr);
    // unlike bse class, we need to make this class copyable (bullshit!)
    //CharacterSkillGroupsModel(const CharacterSkillGroupsModel &other);
    //CharacterSkillGroupsModel(CharacterSkillGroupsModel &&other);
    //CharacterSkillGroupsModel &operator=(const CharacterSkillGroupsModel &other);
    //CharacterSkillGroupsModel &operator=(CharacterSkillGroupsModel &&other);

public:  // reimplmented interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
    void setFromSkills(const QVector<CharacterSkill> &skills);
    void setActiveTrainingGroupId(quint64 groupId);

private:
    QHash<int, QByteArray> m_roles;
    QVector<ModelData> m_data;
    mutable QMutex m_mutex;
};

}

#endif
