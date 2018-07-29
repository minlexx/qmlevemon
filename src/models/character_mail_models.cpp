#include <QDataStream>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "character_mail_models.h"


namespace EM {


bool MailLabel::operator==(const MailLabel &other) const
{
    return (id           == other.id)
        && (name         == other.name)
        && (color        == other.color)
        && (unread_count == other.unread_count);
}

bool MailLabel::operator!=(const MailLabel &other) const
{
    return !(operator==(other));
}


bool MailRecipient::operator==(const MailRecipient &other) const
{
    return (id   == other.id)
        && (type == other.type)
        && (name == other.name);
}

bool MailRecipient::operator!=(const MailRecipient &other) const
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

QString MailRecipient::typeToString(MailRecipient::Type typ)
{
    QString ret;
    if (typ == Character) {
        ret = QLatin1String("character");
    } else if (typ == Corporation) {
        ret = QLatin1String("corporation");
    } else if (typ == Alliance) {
        ret = QLatin1String("alliance");
    } else if (typ == MailingList) {
        ret = QLatin1String("mailing_list");
    }
    return ret;
}


bool Mail::operator==(const Mail &other) const
{
    return (id         == other.id)
        && (body       == other.body)
        && (subject    == other.subject)
        && (from       == other.from)
        && (is_read    == other.is_read)
        && (timestamp  == other.timestamp)
        && (labels     == other.labels)
        && (labels_str == other.labels_str)
        && (recipients == other.recipients);
}

bool Mail::operator!=(const Mail &other) const
{
    return !(operator==(other));
}

QString Mail::fromName() const { return from.name; }

quint64 Mail::fromId() const { return from.id; }

QString Mail::labelsString() const
{
    QString ret;
    for (const QString &lbl: labels_str) {
        if (!ret.isEmpty()) {
            ret.append(QLatin1String(", "));
        }
        ret.append(lbl);
    }
    return ret;
}

QString Mail::recipientsString() const
{
    QString ret;
    for (const MailRecipient &rcpt: recipients) {
        if (!ret.isEmpty()) {
            ret.append(QLatin1String(", "));
        }
        ret.append(rcpt.name);
    }
    return ret;
}

void Mail::resolveLabels(const QVector<MailLabel> &charLabels)
{
    labels_str.clear();
    // for each label_id find its name in passed in vector
    for (quint64 lbl_id: labels) {
        QString lbl_str = QString::number(lbl_id);
        for (const MailLabel &charLabel: charLabels) {
            if (charLabel.id == lbl_id) {
                lbl_str = charLabel.name;
                break;
            }
        }
        labels_str.push_back(lbl_str);
    }
}

Mail Mail::fromJson(const QJsonObject &jobj)
{
    Mail ret;
    ret.body = jobj.value(QLatin1String("body")).toString();
    ret.from.id = jobj.value(QLatin1String("from")).toVariant().toULongLong();
    ret.from.type = MailRecipient::None; // ID type unknown;
    // TODO: fix this when API will have this field explicitly (from_type or sender_type) ^^
    ret.subject = jobj.value(QLatin1String("subject")).toString();
    ret.timestamp = jobj.value(QLatin1String("timestamp")).toVariant().toDateTime();
    const QJsonArray jlabels = jobj.value(QLatin1String("labels")).toArray();
    const QJsonArray jrecipients = jobj.value(QLatin1String("recipients")).toArray();
    for (const QJsonValue &jval: jlabels) {
        ret.labels.append(static_cast<quint64>(jval.toInt()));
    }
    for (const QJsonValue &jval: jrecipients) {
        const QJsonObject jrecipient = jval.toObject();
        const quint64 recipient_id = static_cast<quint64>(jrecipient.value(QLatin1String("recipient_id")).toInt());
        const QString recipient_type = jrecipient.value(QLatin1String("recipient_type")).toString();
        MailRecipient rcpt;
        rcpt.id = recipient_id;
        rcpt.type = MailRecipient::typeFromString(recipient_type);
        ret.recipients.push_back(std::move(rcpt));
    }
    return ret;
}


CharacterMailLabels::CharacterMailLabels(QObject *parent)
    : CommonModelBase<MailLabel>(parent)
{
}

QHash<int,QByteArray> CharacterMailLabels::roleNames() const
{
    static QHash<int,QByteArray> roles = {
        {Qt::DisplayRole,    QByteArrayLiteral("display")},
        {Roles::Id,          QByteArrayLiteral("id")},
        {Roles::Name,        QByteArrayLiteral("name")},
        {Roles::Color,       QByteArrayLiteral("color")},
        {Roles::UnreadCount, QByteArrayLiteral("unreadCount")},
    };
    return roles;
}

QVariant CharacterMailLabels::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    const MailLabel *mailLabel = validateIndexAndGetData(index);
    if (!mailLabel) return ret;
    switch (role) {
    case Qt::DisplayRole:
    case Roles::Name:
        ret = mailLabel->name;
        break;
    case Roles::Id:
        ret = mailLabel->id;
        break;
    case Roles::Color:
        ret = mailLabel->color;
        break;
    case Roles::UnreadCount:
        ret = mailLabel->unread_count;
        break;
    }
    return ret;
}



CharacterMails::CharacterMails(QObject *parent)
    : CommonModelBase<Mail>(parent)
{
}

QHash<int,QByteArray> CharacterMails::roleNames() const
{
    QHash<int,QByteArray> roles = {
        {Qt::DisplayRole,   QByteArrayLiteral("display")},
        {Roles::Id,         QByteArrayLiteral("id")},
        {Roles::Body,       QByteArrayLiteral("body")},
        {Roles::Subject,    QByteArrayLiteral("subject")},
        {Roles::FromId,     QByteArrayLiteral("fromId")},
        {Roles::FromName,   QByteArrayLiteral("fromName")},
        {Roles::IsRead,     QByteArrayLiteral("isRead")},
        {Roles::Timestamp,  QByteArrayLiteral("timestamp")},
        {Roles::Labels,     QByteArrayLiteral("labels")},
        {Roles::Recipients, QByteArrayLiteral("recipients")},
    };
    return roles;
}

QVariant CharacterMails::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    const Mail *mail = validateIndexAndGetData(index);
    if (!mail) return ret;
    switch (role) {
    case Qt::DisplayRole:
    case Roles::Subject:
        ret = mail->subject;
        break;
    case Roles::Id:
        ret = mail->id;
        break;
    case Roles::Body:
        ret = mail->body;
        break;
    case Roles::FromId:
        ret = mail->from.id;
        break;
    case Roles::FromName:
        ret = mail->from.name;
        break;
    case Roles::IsRead:
        ret = mail->is_read;
        break;
    case Roles::Timestamp:
        ret = mail->timestamp;
        break;
    case Roles::Labels: {
            QStringList lblList;
            for (const QString &lblName: mail->labels_str) {
                lblList.append(lblName);
            }
            ret = lblList;
        } break;
    }
    return ret;
}

void CharacterMails::setMailBody(quint64 mailId, const QString &body)
{
    int iRow = 0;
    for (Mail &m: m_data) {
        if (m.id == mailId) {
            m.body = body;
            QModelIndex idx = index(iRow);
            Q_EMIT dataChanged(idx, idx);
        }
        iRow++;
    }
}

bool CharacterMails::findMailById(quint64 mailId, Mail &ret) const
{
    for (const Mail &m: m_data) {
        if (m.id == mailId) {
            ret = m;
            return true;
        }
    }
    return false;
}


MailLabelFilteredMailsModel::MailLabelFilteredMailsModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void MailLabelFilteredMailsModel::setFilterMailLabelId(quint64 labelId)
{
    m_filterMailLabelId = labelId;
    invalidateFilter();
}

quint64 MailLabelFilteredMailsModel::filterMailLabelId() const
{
    return m_filterMailLabelId;
}

bool MailLabelFilteredMailsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_filterMailLabelId == 0) {
        // no filter - return all
        return true;
    }
    const CharacterMails *parentModel = qobject_cast<const CharacterMails *>(sourceModel());
    if (Q_UNLIKELY(!parentModel)) {
        // somehow our source model is not CharacterMails instance
        return true; // blindly accept, should never happen
    }
    const QModelIndex sourceIndex = parentModel->index(sourceRow, 0, sourceParent);
    const QVector<Mail> &allMails = parentModel->internalData();
    const Mail &sourceMail = allMails.at(sourceIndex.row());
    if (sourceMail.labels.contains(m_filterMailLabelId)) {
        return true;
    }
    // separate condition check for mailing lists
    //   usually mails do not contain more than 1-2 recipients,
    //   so no worries about loop here
    for (const MailRecipient &recipient: sourceMail.recipients) {
        if (recipient.type == MailRecipient::MailingList) {
            if (recipient.id == m_filterMailLabelId) {
                return true;
            }
        }
    }
    return false;
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
    stream << mail.from;
    stream << mail.is_read;
    stream << mail.timestamp;
    stream << mail.labels;
    stream << mail.labels_str;
    stream << mail.recipients;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EM::Mail &mail)
{
    stream >> mail.id;
    stream >> mail.body;
    stream >> mail.subject;
    stream >> mail.from;
    stream >> mail.is_read;
    stream >> mail.timestamp;
    stream >> mail.labels;
    stream >> mail.labels_str;
    stream >> mail.recipients;
    return stream;
}


QDebug operator<<(QDebug &stream, const EM::MailLabel &label)
{
    stream.nospace() << "[Label " << label.id << "/" << label.name << "]";
    return stream;
}

QDebug operator<<(QDebug &stream, const EM::MailRecipient &rcpt)
{
    stream.nospace() << "[Recepient " << rcpt.id << "/" << rcpt.name
                     << "/" << EM::MailRecipient::typeToString(rcpt.type) << "]";
    return stream;
}

QDebug operator<<(QDebug &stream, const EM::Mail &mail)
{
    stream.nospace() << "[Mail " << mail.id << "; Subject: " << mail.subject
                     << "; From: " << mail.from << "; Date: " << mail.timestamp
                     << "; Labels:" << mail.labels << mail.labels_str
                     << "; Recipients: " << mail.recipients << "]";
    return stream;
}
