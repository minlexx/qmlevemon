#include "character_implants_group.h"


namespace EM {


CharacterImplantsGroup::CharacterImplantsGroup(QObject *parent)
    : QAbstractListModel(parent) // QObject(parent)
{
    m_roles = {
        {Roles::DisplayName,       "displayName"},
        {Roles::TypeId,            "typeId"},
        {Roles::ImplantSlot,       "implantSlot"},
        {Roles::IntelligenceBonus, "intelligenceBonus"},
        {Roles::MemoryBonus,       "memoryBonus"},
        {Roles::PerceptionBonus,   "perceptionBonus"},
        {Roles::WillpowerBonus,    "willpowerBonus"},
        {Roles::CharismaBonus,     "charismaBonus"},
    };
}

CharacterImplantsGroup::CharacterImplantsGroup(const CharacterImplantsGroup &other)
    : QAbstractListModel(other.parent())
{
    (*this) = other;
}

CharacterImplantsGroup::CharacterImplantsGroup(CharacterImplantsGroup &&other)
{
    (*this) = std::move(other);
}

CharacterImplantsGroup &CharacterImplantsGroup::operator=(const CharacterImplantsGroup &other)
{
    if (this == &other) return (*this);
    beginResetModel();
    m_roles    = other.m_roles;
    m_implants = other.m_implants;
    endResetModel();
    return (*this);
}

CharacterImplantsGroup &CharacterImplantsGroup::operator=(CharacterImplantsGroup &&other)
{
    if (this == &other) return (*this);
    beginResetModel();
    m_roles    = std::move(other.m_roles);
    m_implants = std::move(other.m_implants);
    endResetModel();
    return (*this);
}

QHash<int, QByteArray> CharacterImplantsGroup::roleNames() const { return m_roles; }

int CharacterImplantsGroup::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_implants.count();
}

QVariant CharacterImplantsGroup::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) {
        return ret;
    }
    const int row = index.row();
    if ((row < 0) || (row >= m_implants.count())) {
        return ret;
    }
    const auto &item = m_implants.at(row);
    const InvTypeAttribute *attr = nullptr;
    switch (role) {
    case Roles::DisplayName:
        ret = item.typeName();
        break;
    case Roles::TypeId:
        ret = item.typeId();
        break;
    case Roles::ImplantSlot: {
            attr = item.getAttribute(QLatin1String("implantness"));
            if (attr) {
                ret = attr->valueInt;
            }
        } break;
    case Roles::IntelligenceBonus: {
            attr = item.getAttribute(QLatin1String("intelligenceBonus"));
            if (attr) {
                ret = attr->valueFloat;
            }
        } break;
    case Roles::MemoryBonus : {
            attr = item.getAttribute(QLatin1String("memoryBonus"));
            if (attr) {
                ret = attr->valueFloat;
            }
        } break;
    case Roles::PerceptionBonus : {
            attr = item.getAttribute(QLatin1String("perceptionBonus"));
            if (attr) {
                ret = attr->valueFloat;
            }
        } break;
    case Roles::WillpowerBonus : {
            attr = item.getAttribute(QLatin1String("willpowerBonus"));
            if (attr) {
                ret = attr->valueFloat;
            }
        } break;
    case Roles::CharismaBonus : {
            attr = item.getAttribute(QLatin1String("charismaBonus"));
            if (attr) {
                ret = attr->valueFloat;
            }
        } break;
    }
    return ret;
}

void CharacterImplantsGroup::addImplant(const InvType &imp)
{
    beginInsertRows(QModelIndex(), m_implants.size(), m_implants.size());
    m_implants.append(imp);
    endInsertRows();
}

void CharacterImplantsGroup::addImplant(InvType &&imp)
{
    beginInsertRows(QModelIndex(), m_implants.size(), m_implants.size());
    m_implants.append(std::move(imp));
    endInsertRows();
}

void CharacterImplantsGroup::clearImplants()
{
    beginResetModel();
    m_implants.clear();
    endResetModel();
}

int CharacterImplantsGroup::count() const
{
    return m_implants.count();
}

const InvType &CharacterImplantsGroup::at(int i) const
{
    return m_implants.at(i);
}


} // namespace EM


QDataStream &operator<<(QDataStream &stream, const EM::CharacterImplantsGroup &impGroup)
{
    stream << impGroup.m_roles;
    stream << impGroup.m_implants;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::CharacterImplantsGroup &impGroup)
{
    stream >> impGroup.m_roles;
    stream >> impGroup.m_implants;
    return stream;
}

QDebug operator<<(QDebug stream, const EM::CharacterImplantsGroup &impGroup)
{
    stream.nospace() << "CharacterImplantsGroup: [" << impGroup.m_implants << "]";
    return stream;
}
