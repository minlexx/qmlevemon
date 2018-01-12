#include "skill_tree_model.h"

namespace EM {


SkillTreeModel::SkillTreeModel(QObject *parent):
    QAbstractItemModel(parent)
{
    // init role names
    m_roles.insert(Qt::DisplayRole, "displayName");
    m_roles.insert(Roles::Type, "type");
}


SkillTreeModel::~SkillTreeModel() { }


QHash<int, QByteArray> SkillTreeModel::roleNames() const
{
    return m_roles;
}


int SkillTreeModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}


int SkillTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}


bool SkillTreeModel::hasChildren(const QModelIndex &parent) const
{
    return false;
}


QVariant SkillTreeModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}


QModelIndex SkillTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column, nullptr);
}


QModelIndex SkillTreeModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}


bool SkillTreeModel::load()
{
    return true;
}


} // namespace
