#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <QObject>
#include <QVector>
#include <QHash>
#include <QAbstractListModel>

QT_FORWARD_DECLARE_CLASS(QDataStream)

// forward declare data stream operators

namespace EM {
    class Notification;
    class CharacterNotifications;
}


QDataStream& operator<<(QDataStream &stream, const EM::Notification &ntf);
QDataStream& operator>>(QDataStream &stream, EM::Notification &ntf);

QDataStream& operator<<(QDataStream &stream, const EM::CharacterNotifications &charMailLabels);
QDataStream& operator>>(QDataStream &stream, EM::CharacterNotifications &charMailLabels);


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


class CharacterNotifications: public QAbstractListModel
{
    Q_OBJECT
private:
    enum Role {
        Id = Qt::UserRole + 1, Type, SenderId, SenderType, Timestamp, Text,
        SenderDisplayName
    };

public:
    CharacterNotifications(QObject *parent = nullptr);
    CharacterNotifications(const CharacterNotifications &other);
    CharacterNotifications(CharacterNotifications &&other);
    CharacterNotifications &operator=(const CharacterNotifications &other);
    CharacterNotifications &operator=(CharacterNotifications &&other);
    bool operator==(const CharacterNotifications &other) const;
    bool operator!=(const CharacterNotifications &other) const;

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public: // for internal use, m_data getter
    QVector<Notification> &internalData();
    const QVector<Notification> &internalData() const;

protected:
    QVector<Notification> m_data;

    friend QDataStream& (::operator<<)(QDataStream &stream, const EM::CharacterNotifications &charNotifications);
    friend QDataStream& (::operator>>)(QDataStream &stream, EM::CharacterNotifications &charNotifications);
};


} // namespace EM


QDataStream& operator<<(QDataStream &stream, const EM::Notification &ntf);
QDataStream& operator>>(QDataStream &stream, EM::Notification &ntf);

QDataStream& operator<<(QDataStream &stream, const EM::CharacterNotifications &charNotifications);
QDataStream& operator>>(QDataStream &stream, EM::CharacterNotifications &charNotifications);


#endif // NOTIFICATION_H
