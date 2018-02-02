#ifndef H_SKILL_TREE_MODEL
#define H_SKILL_TREE_MODEL

#include "skill.h"
#include <QAbstractItemModel>
#include <QHash>
#include <QByteArray>
#include <QVector>

namespace EM {

class SkillTreeModel: public QAbstractItemModel
{
    Q_OBJECT

    enum Roles {
        Id = Qt::UserRole + 1,
        Name,
        Type
    };

    class SkillTreeNode {
    public:
        enum class NodeType { Root, Group, Skill };
        SkillTreeNode(): type(NodeType::Root) { }
        explicit SkillTreeNode(SkillGroup *gr): type(NodeType::Group), group(gr) { }
        explicit SkillTreeNode(SkillTemplate *sk): type(NodeType::Skill), skill(sk) { }
        ~SkillTreeNode();
        void appendChild(SkillTreeNode *node);
        QString name() const;
        quint64 id() const;
    public:
        NodeType type = NodeType::Root;
        SkillGroup *group = nullptr;
        SkillTemplate *skill = nullptr;
        SkillTreeNode *parent = nullptr;
        QList<SkillTreeNode *> children;
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
    QVector<SkillGroup *> getSkillGroups() const;
    const SkillGroup *findSkillGroupForSkillId(quint64 skillId) const;

private:
    SkillTreeNode *modelDataFromIndex(const QModelIndex& idx) const;

protected:
    QHash<int, QByteArray> m_roles;

    // storage for all skill tree data in memmory
    QHash<quint64, SkillGroup *>    m_skillGroups;     // map groupId => SkillGroup
    QHash<quint64, SkillTemplate *> m_skillTemplates;  // map skillId => SkillTemplate

    // actual model data
    SkillTreeNode *m_rootNode = nullptr;
};


} // namespace

#endif
