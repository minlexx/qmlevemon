#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QString>
#include <QDateTime>

class Notification
{
public:
    Notification() = default;
    Notification(const Notification &) = default;
    Notification(Notification &&) = default;
    Notification &operator=(const Notification &) = default;
    Notification &operator=(Notification &&) = default;
    bool operator==(const Notification &o) const;
    bool operator!=(const Notification &o) const;

public:
    quint64 notificationId = 0;
    QString notificationType;
    quint64 senderId = 0;
    QString senderType;
    QDateTime timestamp;
    QString text;
};

#endif // NOTIFICATION_H
