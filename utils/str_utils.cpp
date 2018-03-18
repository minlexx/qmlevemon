#include "str_utils.h"


namespace Utils {


QString numberOfSecondsToTimeLeftString(qint64 seconds)
{
    qint64 days = 0, hours = 0, minutes = 0;

    days = seconds / 86400;
    seconds -= days * 86400;
    hours = seconds / 3600;
    seconds -= hours * 3600;
    minutes = seconds / 60;
    seconds -= minutes * 60;

    QString ret_str;
    if (days > 0) {
        ret_str += QString::number(days) + QLatin1String("d ");
    }
    if (hours > 0) {
        ret_str += QString::number(hours) + QLatin1String("h ");
    }
    if (minutes > 0) {
        ret_str += QString::number(minutes) + QLatin1String("m ");
    }
    ret_str += QString::number(seconds) + QLatin1String("s");

    return ret_str;
}


} // namespace Utils

