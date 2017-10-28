#include <QDataStream>
#include "update_timestamps.h"


namespace EM {


UpdateTimestamps::UpdateTimestamps()
{
    //
}


UpdateTimestamps::UpdateTimestamps(const UpdateTimestamps& other)
{
    (*this) = other;
}


void UpdateTimestamps::updateTs(UTST kind)
{
    QDateTime& dt_ref = p_get_ts(kind);
    // next update dt = current dt + cache interval
    dt_ref = QDateTime::currentDateTime().addSecs(p_get_cache_seconds(kind));
}


bool UpdateTimestamps::isUpdateNeeded(UTST kind)
{
    QDateTime& dt_ref = p_get_ts(kind);
    if (!dt_ref.isValid() || dt_ref.isNull()) return true;
    if (dt_ref <= QDateTime::currentDateTime()) return true;
    return false;
}


QDateTime& UpdateTimestamps::p_get_ts(UTST kind)
{
    switch (kind) {
    case UTST::PUBLIC: return dt_public; break;
    case UTST::SKILLS: return dt_skills; break;
    case UTST::WALLET: return dt_wallet; break;
    case UTST::LOCATION: return dt_location; break;
    case UTST::CLONES: return dt_clones; break;
    case UTST::ASSETS: return dt_assets; break;
    }
    return *new QDateTime(); // should never happen
}


qint64 UpdateTimestamps::p_get_cache_seconds(UTST kind)
{
    switch (kind) {
    case UTST::PUBLIC: return 3600; break;
    case UTST::SKILLS: return 3600; break;
    case UTST::WALLET: return 120; break;
    case UTST::LOCATION: return 60; break;
    case UTST::CLONES: return 3600; break;
    case UTST::ASSETS: return 3600; break;
    }
    return 0; // should never happen
}


UpdateTimestamps& UpdateTimestamps::operator=(const UpdateTimestamps& other)
{
    if (this == &other) return (*this);
    dt_public = other.dt_public;
    dt_skills = other.dt_skills;
    dt_wallet = other.dt_wallet;
    dt_location = other.dt_location;
    dt_clones = other.dt_clones;
    dt_assets = other.dt_assets;
    return (*this);
}


} // namepsace


// serializing functions
QDataStream& operator<<(QDataStream& stream, const EM::UpdateTimestamps& ts)
{
    stream << ts.dt_public;
    stream << ts.dt_skills;
    stream << ts.dt_wallet;
    stream << ts.dt_location;
    stream << ts.dt_clones;
    stream << ts.dt_assets;
    return stream;
}


QDataStream& operator>>(QDataStream& stream, EM::UpdateTimestamps& ts)
{
    stream >> ts.dt_public;
    stream >> ts.dt_skills;
    stream >> ts.dt_wallet;
    stream >> ts.dt_location;
    stream >> ts.dt_clones;
    stream >> ts.dt_assets;
    return stream;
}
