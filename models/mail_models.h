#ifndef MAIL_MODELS_H
#define MAIL_MODELS_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QDateTime>
#include <QByteArray>
#include <QHash>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QDebug>


QT_FORWARD_DECLARE_CLASS(QDataStream)
QT_FORWARD_DECLARE_CLASS(QJsonObject)


namespace EM {
class CharacterMailLabels;
class CharacterMails;
}


QDataStream &operator<<(QDataStream &stream, const EM::CharacterMailLabels &charMailLabels);
QDataStream &operator>>(QDataStream &stream, EM::CharacterMailLabels &charMailLabels);
QDataStream &operator<<(QDataStream &stream, const EM::CharacterMails &charMails);
QDataStream &operator>>(QDataStream &stream, EM::CharacterMails &charMails);


namespace EM {


class MailLabel {
public:
    MailLabel() {}
    MailLabel(const MailLabel &) = default;
    MailLabel(MailLabel &&) = default;
    MailLabel &operator=(const MailLabel &) = default;
    MailLabel &operator=(MailLabel &&) = default;
    bool operator==(const MailLabel &other) const;
    bool operator!=(const MailLabel &other) const;

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
    bool operator==(const MailRecipient &other) const;
    bool operator!=(const MailRecipient &other) const;

    static Type typeFromString(const QString &typeName);
    static QString typeToString(Type typ);

    quint64 id = 0;
    Type type = Type::None;
    QString name;
};


class Mail {
private:
    Q_GADGET
    Q_PROPERTY(quint64   id MEMBER id)
    Q_PROPERTY(QString   body MEMBER body)
    Q_PROPERTY(QString   subject MEMBER subject)
    Q_PROPERTY(QString   fromName READ fromName)
    Q_PROPERTY(bool      is_read MEMBER is_read)
    Q_PROPERTY(QDateTime timestamp MEMBER timestamp)
    Q_PROPERTY(QString   labelsString READ labelsString)
    Q_PROPERTY(QString   recipientsString READ recipientsString)

public:
    Mail() { }
    Mail(const Mail &) = default;
    Mail(Mail &&) = default;
    Mail &operator=(const Mail &) = default;
    Mail &operator=(Mail &&) = default;
    bool operator==(const Mail &other) const;
    bool operator!=(const Mail &other) const;

    QString fromName() const;
    QString labelsString() const;
    QString recipientsString() const;

    void resolveLabels(const QVector<MailLabel> &charLabels);

    static Mail fromJson(const QJsonObject &jobj);

public:
    quint64 id = 0;
    QString body;
    QString subject;
    MailRecipient from;
    bool is_read = false;
    QDateTime timestamp;
    QVector<quint64> labels;
    QVector<QString> labels_str;
    QVector<MailRecipient> recipients;
};


class CharacterMailLabels: public QAbstractListModel
{
    Q_OBJECT
private:
    enum Roles {
        Id = Qt::UserRole + 1, Name, Color, UnreadCount
    };

public:
    CharacterMailLabels(QObject *parent = nullptr);
    CharacterMailLabels(const CharacterMailLabels &other);
    CharacterMailLabels(CharacterMailLabels &&other);
    CharacterMailLabels &operator=(const CharacterMailLabels &other);
    CharacterMailLabels &operator=(CharacterMailLabels &&other);
    bool operator==(const CharacterMailLabels &other) const;
    bool operator!=(const CharacterMailLabels &other) const;

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public: // for internal use, m_data getter
    QVector<MailLabel> &internalData();
    const QVector<MailLabel> &internalData() const;

protected:
    QVector<MailLabel> m_data;

    friend QDataStream& (::operator<<)(QDataStream &stream, const EM::CharacterMailLabels &charMailLabels);
    friend QDataStream& (::operator>>)(QDataStream &stream, EM::CharacterMailLabels &charMailLabels);
};


class CharacterMails: public QAbstractListModel
{
    Q_OBJECT
private:
    enum Roles {
        Id = Qt::UserRole + 1, Body, Subject, FromId, FromName, IsRead, Timestamp, Labels, Recipients
    };

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

public: // for internal use, m_data getter
    QVector<Mail> &internalData();
    const QVector<Mail> &internalData() const;

    void setMailBody(quint64 mailId, const QString &body);
    bool findMailById(quint64 mailId, Mail &ret);

protected:
    QVector<Mail> m_data;

    friend QDataStream& (::operator<<)(QDataStream &stream, const EM::CharacterMails &charMails);
    friend QDataStream& (::operator>>)(QDataStream &stream, EM::CharacterMails &charMails);
};


class MailLabelFilteredMailsModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MailLabelFilteredMailsModel(QObject *parent = nullptr);

    void setFilterMailLabelId(quint64 labelId);
    quint64 filterMailLabelId() const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    quint64 m_filterMailLabelId = 0;
};


} // namespace

Q_DECLARE_METATYPE(EM::Mail)


QDataStream &operator<<(QDataStream &stream, const EM::MailLabel &label);
QDataStream &operator>>(QDataStream &stream, EM::MailLabel &label);

QDataStream &operator<<(QDataStream &stream, const EM::MailRecipient &recipient);
QDataStream &operator>>(QDataStream &stream, EM::MailRecipient &recipient);

QDataStream &operator<<(QDataStream &stream, const EM::Mail &mail);
QDataStream &operator>>(QDataStream &stream, EM::Mail &mail);

QDebug operator<<(QDebug &stream, const EM::MailLabel &label);
QDebug operator<<(QDebug &stream, const EM::MailRecipient &rcpt);
QDebug operator<<(QDebug &stream, const EM::Mail &mail);


#endif // MAIL_MODELS_H
