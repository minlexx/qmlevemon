#include <QDataStream>
#include "mail_models.h"


namespace EM {


bool MailLabel::operator==(const MailLabel &other)
{
    return (id           == other.id)
        && (name         == other.name)
        && (color        == other.color)
        && (unread_count == other.unread_count);
}

bool MailLabel::operator!=(const MailLabel &other)
{
    return !(operator==(other));
}


bool MailRecipient::operator==(const MailRecipient &other)
{
    return (id   == other.id)
        && (type == other.type)
        && (name == other.name);
}

bool MailRecipient::operator!=(const MailRecipient &other)
{
    return !(operator==(other));
}

MailRecipient::Type MailRecipient::typeFromString(const QString &typeName)
{
    Type ret = Type::None;
    if (typeName == QLatin1String("character")) {
        ret = Character;
    } else if (typeName == QLatin1String("corporation")) {
        ret = Corporation;
    } else if (typeName == QLatin1String("alliance")) {
        ret = Alliance;
    } else if (typeName == QLatin1String("mailing_list")) {
        ret = MailingList;
    }
    return ret;
}


CharacterMails::CharacterMails(QObject *parent)
    : QAbstractListModel(parent)
{
}

CharacterMails::CharacterMails(const CharacterMails &other)
    : QAbstractListModel(other.parent())
{
    (*this) = other;
}

CharacterMails::CharacterMails(CharacterMails &&other)
    : QAbstractListModel(other.parent())
{
    (*this) = std::move(other);
}

CharacterMails &CharacterMails::operator=(const CharacterMails &other)
{
    if (this == &other) return (*this);
    beginResetModel();
    m_data = other.m_data;
    endResetModel();
    return (*this);
}

CharacterMails &CharacterMails::operator=(CharacterMails &&other)
{
    if (this == &other) return (*this);
    beginResetModel();
    m_data = std::move(other.m_data);
    endResetModel();
    return (*this);
}

bool CharacterMails::operator==(const CharacterMails &other) const
{
    return m_data == other.m_data;
}

bool CharacterMails::operator!=(const CharacterMails &other) const
{
    return !(operator==(other));
}

int CharacterMails::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.size();
}

QHash<int,QByteArray> CharacterMails::roleNames() const
{
    QHash<int,QByteArray> roles = {
        {Qt::DisplayRole, QByteArrayLiteral("display")}
    };
    return roles;
}

QVariant CharacterMails::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) {
        return ret;
    }
    int row = index.row();
    if ((row < 0) || (row >= m_data.size())) {
        return ret;
    }
    switch (role) {
    case Qt::DisplayRole:
        ret = QLatin1String("TBD");
        break;
    }
    return ret;
}

bool Mail::operator==(const Mail &other)
{
    return (id         == other.id)
        && (body       == other.body)
        && (subject    == other.subject)
        && (id_from    == other.id_from)
        && (is_read    == other.is_read)
        && (timestamp  == other.timestamp)
        && (labels     == other.labels)
        && (recipients == other.recipients);
}

bool Mail::operator!=(const Mail &other)
{
    return !(operator==(other));
}


}  // namespace EM


QDataStream &operator<<(QDataStream &stream, const EM::MailLabel &label)
{
    stream << label.id;
    stream << label.name;
    stream << label.color;
    stream << label.unread_count;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::MailLabel &label)
{
    stream >> label.id;
    stream >> label.name;
    stream >> label.color;
    stream >> label.unread_count;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const EM::MailRecipient &recipient)
{
    stream << recipient.id;
    stream << recipient.name;
    stream << static_cast<int>(recipient.type);
    return stream;
}


QDataStream &operator>>(QDataStream &stream, EM::MailRecipient &recipient)
{
    int i = 0;
    stream >> recipient.id;
    stream >> recipient.name;
    stream >> i;
    recipient.type = static_cast<EM::MailRecipient::Type>(i);
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const EM::Mail &mail)
{
    stream << mail.id;
    stream << mail.body;
    stream << mail.subject;
    stream << mail.id_from;
    stream << mail.is_read;
    stream << mail.timestamp;
    stream << mail.labels;
    stream << mail.recipients;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::Mail &mail)
{
    stream >> mail.id;
    stream >> mail.body;
    stream >> mail.subject;
    stream >> mail.id_from;
    stream >> mail.is_read;
    stream >> mail.timestamp;
    stream >> mail.labels;
    stream >> mail.recipients;
    return stream;
}
