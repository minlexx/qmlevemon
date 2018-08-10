#include <QDataStream>
#include "character_notifications.h"


namespace EM {


bool Notification::operator==(const Notification &o) const
{
    if (
            (notificationId == o.notificationId)
            && (notificationType == o.notificationType)
            && (senderId == o.senderId)
            && (senderType == o.senderType)
            && (timestamp == o.timestamp)
            && (text == o.text)
            && (senderDisplayName == o.senderDisplayName)
            && (is_read == o.is_read)
        )
    {
        return true;
    }
    return false;
}

bool Notification::operator!=(const Notification &o) const
{
    return !((*this) == o);
}


EM::CharacterNotifications::CharacterNotifications(QObject *parent)
    : CommonModelBase<Notification>(parent)
{
}

QHash<int, QByteArray> CharacterNotifications::roleNames() const
{
    static QHash<int,QByteArray> roles = {
        {Qt::DisplayRole,         QByteArrayLiteral("type")},
        {Role::Id,                QByteArrayLiteral("id")},
        {Role::Type,              QByteArrayLiteral("type")},
        {Role::SenderId,          QByteArrayLiteral("senderId")},
        {Role::SenderType,        QByteArrayLiteral("senderType")},
        {Role::Timestamp,         QByteArrayLiteral("timestamp")},
        {Role::Text,              QByteArrayLiteral("text")},
        {Role::SenderDisplayName, QByteArrayLiteral("senderDisplayName")},
        {Role::IsRead,            QByteArrayLiteral("isRead")},
    };
    return roles;
}

QVariant CharacterNotifications::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    const Notification *ntf = validateIndexAndGetData(index);
    if (!ntf) return ret;
    switch (role) {
    case Qt::DisplayRole:
    case Role::Type:
        ret = ntf->notificationType;
        break;
    case Role::Id:
        ret = ntf->notificationId;
        break;
    case Role::SenderId:
        ret = ntf->senderId;
        break;
    case Role::SenderType:
        ret = ntf->senderType;
        break;
    case Role::Timestamp:
        ret = ntf->timestamp;
        break;
    case Role::Text:
        ret = ntf->text;
        break;
    case Role::SenderDisplayName:
        ret = ntf->senderDisplayName;
        break;
    case Role::IsRead:
        ret = ntf->is_read;
        break;
    }
    return ret;
}


} // namespace EM


QDataStream &operator<<(QDataStream &stream, const EM::Notification &ntf)
{
    stream << ntf.notificationId;
    stream << ntf.notificationType;
    stream << ntf.senderId;
    stream << ntf.senderType;
    stream << ntf.timestamp;
    stream << ntf.text;
    stream << ntf.senderDisplayName;
    stream << ntf.is_read;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::Notification &ntf)
{
    stream >> ntf.notificationId;
    stream >> ntf.notificationType;
    stream >> ntf.senderId;
    stream >> ntf.senderType;
    stream >> ntf.timestamp;
    stream >> ntf.text;
    stream >> ntf.senderDisplayName;
    stream >> ntf.is_read;
    return stream;
}
