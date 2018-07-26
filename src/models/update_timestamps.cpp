#include <utility>
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

UpdateTimestamps::UpdateTimestamps(UpdateTimestamps &&other)
{
    (*this) = std::move(other);
}


void UpdateTimestamps::resetTs(UTST kind)
{
    QDateTime& dt_ref = p_get_ts(kind);
    dt_ref = QDateTime(); // zero out, assign empty QDateTime
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
    case UTST::PUBLIC:         return dt_public;
    case UTST::SKILLS:         return dt_skills;
    case UTST::WALLET:         return dt_wallet;
    case UTST::WALLET_JOURNAL: return dt_wallet_journal;
    case UTST::LOCATION:       return dt_location;
    case UTST::CLONES:         return dt_clones;
    case UTST::FATIGUE:        return dt_fatigue;
    case UTST::ASSETS:         return dt_assets;
    case UTST::MAIL:           return dt_mail;
    case UTST::NOTIFICATIONS:  return dt_notifications;
    }
    return *new QDateTime(); // should never happen
}


qint64 UpdateTimestamps::p_get_cache_seconds(UTST kind)
{
    switch (kind) {
    case UTST::PUBLIC:         return 3600;
    case UTST::SKILLS:         return 3600;
    case UTST::WALLET:         return 120;
    case UTST::WALLET_JOURNAL: return 3600; // both wallet journal and transactions are cached for 1 hour.
    case UTST::LOCATION:       return 60;
    case UTST::CLONES:         return 3600;
    case UTST::FATIGUE:        return 300;
    case UTST::ASSETS:         return 3600;
    case UTST::MAIL:           return 120; // in ESI mails are cached for 30 seconds, mailing lists for 120 seconds
    case UTST::NOTIFICATIONS:  return 600;
    }
    return 0; // should never happen
}


UpdateTimestamps& UpdateTimestamps::operator=(const UpdateTimestamps& other)
{
    if (this == &other) return (*this);
    dt_public         = other.dt_public;
    dt_skills         = other.dt_skills;
    dt_wallet         = other.dt_wallet;
    dt_location       = other.dt_location;
    dt_clones         = other.dt_clones;
    dt_fatigue        = other.dt_fatigue;
    dt_assets         = other.dt_assets;
    dt_mail           = other.dt_mail;
    dt_notifications  = other.dt_notifications;
    dt_wallet_journal = other.dt_wallet_journal;
    return (*this);
}

UpdateTimestamps& UpdateTimestamps::operator=(UpdateTimestamps&& other)
{
    dt_public         = std::move(other.dt_public);
    dt_skills         = std::move(other.dt_skills);
    dt_wallet         = std::move(other.dt_wallet);
    dt_location       = std::move(other.dt_location);
    dt_clones         = std::move(other.dt_clones);
    dt_fatigue        = std::move(other.dt_fatigue);
    dt_assets         = std::move(other.dt_assets);
    dt_mail           = std::move(other.dt_mail);
    dt_notifications  = std::move(other.dt_notifications);
    dt_wallet_journal = std::move(other.dt_wallet_journal);
    return (*this);
}


} // namepsace


// serializing functions

static const int UTST_SAVEDATA_VERSION = 4;

QDataStream& operator<<(QDataStream& stream, const EM::UpdateTimestamps& ts)
{
    stream << UTST_SAVEDATA_VERSION;
    stream << ts.dt_public;
    stream << ts.dt_skills;
    stream << ts.dt_wallet;
    stream << ts.dt_location;
    stream << ts.dt_clones;
    stream << ts.dt_fatigue;
    stream << ts.dt_assets;
    stream << ts.dt_mail;
    stream << ts.dt_notifications;
    stream << ts.dt_wallet_journal;
    return stream;
}


QDataStream& operator>>(QDataStream& stream, EM::UpdateTimestamps& ts)
{
    int savedata_version = 0;
    stream >> savedata_version;
    //
    stream >> ts.dt_public;
    stream >> ts.dt_skills;
    stream >> ts.dt_wallet;
    stream >> ts.dt_location;
    stream >> ts.dt_clones;
    stream >> ts.dt_fatigue;
    stream >> ts.dt_assets;
    stream >> ts.dt_mail;
    if (savedata_version >= 3) {
        stream >> ts.dt_notifications;
    }
    if (savedata_version >= 4) {
        stream >> ts.dt_wallet_journal;
    }
    return stream;
}
