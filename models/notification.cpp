#include <QDataStream>
#include "notification.h"


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
    : QAbstractListModel(parent)
{
}

CharacterNotifications::CharacterNotifications(const CharacterNotifications &other)
    : QAbstractListModel(other.parent())
{
    (*this) = other;
}

CharacterNotifications::CharacterNotifications(CharacterNotifications &&other)
    : QAbstractListModel(other.parent())
{
    (*this) = std::move(other);
}

CharacterNotifications &CharacterNotifications::operator=(const CharacterNotifications &other)
{
    if (this == &other) return (*this);
    beginResetModel();
    m_data = other.m_data;
    endResetModel();
    return (*this);
}

CharacterNotifications &CharacterNotifications::operator=(CharacterNotifications &&other)
{
    if (this == &other) return (*this);
    beginResetModel();
    m_data = std::move(other.m_data);
    endResetModel();
    return (*this);
}

bool CharacterNotifications::operator==(const CharacterNotifications &other) const
{
    return m_data == other.m_data;
}

bool CharacterNotifications::operator!=(const CharacterNotifications &other) const
{
    return !((*this) == other);
}

int CharacterNotifications::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
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
        {Role::SenderDisplayName, QByteArrayLiteral("SenderDisplayName")},
    };
    return roles;
}

QVariant CharacterNotifications::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) {
        return ret;
    }
    int row = index.row();
    if ((row < 0) || (row >= m_data.size())) {
        return ret;
    }
    const Notification &ntf = m_data.at(row);
    switch (role) {
    case Qt::DisplayRole:
    case Role::Type:
        ret = ntf.notificationType;
        break;
    case Role::Id:
        ret = ntf.notificationId;
        break;
    case Role::SenderId:
        ret = ntf.senderId;
        break;
    case Role::SenderType:
        ret = ntf.senderType;
        break;
    case Role::Timestamp:
        ret = ntf.timestamp;
        break;
    case Role::Text:
        ret = ntf.text;
        break;
    case Role::SenderDisplayName:
        ret = ntf.senderDisplayName;
        break;
    }
    return ret;
}

QVector<Notification> &CharacterNotifications::internalData()
{
    return m_data;
}

const QVector<Notification> &CharacterNotifications::internalData() const
{
    return m_data;
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
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const EM::CharacterNotifications &charNotifications)
{
    stream << charNotifications.m_data;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::CharacterNotifications &charNotifications)
{
    stream >> charNotifications.m_data;
    return stream;
}

