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
        GroupId
    };

    class ModelData {
    public:
        ModelData() {}
        ModelData(quint64 id, const QString &name): m_id(id), m_name(name) {}
        ModelData(const ModelData &) = default;
        ModelData(ModelData &&) = default;
        ModelData &operator=(const ModelData &) = default;
        ModelData &operator=(ModelData &&) = default;
        bool operator<(const ModelData &o) const;
        quint64 m_id = 0;
        QString m_name;
    };

public:
    explicit CharacterSkillGroupsModel(QObject *parent = nullptr);

public:  // reimplmented interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
    void setFromSkills(const QVector<CharacterSkill> &skills);

private:
    QHash<int, QByteArray> m_roles;
    QVector<ModelData> m_data;
    mutable QMutex m_mutex;
};

}

#endif
