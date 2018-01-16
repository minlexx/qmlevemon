#ifndef H_SKILL_TREE_MODEL
#define H_SKILL_TREE_MODEL

#include "skill.h"
#include <QAbstractItemModel>
#include <QHash>
#include <QByteArray>
#include <QVector>

namespace EM {


class SkillTreeModelData;

class SkillTreeModel: public QAbstractItemModel
{
    Q_OBJECT

    enum Roles {
        Id = Qt::UserRole + 1,
        Name,
        Type
    };

public:
    explicit SkillTreeModel(QObject *parent = Q_NULLPTR);
    virtual ~SkillTreeModel();

    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE virtual QModelIndex parent(const QModelIndex &child) const override;
    Q_INVOKABLE virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

public:
    bool load();
    QVector<SkillGroup> getSkillGroups() const;

private:
    bool isValidIndex(const QModelIndex& idx) const;
    SkillTreeModelData *modelDataFromIndex(const QModelIndex& idx) const;

protected:
    QHash<int, QByteArray> m_roles;
    QHash<quint64, SkillGroup> m_skillGroups;  // map groupId => SkillGroup
    QVector<SkillTreeModelData *> m_data;
    QHash<quint64, int> m_idToModelIndexMap;
};


} // namespace

#endif
