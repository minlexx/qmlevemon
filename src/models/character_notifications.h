#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QString>
#include <QDateTime>
#include <QObject>
#include "common_model_base.h"

QT_FORWARD_DECLARE_CLASS(QDataStream)

// forward declare data stream operators

namespace EM {
    class Notification;
    class CharacterNotifications;
}


QDataStream& operator<<(QDataStream &stream, const EM::Notification &ntf);
QDataStream& operator>>(QDataStream &stream, EM::Notification &ntf);


namespace EM {

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
    QString senderDisplayName;
};


class CharacterNotifications: public CommonModelBase<Notification>
{
    Q_OBJECT
    enum Role {
        Id = Qt::UserRole + 1, Type, SenderId, SenderType, Timestamp, Text,
        SenderDisplayName
    };
public:
    CharacterNotifications(QObject *parent = nullptr);
public:
    QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};


} // namespace EM


QDataStream& operator<<(QDataStream &stream, const EM::Notification &ntf);
QDataStream& operator>>(QDataStream &stream, EM::Notification &ntf);


#endif // NOTIFICATION_H
