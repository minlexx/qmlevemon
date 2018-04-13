#include "invtype.h"

namespace EM {


InvType::InvType(QObject *parent)
    : QObject(parent)
{
}


InvType::InvType(const InvType &other)
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


} // namespace EM
