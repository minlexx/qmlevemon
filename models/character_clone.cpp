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
    m_location     = other.m_location;
    m_implants     = other.m_implants;
    return (*this);
}

CharacterClone &CharacterClone::operator=(CharacterClone &&other)
{
    if (this == &other) return (*this);
    m_cloneName    = std::move(other.m_cloneName);
    m_cloneId      = std::move(other.m_cloneId);
    m_location     = std::move(other.m_location);
    m_implants     = std::move(other.m_implants);
    return (*this);
}

bool CharacterClone::operator==(const CharacterClone &other) const
{
    const bool id_equals = (m_cloneId == other.m_cloneId);
    bool implants_equal = true; // all implants are same
    if (m_implants.count() != other.m_implants.count()) {
        implants_equal = false; // implant count differs
    } else {
        // count is equal; compare each implant
        int i = 0;
        while (i < m_implants.count()) {
            const InvType &myImp = m_implants.at(i);
            const InvType &otherImp = other.m_implants.at(i);
            if (myImp != otherImp) {
                implants_equal = false;
                break;
            }
            i++;
        }
    }
    return (id_equals && implants_equal);
}

bool CharacterClone::operator!=(const CharacterClone &other) const
{
    return !(this->operator==(other));
}


QString CharacterClone::cloneName() const { return m_cloneName; }

quint64 CharacterClone::cloneId() const { return m_cloneId; }

QObject *CharacterClone::locationObj() { return static_cast<QObject *>(&m_location); }

const QObject *CharacterClone::locationObj() const { return static_cast<const QObject *>(&m_location); }

EveLocation *CharacterClone::location() { return &m_location; }

const EveLocation *CharacterClone::location() const { return &m_location; }

QObject *CharacterClone::implantsModel() { return static_cast<QObject *>(&m_implants); }

const QObject *CharacterClone::implantsModel() const { return static_cast<const QObject *>(&m_implants); }

CharacterImplantsGroup *CharacterClone::implants() { return &m_implants; }

const CharacterImplantsGroup *CharacterClone::implants() const { return &m_implants; }

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

void CharacterClone::setLocation(const EveLocation &loc)
{
    if (m_location == loc) return;
    m_location = loc;
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
    stream << clone.m_location;
    stream << clone.m_implants;
    return stream;
}

QDataStream& operator>>(QDataStream &stream, EM::CharacterClone &clone)
{
    stream >> clone.m_cloneName;
    stream >> clone.m_cloneId;
    stream >> clone.m_location;
    stream >> clone.m_implants;
    return stream;
}

QDebug operator<<(QDebug stream, const EM::CharacterClone &clone)
{
    stream.nospace() << "[Clone " << clone.m_cloneName << "/"
                     << clone.m_cloneId << "; "
                     << clone.implants()->rowCount()
                     << " implants]";
    return stream;
}
