#include <QLoggingCategory>
#include <QDebug>

#include "skill_tree_model.h"
#include "db/db.h"
#include "qmlevemon_app.h"

Q_LOGGING_CATEGORY(logStree, "evemon.skilltree")

namespace EM {


SkillTreeModel::SkillTreeNode::~SkillTreeNode()
{
    for (SkillTreeNode *node: children) {
        delete node;
    }
    children.clear();
}

void SkillTreeModel::SkillTreeNode::appendChild(SkillTreeNode *node)
{
    node->parent = this;
    children.append(node);
}

QString SkillTreeModel::SkillTreeNode::name() const
{
    switch (type) {
    case NodeType::Group: return group->groupName();
    case NodeType::Skill: return skill->skillName();
    default: break;
    }
    return QString();
}

quint64 SkillTreeModel::SkillTreeNode::id() const
{
    switch (type) {
    case NodeType::Group: return group->groupId();
    case NodeType::Skill: return skill->skillId();
    default: break;
    }
    return 0;
}



SkillTreeModel::SkillTreeModel(QObject *parent):
    QAbstractItemModel(parent),
    m_rootNode(new SkillTreeNode())
{
    // init role names
    m_roles.insert(Qt::DisplayRole, "name");
    m_roles.insert(Roles::Id, "id");
    m_roles.insert(Roles::Name, "name");
    m_roles.insert(Roles::Type, "type");
}


SkillTreeModel::~SkillTreeModel()
{
    delete m_rootNode;
}


QHash<int, QByteArray> SkillTreeModel::roleNames() const
{
    return m_roles;
}


int SkillTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        // child node?
        SkillTreeNode *node = modelDataFromIndex(parent);
        return node->children.size();
    }
    return m_rootNode->children.size();
}


int SkillTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}


SkillTreeModel::SkillTreeNode *SkillTreeModel::modelDataFromIndex(const QModelIndex& idx) const
{
    auto *ret = static_cast<SkillTreeModel::SkillTreeNode *>(idx.internalPointer());
    if (!ret) {
        qCWarning(logStree) << Q_FUNC_INFO << "returning nullptr!";
    }
    return ret;
}


QVariant SkillTreeModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    SkillTreeModel::SkillTreeNode *modelData = modelDataFromIndex(index);
    if (!modelData) {
        qCWarning(logStree) << Q_FUNC_INFO << "no data for index" << index;
        return ret;
    }
    switch(role) {
    case Qt::DisplayRole:
    case Roles::Name:
        ret = modelData->name();
        break;
    case Roles::Id:
        ret = modelData->id();
        break;
    case Roles::Type:
        ret = static_cast<int>(modelData->type);
        break;
    }
    if (!ret.isValid()) {
        qCWarning(logStree) << "returning invalid QVariant for index" << index;
    }
    return ret;
}


QVariant SkillTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(role)
    if (orientation == Qt::Horizontal) {
        if (section == 0) {
            return QVariant(tr("Name"));
        }
    }
    return QVariant();
}


QModelIndex SkillTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    SkillTreeModel::SkillTreeNode *linkedNode = nullptr;
    if (parent.isValid()) {
        // need to find model data for group item first
        int parent_idx = parent.row();
        SkillTreeModel::SkillTreeNode *groupNode = m_rootNode->children.at(parent_idx);
        linkedNode = groupNode->children.at(row);
    } else {
        // this is an index for group itself
        linkedNode = m_rootNode->children.at(row);
    }
    QModelIndex index = createIndex(row, column, linkedNode);
    return index;
}


QModelIndex SkillTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return QModelIndex();
    SkillTreeModel::SkillTreeNode *modelData = modelDataFromIndex(child);
    if (modelData->type == SkillTreeModel::SkillTreeNode::NodeType::Group) {
        // skill groups have no parent
        return QModelIndex();
    }
    // but skills do have a parent - skill group
    quint64 parent_id = modelData->skill->skillGroupId();
    int parent_row = 0;
    for (SkillTreeModel::SkillTreeNode *node: m_rootNode->children) {
        if (node->id() == parent_id) {
            return createIndex(parent_row, 0, node);
        }
        parent_row++;
    }
    // something went wrong
    return QModelIndex();
}


bool SkillTreeModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) return false;
    SkillTreeModel::SkillTreeNode *modelData = modelDataFromIndex(parent);
    if (modelData->type == SkillTreeModel::SkillTreeNode::NodeType::Group) {
        // all skill groups can have children
        return true;
    }
    // other type - skill - always has no children
    return false;
}

bool SkillTreeModel::load()
{
    int numGroups = 0, numSkills = 0;
    Db *db = globalAppInstance()->database();

    // load skill groups
    QJsonArray jskillGroups = db->loadSkillGroups();
    for (auto it = jskillGroups.constBegin(); it != jskillGroups.constEnd(); it++) {
        numGroups++;
        const QJsonObject& jobj = (*it).toObject();
        // qCDebug(logStree) << "  load group: " << jobj;
        // ^^ QJsonObject({"id":"255","name":"Gunnery"})
        SkillGroup *skillGroup = new SkillGroup();
        skillGroup->setGroupId(jobj.value(QLatin1String("id")).toVariant().toULongLong());
        skillGroup->setGroupName(jobj.value(QLatin1String("name")).toString());
        if (!m_skillGroups.contains(skillGroup->groupId())) {
            m_skillGroups.insert(skillGroup->groupId(), skillGroup);
        } else {
            qCWarning(logStree) << "Skill group:" << skillGroup->groupId() << "already loaded! Error in SDE?";
        }

        // create skillgroup node
        SkillTreeModel::SkillTreeNode *node = new SkillTreeModel::SkillTreeNode(skillGroup);
        m_rootNode->appendChild(node);
    }

    // load skills
    for (SkillTreeModel::SkillTreeNode *groupNode: m_rootNode->children) {
        // load skills for each group
        quint64 groupId = groupNode->group->groupId();
        QJsonArray jskills = db->loadSkillsInGroup(groupId);
        for (auto it = jskills.constBegin(); it != jskills.constEnd(); it++) {
            numSkills++;
            const QJsonObject& jobj = (*it).toObject();
            const SkillGroup *skillGroup = m_skillGroups[groupId];
            SkillTemplate *skillTemplate = new SkillTemplate();
            skillTemplate->setSkillGroup(skillGroup);
            skillTemplate->setSkillId(jobj.value(QLatin1String("id")).toVariant().toULongLong());
            skillTemplate->setSkillName(jobj.value(QLatin1String("name")).toString());
            if (!m_skillTemplates.contains(skillTemplate->skillId())) {
                m_skillTemplates.insert(skillTemplate->skillId(), skillTemplate);
            } else {
                qCWarning(logStree) << "Skill template:" << skillTemplate->skillId() << "already loaded! Error in SDE?";
            }

            // create skill entry
            SkillTreeModel::SkillTreeNode *skillNode = new SkillTreeModel::SkillTreeNode(skillTemplate);
            groupNode->appendChild(skillNode);
        }
    }

    qCDebug(logStree) << "loaded " << numSkills << " skills in " << numGroups << " groups.";
    return true;
}


QVector<SkillGroup *> SkillTreeModel::getSkillGroups() const
{
    QVector<SkillGroup *> ret;
    for (auto it = m_skillGroups.constBegin(); it != m_skillGroups.constEnd(); it++) {
        ret.append(it.value());
    }
    return ret;
}

const SkillTemplate *SkillTreeModel::findSkill(quint64 skillId) const
{
    if (!m_skillTemplates.contains(skillId)) {
        qCWarning(logStree) << Q_FUNC_INFO << "cannot find template for skill:" << skillId;
        return nullptr;
    }
    SkillTemplate *tmpl = m_skillTemplates[skillId];
    return tmpl;
}

const SkillGroup *SkillTreeModel::findSkillGroupForSkillId(quint64 skillId) const
{
    if (!m_skillTemplates.contains(skillId)) {
        qCWarning(logStree) << Q_FUNC_INFO << "cannot find template for skill:" << skillId;
        return nullptr;
    }
    SkillTemplate *tmpl = m_skillTemplates[skillId];
    return tmpl->skillGroup();
}


} // namespace EM
