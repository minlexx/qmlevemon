#include <QVariant>
#include "invtype.h"

namespace EM {


InvTypeAttribute InvTypeAttribute::fromDatabaseJson(const QJsonObject &jobj)
{
    InvTypeAttribute ret;
    if (!jobj.isEmpty()) {
        ret.attributeId = jobj.value(QLatin1String("attributeid")).toVariant().toULongLong();
        ret.attributeName = jobj.value(QLatin1String("attributename")).toString();
        ret.valueInt = jobj.value(QLatin1String("valueInt")).toInt();
        ret.valueFloat = jobj.value(QLatin1String("valueFloat")).toDouble();
    }
    return ret;
}


InvType InvType::fromDatabaseJson(const QJsonObject &jobj)
{
    InvType ret;
    if (!jobj.isEmpty()) {
        ret.setTypeId(jobj.value(QLatin1String("typeid")).toVariant().toULongLong());
        ret.setTypeName(jobj.value(QLatin1String("typename")).toString());
        ret.setGroupId(jobj.value(QLatin1String("groupid")).toVariant().toULongLong());
        ret.setGroupName(jobj.value(QLatin1String("groupname")).toString());
        ret.setCategoryId(jobj.value(QLatin1String("categoryid")).toVariant().toULongLong());
        ret.setCategoryName(jobj.value(QLatin1String("categoryname")).toString());
    }
    return ret;
}

InvType InvType::fromDatabaseJson(const QJsonObject &jobj, const QJsonArray &jattrs)
{
    InvType ret = InvType::fromDatabaseJson(jobj);
    if (jattrs.count() > 0) {
        // also load attributes
        QVector<InvTypeAttribute> newAttrs;
        newAttrs.reserve(jattrs.count());
        for (const QJsonValue &jval : jattrs) {
            const QJsonObject &jattr = jval.toObject();
            newAttrs.append(InvTypeAttribute::fromDatabaseJson(jattr));
        }
        ret.setAttributes(newAttrs);
    }
    return ret;
}

InvType::InvType(QObject *parent)
    : QObject(parent)
{
}


InvType::InvType(const InvType &other)
    : QObject(other.parent())
{
    (*this) = other;
}

InvType::InvType(InvType &&other)
{
    (*this) = std::move(other);
}

const InvType &InvType::operator=(const InvType &other)
{
    if (this == &other) return (*this);
    m_typeId        = other.m_typeId;
    m_typeName      = other.m_typeName;
    m_groupId       = other.m_groupId;
    m_groupName     = other.m_groupName;
    m_categoryId    = other.m_categoryId;
    m_categoryName  = other.m_categoryName;
    m_attrs         = other.m_attrs;
    return (*this);
}

const InvType &InvType::operator=(InvType &&other)
{
    if (this == &other) return (*this);
    m_typeId        = std::move(other.m_typeId);
    m_typeName      = std::move(other.m_typeName);
    m_groupId       = std::move(other.m_groupId);
    m_groupName     = std::move(other.m_groupName);
    m_categoryId    = std::move(other.m_categoryId);
    m_categoryName  = std::move(other.m_categoryName);
    m_attrs         = std::move(other.m_attrs);
    return (*this);
}

bool InvType::operator==(const InvType &other) const
{
    return (m_typeId == other.m_typeId);
}


bool InvType::isValid() const { return (m_typeId > 0); }
quint64 InvType::typeId() const { return m_typeId; }
QString InvType::typeName() const { return m_typeName; }
quint64 InvType::groupId() const { return m_groupId; }
QString InvType::groupName() const { return m_groupName; }
quint64 InvType::categoryId() const { return m_categoryId; }
QString InvType::categoryName() const { return m_categoryName; }
const QVector<InvTypeAttribute> &InvType::attributes() const { return m_attrs; }
QVector<InvTypeAttribute> &InvType::attributes() { return m_attrs; }

const InvTypeAttribute *InvType::getAttribute(const QString &attributeName) const
{
    const InvTypeAttribute *ret = nullptr;
    for (const InvTypeAttribute &attr: m_attrs) {
        if (attr.attributeName == attributeName) {
            ret = &attr;
            break;
        }
    }
    return ret;
}

const InvTypeAttribute *InvType::getAttribute(quint64 attributeId) const
{
    const InvTypeAttribute *ret = nullptr;
    for (const InvTypeAttribute &attr: m_attrs) {
        if (attr.attributeId == attributeId) {
            ret = &attr;
            break;
        }
    }
    return ret;
}

void InvType::setTypeId(quint64 typeId)
{
    if (m_typeId == typeId) return;
    m_typeId = typeId;
    Q_EMIT typeIdChanged(m_typeId);
}

void InvType::setTypeName(QString typeName)
{
    if (m_typeName == typeName) return;
    m_typeName = typeName;
    Q_EMIT typeNameChanged(m_typeName);
}

void InvType::setGroupId(quint64 groupId)
{
    if (m_groupId == groupId) return;
    m_groupId = groupId;
    Q_EMIT groupIdChanged(m_groupId);
}

void InvType::setGroupName(QString groupName)
{
    if (m_groupName == groupName) return;
    m_groupName = groupName;
    Q_EMIT groupNameChanged(m_groupName);
}

void InvType::setCategoryId(quint64 categoryId)
{
    if (m_categoryId == categoryId) return;
    m_categoryId = categoryId;
    Q_EMIT categoryIdChanged(m_categoryId);
}

void InvType::setCategoryName(QString categoryName)
{
    if (m_categoryName == categoryName) return;
    m_categoryName = categoryName;
    Q_EMIT categoryNameChanged(m_categoryName);
}

void InvType::setAttributes(const QVector<InvTypeAttribute> attrs)
{
    m_attrs = attrs;
    Q_EMIT attributesChanged();
}


} // namespace EM


QDataStream &operator<<(QDataStream &stream, const EM::InvTypeAttribute &attr)
{
    stream << attr.attributeId;
    stream << attr.attributeName;
    stream << attr.valueInt;
    stream << attr.valueFloat;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::InvTypeAttribute &attr)
{
    stream >> attr.attributeId;
    stream >> attr.attributeName;
    stream >> attr.valueInt;
    stream >> attr.valueFloat;
    return stream;
}


QDataStream &operator<<(QDataStream &stream, const EM::InvType &item)
{
    stream << item.m_typeId;
    stream << item.m_typeName;
    stream << item.m_groupId;
    stream << item.m_groupName;
    stream << item.m_categoryId;
    stream << item.m_categoryName;
    stream << item.m_attrs;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::InvType &item)
{
    stream >> item.m_typeId;
    stream >> item.m_typeName;
    stream >> item.m_groupId;
    stream >> item.m_groupName;
    stream >> item.m_categoryId;
    stream >> item.m_categoryName;
    stream >> item.m_attrs;
    return stream;
}

QDebug operator<<(QDebug stream, const EM::InvType &item)
{
    stream.nospace() << "InvType [" << item.m_typeName << "/" << item.m_typeId
                     << ", " << item.m_attrs.size() << " attrs"
                     << "]";
    return stream;
}
