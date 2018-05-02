#ifndef MAIL_MODELS_H
#define MAIL_MODELS_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QDateTime>
#include <QByteArray>
#include <QHash>
#include <QAbstractListModel>


QT_FORWARD_DECLARE_CLASS(QDataStream)


namespace EM {


class MailLabel {
public:
    MailLabel() {}
    MailLabel(const MailLabel &) = default;
    MailLabel(MailLabel &&) = default;
    MailLabel &operator=(const MailLabel &) = default;
    MailLabel &operator=(MailLabel &&) = default;
    bool operator==(const MailLabel &other);
    bool operator!=(const MailLabel &other);

    quint64 id = 0;
    QString name;
    QColor color;
    int unread_count = 0;
};


class MailRecipient {
public:
    enum Type {
        None,
        Character,
        Corporation,
        Alliance,
        MailingList
    };

    MailRecipient() { }
    MailRecipient(const MailRecipient &) = default;
    MailRecipient(MailRecipient &&) = default;
    MailRecipient& operator=(const MailRecipient &) = default;
    MailRecipient& operator=(MailRecipient &&) = default;
    bool operator==(const MailRecipient &other);
    bool operator!=(const MailRecipient &other);

    static Type typeFromString(const QString &typeName);

    quint64 id = 0;
    Type type = Type::None;
    QString name;
};


class Mail {
public:
    Mail() { }
    Mail(const Mail &) = default;
    Mail(Mail &&) = default;
    Mail &operator=(const Mail &) = default;
    Mail &operator=(Mail &&) = default;
    bool operator==(const Mail &other);
    bool operator!=(const Mail &other);

public:
    quint64 id = 0;
    QString body;
    QString subject;
    quint64 id_from = 0;
    bool is_read = false;
    QDateTime timestamp;
    QVector<quint64> labels;
    QVector<MailRecipient> recipients;
};


class CharacterMails: public QAbstractListModel
{
public:
    CharacterMails(QObject *parent = nullptr);
    CharacterMails(const CharacterMails &other);
    CharacterMails(CharacterMails &&other);
    CharacterMails &operator=(const CharacterMails &other);
    CharacterMails &operator=(CharacterMails &&other);
    bool operator==(const CharacterMails &other) const;
    bool operator!=(const CharacterMails &other) const;

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    QVector<Mail> m_data;
};


} // namespace


QDataStream &operator<<(QDataStream &stream, const EM::MailLabel &label);
QDataStream &operator>>(QDataStream &stream, EM::MailLabel &label);

QDataStream &operator<<(QDataStream &stream, const EM::MailRecipient &recipient);
QDataStream &operator>>(QDataStream &stream, EM::MailRecipient &recipient);

QDataStream &operator<<(QDataStream &stream, const EM::Mail &mail);
QDataStream &operator>>(QDataStream &stream, EM::Mail &mail);


#endif // MAIL_MODELS_H
