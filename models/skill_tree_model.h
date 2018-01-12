#ifndef H_SKILL_TREE_MODEL
#define H_SKILL_TREE_MODEL

#include "skill.h"
#include <QAbstractItemModel>
#include <QHash>
#include <QByteArray>

namespace EM {


class SkillTreeModel: public QAbstractItemModel
{
    Q_OBJECT

    enum Roles {
        Type = Qt::UserRole + 1
    };

public:
    explicit SkillTreeModel(QObject *parent = Q_NULLPTR);
    virtual ~SkillTreeModel();

    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE virtual QModelIndex parent(const QModelIndex &child) const override;

public:
    bool load();

protected:
    QHash<int, QByteArray> m_roles;
};


} // namespace

#endif
