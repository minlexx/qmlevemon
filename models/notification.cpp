#include "notification.h"


bool Notification::operator==(const Notification &o) const
{
    if ((notificationId == o.notificationId)
            && (notificationType == o.notificationType)
            && (senderId == o.senderId)
            && (senderType == o.senderType)
            && (timestamp == o.timestamp)
            && (text == o.text)) {
        return true;
    }
    return false;
}

bool Notification::operator!=(const Notification &o) const
{
    return !((*this) == o);
}
