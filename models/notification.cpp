#include <QDataStream>
#include "notification.h"


namespace EM {


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
        {Qt::DisplayRole,    QByteArrayLiteral("type")},
        {Roles::Id,          QByteArrayLiteral("id")},
        {Roles::Type,        QByteArrayLiteral("type")},
        {Roles::SenderId,    QByteArrayLiteral("senderId")},
        {Roles::SenderType,  QByteArrayLiteral("senderType")},
        {Roles::Timestamp,   QByteArrayLiteral("timestamp")},
        {Roles::Text,        QByteArrayLiteral("text")},
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
    case Roles::Type:
        ret = ntf.notificationType;
        break;
    case Roles::Id:
        ret = ntf.notificationId;
        break;
    case Roles::SenderId:
        ret = ntf.senderId;
        break;
    case Roles::SenderType:
        ret = ntf.senderType;
        break;
    case Roles::Timestamp:
        ret = ntf.timestamp;
        break;
    case Roles::Text:
        ret = ntf.text;
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

