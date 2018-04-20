#include <QDataStream>
#include "eve_location.h"


namespace EM {


EveLocation::EveLocation(QObject *parent)
    : QObject(parent)
{
    //
}

EveLocation::EveLocation(const EveLocation &other)
    : QObject(other.parent())
{
    (*this) = other;
}

EveLocation::EveLocation(EveLocation &&other)
    : QObject(other.parent())
{
    (*this) = std::move(other);
}

EveLocation &EveLocation::operator=(const EveLocation &other)
{
    if (this == &other) return (*this);
    m_locationId    = other.m_locationId;
    m_typeId        = other.m_typeId;
    m_solarSystemId = other.m_solarSystemId;
    m_name          = other.m_name;
    m_x             = other.m_x;
    m_y             = other.m_y;
    m_z             = other.m_z;
    return (*this);
}

EveLocation &EveLocation::operator=(EveLocation &&other)
{
    if (this == &other) return (*this);
    m_locationId    = std::move(other.m_locationId);
    m_typeId        = std::move(other.m_typeId);
    m_solarSystemId = std::move(other.m_solarSystemId);
    m_name          = std::move(other.m_name);
    m_x             = std::move(other.m_x);
    m_y             = std::move(other.m_y);
    m_z             = std::move(other.m_z);
    return (*this);
}

bool EveLocation::operator==(const EveLocation &other) const
{
    return m_locationId == other.m_locationId;
}

bool EveLocation::operator!=(const EveLocation &other) const
{
    return !((*this) == other);
}

quint64 EveLocation::locationId() const
{
    return m_locationId;
}

void EveLocation::setLocationId(quint64 id)
{
    if (m_locationId != id) {
        m_locationId = id;
        Q_EMIT locationIdChanged();
    }
}

quint64 EveLocation::typeId() const
{
    return m_typeId;
}

void EveLocation::setTypeId(quint64 id)
{
    if (m_typeId != id) {
        m_typeId = id;
        Q_EMIT typeIdChanged();
    }
}

quint64 EveLocation::solarSystemId() const
{
    return m_solarSystemId;
}

void EveLocation::setSolarSystemId(quint64 id)
{
    if (m_solarSystemId != id) {
        m_solarSystemId = id;
        Q_EMIT solarSystemIdChanged();
    }
}

QString EveLocation::name() const
{
    return m_name;
}

void EveLocation::setName(const QString &s)
{
    if (m_name != s) {
        m_name = s;
        Q_EMIT nameChanged();
    }
}

double EveLocation::x() const
{
    return m_x;
}

double EveLocation::y() const
{
    return m_y;
}

double EveLocation::z() const
{
    return m_z;
}

void EveLocation::setCoords(double x_, double y_, double z_)
{
    if (!qFuzzyCompare(m_x, x_) || !qFuzzyCompare(m_y, y_) || !qFuzzyCompare(m_z, z_)) {
        m_x = x_;
        m_y = y_;
        m_z = z_;
        Q_EMIT coordsChanged();
    }
}


} // namespace EM


QDataStream &operator<<(QDataStream &stream, const EM::EveLocation &loc)
{
    stream << loc.m_locationId;
    stream << loc.m_typeId;
    stream << loc.m_solarSystemId;
    stream << loc.m_name;
    stream << loc.m_x;
    stream << loc.m_y;
    stream << loc.m_z;
    return stream;
}


QDataStream &operator>>(QDataStream &stream, EM::EveLocation &loc)
{
    stream >> loc.m_locationId;
    stream >> loc.m_typeId;
    stream >> loc.m_solarSystemId;
    stream >> loc.m_name;
    stream >> loc.m_x;
    stream >> loc.m_y;
    stream >> loc.m_z;
    return stream;
}


QDebug operator<<(QDebug &stream, const EM::EveLocation &loc)
{
    stream.nospace() << "[Location " << loc.m_name << "/" << loc.m_locationId << "]";
    return stream;
}
