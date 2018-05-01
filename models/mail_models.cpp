#include <QDataStream>
#include "mail_models.h"


namespace EM {


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
