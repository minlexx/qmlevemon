#include "character_clone.h"
#include <QDataStream>


namespace EM {


CharacterClone::CharacterClone(QObject *parent)
    : QObject(parent)
{
}

CharacterClone::CharacterClone(const CharacterClone &other)
    : QObject(other.parent())
{
    (*this) = other;
}

CharacterClone::CharacterClone(CharacterClone &&other)
{
    (*this) = std::move(other);
}

CharacterClone &CharacterClone::operator=(const CharacterClone &other)
{
    if (this == &other) return (*this);
    m_cloneName    = other.m_cloneName;
    m_cloneId      = other.m_cloneId;
    m_locationId   = other.m_locationId;
    m_locationName = other.m_locationName;
    m_locationType = other.m_locationType;
    m_implants     = other.m_implants;
    return (*this);
}

CharacterClone &CharacterClone::operator=(CharacterClone &&other)
{
    if (this == &other) return (*this);
    m_cloneName    = std::move(other.m_cloneName);
    m_cloneId      = std::move(other.m_cloneId);
    m_locationId   = std::move(other.m_locationId);
    m_locationName = std::move(other.m_locationName);
    m_locationType = std::move(other.m_locationType);
    m_implants     = std::move(other.m_implants);
    return (*this);
}

bool CharacterClone::operator==(const CharacterClone &other) const
{
    return m_cloneId == other.m_cloneId;
}

bool CharacterClone::operator!=(const CharacterClone &other) const
{
    return m_cloneId != other.m_cloneId;
}


QString CharacterClone::cloneName() const { return m_cloneName; }

quint64 CharacterClone::cloneId() const { return m_cloneId; }

quint64 CharacterClone::locationId() const { return m_locationId; }

QString CharacterClone::locationName() const { return m_locationName; }

QString CharacterClone::locationType() const { return m_locationType; }

QObject *CharacterClone::implantsModel() { return static_cast<QObject *>(&m_implants); }

CharacterImplantsGroup *CharacterClone::implants() { return &m_implants; }

void CharacterClone::setCloneName(const QString &name)
{
    if (name == m_cloneName) return;
    m_cloneName = name;
    Q_EMIT cloneChanged();
}

void CharacterClone::setCloneId(quint64 id)
{
    if (id == m_cloneId) return;
    m_cloneId = id;
    Q_EMIT cloneChanged();
}

void CharacterClone::setLocationId(quint64 id)
{
    if (id == m_locationId) return;
    m_locationId = id;
    Q_EMIT locationChanged();
}

void CharacterClone::setLocationName(const QString &name)
{
    if (name == m_locationName) return;
    m_locationName = name;
    Q_EMIT locationChanged();
}

void CharacterClone::setLocationType(const QString &typ)
{
    if (typ == m_locationType) return;
    m_locationType = typ;
    Q_EMIT locationChanged();
}

void CharacterClone::setImplantsGroup(const CharacterImplantsGroup &imps)
{
    m_implants = imps;
    Q_EMIT implantsChanged();
}

void CharacterClone::setImplantsGroup(CharacterImplantsGroup &&imps)
{
    m_implants = std::move(imps);
    Q_EMIT implantsChanged();
}


} // namespace EM


QDataStream& operator<<(QDataStream &stream, const EM::CharacterClone &clone)
{
    stream << clone.m_cloneName;
    stream << clone.m_cloneId;
    stream << clone.m_locationId;
    stream << clone.m_locationName;
    stream << clone.m_locationType;
    stream << clone.m_implants;
    return stream;
}

QDataStream& operator>>(QDataStream &stream, EM::CharacterClone &clone)
{
    stream >> clone.m_cloneName;
    stream >> clone.m_cloneId;
    stream >> clone.m_locationId;
    stream >> clone.m_locationName;
    stream >> clone.m_locationType;
    stream >> clone.m_implants;
    return stream;
}

QDebug operator<<(QDebug stream, const EM::CharacterClone &clone)
{
    stream.nospace() << "[Clone " << clone.m_cloneName << "/" << clone.m_cloneId << "]";
    return stream;
}
