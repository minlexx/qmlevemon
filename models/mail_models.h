#ifndef MAIL_MODELS_H
#define MAIL_MODELS_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QDateTime>


QT_FORWARD_DECLARE_CLASS(QDataStream)


namespace EM {


class MailLabel {
public:
    MailLabel() {}
    MailLabel(const MailLabel &) = default;
    MailLabel(MailLabel &&) = default;
    MailLabel &operator=(const MailLabel &) = default;
    MailLabel &operator=(MailLabel &&) = default;

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


} // namespace


QDataStream &operator<<(QDataStream &stream, const EM::MailLabel &label);
QDataStream &operator>>(QDataStream &stream, EM::MailLabel &label);

QDataStream &operator<<(QDataStream &stream, const EM::MailRecipient &recipient);
QDataStream &operator>>(QDataStream &stream, EM::MailRecipient &recipient);

QDataStream &operator<<(QDataStream &stream, const EM::Mail &mail);
QDataStream &operator>>(QDataStream &stream, EM::Mail &mail);


#endif // MAIL_MODELS_H
