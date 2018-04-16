#include "character_implants_group.h"


namespace EM {


CharacterImplantsGroup::CharacterImplantsGroup(QObject *parent)
    : QObject(parent)
{
}

CharacterImplantsGroup::CharacterImplantsGroup(const CharacterImplantsGroup &other)
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
    m_implants = other.m_implants;
    return (*this);
}

CharacterImplantsGroup &CharacterImplantsGroup::operator=(CharacterImplantsGroup &&other)
{
    if (this == &other) return (*this);
    m_implants = std::move(other.m_implants);
    return (*this);
}


} // namespace EM


QDataStream &operator<<(QDataStream &stream, const EM::CharacterImplantsGroup &impGroup)
{
    stream << impGroup.m_implants;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::CharacterImplantsGroup &impGroup)
{
    stream >> impGroup.m_implants;
    return stream;
}

QDebug operator<<(QDebug stream, const EM::CharacterImplantsGroup &impGroup)
{
    stream.nospace() << "CharacterImplantsGroup: [" << impGroup.m_implants << "]";
    return stream;
}
