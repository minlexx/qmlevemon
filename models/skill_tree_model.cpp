#include <QLoggingCategory>
#include <QDebug>

#include "skill_tree_model.h"
#include "db/db.h"
#include "qmlevemon_app.h"

Q_LOGGING_CATEGORY(logStree, "evemon.skilltree")

namespace EM {


class SkillTreeModelData
{
public:
    enum EntryType {
        TSkill = 0,
        TGroup = 1
    };

public:
    explicit SkillTreeModelData(EntryType typ): type(typ) { }
    ~SkillTreeModelData() = default;

    QString name() const {
        switch (type) {
        case EntryType::TGroup: return group.groupName(); break;
        case EntryType::TSkill: return skill.skillName(); break;
        }
        return QString();
    }

    quint64 id() const {
        switch (type) {
        case EntryType::TGroup: return group.groupId(); break;
        case EntryType::TSkill: return skill.skillId(); break;
        }
        return 0;
    }

    EntryType type;
    SkillGroup group;
    SkillTemplate skill;
};


SkillTreeModel::SkillTreeModel(QObject *parent):
    QAbstractItemModel(parent)
{
    // init role names
    m_roles.insert(Qt::DisplayRole, "name");
    m_roles.insert(Roles::Id, "id");
    m_roles.insert(Roles::Name, "name");
    m_roles.insert(Roles::Type, "type");
}


SkillTreeModel::~SkillTreeModel() { }


QHash<int, QByteArray> SkillTreeModel::roleNames() const
{
    return m_roles;
}


int SkillTreeModel::rowCount(const QModelIndex &parent) const
{
    return m_data.size();
}


int SkillTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}


bool SkillTreeModel::isValidIndex(const QModelIndex& idx) const
{
    int row = idx.row();
    int column = idx.column();
    if (column > 0) {
        return false;
    }
    if ((row < 0) || (row >= m_data.size())) {
        return false;
    }
    return true;
}


SkillTreeModelData *SkillTreeModel::modelDataFromIndex(const QModelIndex& idx) const
{
    return m_data.at(idx.row());
}


QVariant SkillTreeModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!isValidIndex(index)) return ret;
    SkillTreeModelData *modelData = modelDataFromIndex(index);
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
    return ret;
}


QVariant SkillTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (section == 0) {
            return QVariant(tr("Name"));
        }
    }
    return QVariant();
}


QModelIndex SkillTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    QModelIndex index = createIndex(row, column, nullptr);
    if (!isValidIndex(index)) return QModelIndex();
    return index;
}


QModelIndex SkillTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return QModelIndex();
    SkillTreeModelData *modelData = modelDataFromIndex(child);
    if (modelData->type == SkillTreeModelData::EntryType::TGroup) {
        // skill groups have no parent
        return QModelIndex();
    }
    // but skills do have a parent - skill group
    quint64 parent_id = modelData->skill.skillGroupId();
    if (m_idToModelIndexMap.contains(parent_id)) {
        int model_row = m_idToModelIndexMap[parent_id];
        return createIndex(model_row, 0, nullptr);
    }
    // something went wrong
    return QModelIndex();
}


bool SkillTreeModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) return false;
    SkillTreeModelData *modelData = modelDataFromIndex(parent);
    if (modelData->type == SkillTreeModelData::EntryType::TGroup) {
        // skill groups have no parent
        return false;
    }
    // other type - skill - always has parent skillGroup
    return true;
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
        SkillGroup skillGroup;
        skillGroup.setGroupId(jobj.value(QLatin1String("id")).toVariant().toULongLong());
        skillGroup.setGroupName(jobj.value(QLatin1String("name")).toString());
        m_skillGroups.insert(skillGroup.groupId(), skillGroup);
    }

    // load skills
    for (quint64 groupId: m_skillGroups.keys()) {
        // create group entry
        SkillTreeModelData *groupData = new SkillTreeModelData(SkillTreeModelData::EntryType::TGroup);
        groupData->group = m_skillGroups[groupId];
        m_idToModelIndexMap[groupId] = m_data.size();
        m_data.append(groupData);

        // load skills for each group
        QJsonArray jskills = db->loadSkillsInGroup(groupId);
        for (auto it = jskills.constBegin(); it != jskills.constEnd(); it++) {
            numSkills++;
            const QJsonObject& jobj = (*it).toObject();
            SkillTemplate stmpl;
            stmpl.setSkillGroup(&m_skillGroups[groupId]);
            stmpl.setSkillId(jobj.value(QLatin1String("id")).toVariant().toULongLong());
            stmpl.setSkillName(jobj.value(QLatin1String("name")).toString());

            // create skill entry
            SkillTreeModelData *skillData = new SkillTreeModelData(SkillTreeModelData::EntryType::TSkill);
            skillData->skill = stmpl;
            m_idToModelIndexMap[stmpl.skillId()] = m_data.size();
            m_data.append(skillData);
        }
    }

    qCDebug(logStree) << "loaded " << numSkills << " skills in " << numGroups << " groups.";
    return true;
}


QVector<SkillGroup> SkillTreeModel::getSkillGroups() const
{
    QVector<SkillGroup> ret;
    for (auto it = m_skillGroups.constBegin(); it != m_skillGroups.constEnd(); it++) {
        ret.append(it.value());
    }
    return ret;
}


} // namespace EM
