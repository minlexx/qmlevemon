#include <QDateTime>
#include <QLocale>
#include <QLoggingCategory>
#include <QDebug>

#include "character.h"
#include "character_model.h"
#include "qmlevemon_app.h"
#include "db/db.h"
#include "utils/str_utils.h"


Q_LOGGING_CATEGORY(logCharacterModel, "evemon.character_model")


namespace EM {


CharacterModel::CharacterModel(QObject *parent):
    QAbstractListModel(parent),
    m_mutex(QMutex::Recursive)
{
    // init role names for QML
    // general info
    m_roles = {
        {Qt::DisplayRole,                 "displayName"},
        {Roles::CharacterId,              "characterId"},
        {Roles::CharacterName,            "characterName"},
        {Roles::CorporationId,            "corporationId"},
        {Roles::CorporationName,          "corporationName"},
        {Roles::CorporationTicker,        "corporationTicker"},
        {Roles::AllianceId,               "allianceId"},
        {Roles::AllianceName,             "allianceName"},
        {Roles::AllianceTicker,           "allianceTicker"},
        {Roles::Race,                     "race"},
        {Roles::Ancestry,                 "ancestry"},
        {Roles::Bloodline,                "bloodline"},
        {Roles::Gender,                   "gender"}, // 0 for male
        {Roles::Birthday,                 "birthday"}, // 0 for male
        {Roles::SecurityStatus,           "securityStatus"},
        {Roles::Bio,                      "bio"},
        // wallet info
        {Roles::ISK,                      "isk"},
        {Roles::ISKAmountStr,             "iskAmountStr"},
        // skills info
        {Roles::TotalSP,                  "totalSp"},
        {Roles::TotalSkills,              "totalSkills"},
        {Roles::TotalSkillsAtV,           "totalSkillsAt5"},
        {Roles::TrainingSkill,            "trainingSkill"},
        {Roles::TrainingSkillTimeLeft,    "trainingSkillTimeLeft"},
        {Roles::TrainingSkillEndDateTime, "trainingSkillEndDateTime"},
        {Roles::QueueTimeLeft,            "queueTimeLeft"},
        {Roles::QueueFinishDateTime,      "queueFinishDateTime"},
        {Roles::IsQueueEmpty,             "isQueueEmpty"},
        {Roles::IsAlphaClone,             "isAlphaClone"},
    };

    // model init
    m_characters.clear();

    // timer properties
    m_notificationSquashTimer.setSingleShot(false);
    m_notificationSquashTimer.setTimerType(Qt::CoarseTimer);
    m_notificationSquashTimer.setInterval(3000);
    QObject::connect(&m_notificationSquashTimer, &QTimer::timeout,
                     this, &CharacterModel::squashNotifications);
}


CharacterModel::~CharacterModel()
{
    // delete all characters
    QMutexLocker locker(&m_mutex);
    for (Character *ch: m_characters) {
        if (ch) delete ch;
    }
    m_characters.clear();
}


QHash<int, QByteArray> CharacterModel::roleNames() const
{
    return m_roles;
}


int CharacterModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    QMutexLocker locker(&m_mutex);
    return m_characters.size();
}


QVariant CharacterModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) {
        return ret;
    }

    QMutexLocker locker(&m_mutex);
    const int row = index.row();
    if ((row < 0) || (row >= m_characters.count())) {
        return ret;
    }

    const QDateTime curDt = QDateTime::currentDateTime();
    const Character * const ch = m_characters.at(row);

    // return an appropriate role for QML
    switch (role) {
    case Qt::DisplayRole: ret = ch->toString(); break;
        // general info
    case CharacterId: ret = ch->characterId(); break;
    case CharacterName: ret = ch->characterName(); break;
    case CorporationId: ret = ch->corporationId(); break;
    case CorporationName: ret = ch->corporationName(); break;
    case CorporationTicker: ret = ch->corporationTicker(); break;
    case AllianceId: ret = ch->allianceId(); break;
    case AllianceName: ret = ch->allianceName(); break;
    case AllianceTicker: ret = ch->allianceTicker(); break;
    case Race: ret = ch->raceName(); break;
    case Ancestry: ret = ch->ancestryName(); break;
    case Bloodline: ret = ch->bloodlineName(); break;
    case Gender: ret = (ch->gender() == 0 ? tr("Male"): tr("Female")); break;
    case Birthday: ret = ch->birthday().toString(Qt::TextDate); break;
    case SecurityStatus: ret = ch->securityStatus(); break;
    case Bio: ret = ch->bio(); break;
        // wallet info
    case ISK: ret = ch->iskAmount(); break;
    case ISKAmountStr: ret = ch->iskAmountStr(); break;
        // skills info
    case TotalSP: ret = ch->totalSp(); break;
    case TotalSkills: ret = ch->totalSkills(); break;
    case TotalSkillsAtV: ret = ch->totalSkills5(); break;
    case TrainingSkill: {
            ret = QLatin1String("-");
            if (!ch->isSkillQueueEmpty()) {
                const CharacterSkill *sk = ch->currentTrainingSkill();
                if (sk) {
                    // "Large Pulse Laser Specialiation V"
                    ret = QString(QLatin1String("%1 %2")).arg(sk->skillName()).arg(sk->trainingLevelRoman());
                }
            }
        } break;
    case TrainingSkillTimeLeft: {
            ret = QLatin1String("-");
            if (!ch->isSkillQueueEmpty()) {
                const CharacterSkill *sk = ch->currentTrainingSkill();
                if (sk) {
                    const QDateTime endDt = sk->trainFinishDate();
                    const qint64 seconds_left = curDt.secsTo(endDt);
                    ret = Utils::numberOfSecondsToTimeLeftString(seconds_left);
                }
            }
        } break;
    case TrainingSkillEndDateTime: {
            ret = QLatin1String("-");
            if (!ch->isSkillQueueEmpty()) {
                const CharacterSkill *sk = ch->currentTrainingSkill();
                if (sk) {
                    ret = sk->trainFinishDate();
                }
            }
        } break;
    case QueueTimeLeft: {
            ret = QLatin1String("-");
            if (!ch->isSkillQueueEmpty()) {
                const QDateTime finishDt = ch->skillQueue().queueFinishDate();
                const qint64 seconds_left = curDt.secsTo(finishDt);
                ret = Utils::numberOfSecondsToTimeLeftString(seconds_left);
            }
        } break;
    case QueueFinishDateTime: {
            ret = QLatin1String("-");
            if (!ch->isSkillQueueEmpty()) {
                ret = ch->skillQueue().queueFinishDate();
            }
        } break;
    case IsQueueEmpty: ret = ch->isSkillQueueEmpty(); break;
    case IsAlphaClone: ret = ch->isAlphaClone(); break;
    }
    return ret;
}


void CharacterModel::loadCharacters()
{
    {
        QMutexLocker locker(&m_mutex);
        m_characters.clear();
        beginResetModel();
        Db *db = globalAppDatabaseInstance();
        if (db) {
            db->loadCharacters(m_characters);
            // connect all characters signals
            for (Character *character: m_characters) {
                QObject::connect(character, &Character::skillTrainingCompleted,
                                 this, &CharacterModel::onSkillTrainingCompleted);
                QObject::connect(character, &Character::newMailReceived,
                                 this, &CharacterModel::onNewMailReceived);
                QObject::connect(character, &Character::newNotificationReceived,
                                 this, &CharacterModel::onNewNotificationReceived);
            }
        }
        // unlock mutex before emitting any signals
    }
    endResetModel();
}


void CharacterModel::addNewCharacter(Character *character)
{
    {
        QMutexLocker lock(&m_mutex);
        int firstRow = m_characters.size();  // we will append to list
        beginInsertRows(QModelIndex(), firstRow, firstRow);
        m_characters.push_back(character);
        QObject::connect(character, &Character::skillTrainingCompleted,
                         this, &CharacterModel::onSkillTrainingCompleted);
        QObject::connect(character, &Character::newMailReceived,
                         this, &CharacterModel::onNewMailReceived);
        Db *db = globalAppDatabaseInstance();
        if (db) {
            db->saveCharacters(m_characters);
        }
        // unlock mutex before emitting any signals
    }
    endInsertRows();
    Q_EMIT newCharacterAdded();
}


void CharacterModel::removeCharacter(quint64 char_id)
{
    // scope block to hold mutex locker
    {
        QMutexLocker lock(&m_mutex);

        int toRemoveRow = -1;
        for (int i = 0; i < m_characters.size(); i++) {
            Character *ch = m_characters.at(i);
            if (ch && ch->characterId() == char_id) {
                toRemoveRow = i;
                break;
            }
        }
        if (toRemoveRow == -1) {
            qCWarning(logCharacterModel) << "Model cannot find character to remove:" << char_id;
            return;
        }

        beginRemoveRows(QModelIndex(), toRemoveRow, toRemoveRow);
        Character *toRemoveCh = m_characters.takeAt(toRemoveRow);
        delete toRemoveCh;

        Db *db = globalAppDatabaseInstance();
        if (db) {
            db->deletePortrait(char_id);
            db->saveCharacters(m_characters);
        }
    }
    // unlock mutex here, before signals emission

    endRemoveRows();
    Q_EMIT characterRemoved(char_id);
}


/**
 * @brief CharacterModel::getCharacters
 * @return an explicitly copied characters objects list
 */
QList<Character *> CharacterModel::getCharacters() const
{
    QList<Character *> ret_list;
    QMutexLocker lock(&m_mutex);
    for (const Character *ch: m_characters) {
        ret_list.append(new Character(*ch)); // copy-construct into returning list
    }
    return ret_list;
}


/**
 * Tries to find an existing character to update with data from received.
 * Emits signal to model clients that some character has changed data.
 * Deletes its argument.
 **/
void CharacterModel::updateCharacter(const Character *updatedCharacter)
{
    int row = -1;
    {
        QMutexLocker lock(&m_mutex);

        // find index of character with this char_id
        Character *to_modify = nullptr;
        int cindex = 0; // count index
        for (Character *existingCharacter: m_characters) {
            if (existingCharacter->characterId() == updatedCharacter->characterId()) {
                to_modify = existingCharacter;
                // row = m_characterList.indexOf(existingCharacter); // whaaat
                row = cindex;
            }
            ++cindex;
        }

        if ((row == -1) || (!to_modify)) {
            // not found
            qCWarning(logCharacterModel) << "updateCharacter(): Character not found after updating "
                    "from network, is it already deleted?" << updatedCharacter->toString();
            delete updatedCharacter;
            return;
        }

        // actually update character
        (*to_modify) = (*updatedCharacter);  // call lots of operator=()
        to_modify->checkNewMails();

        // save in DB
        Db *db = globalAppDatabaseInstance();
        if (db) {
            db->saveCharacter(to_modify);
        }

        // unlock mutex before emitting any signals
    }

    delete updatedCharacter;

    // emit signal for model clients
    QModelIndex topLeft = index(row);
    Q_EMIT dataChanged(topLeft, topLeft); // one item changed only ;)
}

void CharacterModel::calcCharactersSkillQueue()
{
    // lock model during recalculation
    {
        QMutexLocker lock(&m_mutex);
        for (Character *ch : m_characters) {
            ch->calcSkillQueue();
        }

        // notify model clients of data change
        QModelIndex first = index(0);
        QModelIndex last  = index(m_characters.size() - 1);
        QVector<int> changedRoles{
            Roles::TotalSP,
            Roles::TrainingSkill,
            Roles::TrainingSkillTimeLeft,
            Roles::TrainingSkillEndDateTime,
            Roles::QueueTimeLeft,
            Roles::QueueFinishDateTime,
            Roles::IsQueueEmpty
        };
        Q_EMIT dataChanged(first, last, changedRoles);
    }
}

void CharacterModel::onSkillTrainingCompleted(Character *ch, const CharacterSkill &skill)
{
    QMutexLocker lock(&m_mutex);
    const QString msg = QString(tr("%1 lv %2")).arg(skill.skillName()).arg(skill.trainedLevel());
    if (m_collectedNotificationsSkills.contains(ch)) {
        QString existingString = m_collectedNotificationsSkills.value(ch);
        existingString.append(QLatin1String(", "));
        existingString.append(msg);
        m_collectedNotificationsSkills.insert(ch, existingString);
    } else {
        m_collectedNotificationsSkills.insert(ch, msg);
    }

    m_notificationSquashTimer.start();
    // ^^ wll call squashNotifications() in 3 seconds
}

void CharacterModel::onNewMailReceived(Character *ch, const QString &mailSubject)
{
    Q_UNUSED(mailSubject)
    QMutexLocker lock(&m_mutex);
    int newMailsCount = 0;
    if (m_collectedNotificationsMails.contains(ch)) {
        newMailsCount = m_collectedNotificationsMails.value(ch);
    }
    newMailsCount++;
    m_collectedNotificationsMails.insert(ch, newMailsCount);
    m_notificationSquashTimer.start();
    // ^^ wll call squashNotifications() in 3 seconds
}

void CharacterModel::onNewNotificationReceived(Character *ch)
{
    QMutexLocker lock(&m_mutex);
    int newNtfsCount = 0;
    if (m_collectedNotificationsNotifications.contains(ch)) {
        newNtfsCount = m_collectedNotificationsNotifications.value(ch);
    }
    newNtfsCount++;
    m_collectedNotificationsNotifications.insert(ch, newNtfsCount);
    m_notificationSquashTimer.start();
    // ^^ wll call squashNotifications() in 3 seconds
}

void CharacterModel::squashNotifications()
{
    QMutexLocker lock(&m_mutex);
    m_notificationSquashTimer.stop();

    // collect learned skills
    QString msg;
    for (Character *ch: m_collectedNotificationsSkills.keys()) {
        const QString valueSkills = m_collectedNotificationsSkills.value(ch);
        if (!msg.isEmpty()) {
            Q_EMIT skillCompletedNotification(msg);
            msg.clear();
            // msg.append(QLatin1String("\n"));
        }
        msg.append(ch->characterName());
        msg.append(tr(" learned "));
        msg.append(valueSkills);
    }
    m_collectedNotificationsSkills.clear();
    if (!msg.isEmpty()) {
        Q_EMIT skillCompletedNotification(msg);
    }

    // collect new mails
    msg.clear();
    for (Character *ch: m_collectedNotificationsMails.keys()) {
        int newMailsCount = m_collectedNotificationsMails.value(ch);
        if (!msg.isEmpty()) {
            Q_EMIT newMailsReceivedNotification(msg);
            msg.clear();
            // msg.append(QLatin1String("\n"));
        }
        msg.append(ch->characterName());
        msg.append(tr(" new mails: "));
        msg.append(QString::number(newMailsCount));
    }
    m_collectedNotificationsMails.clear();
    if (!msg.isEmpty()) {
        Q_EMIT newMailsReceivedNotification(msg);
    }

    // collect new notifications
    msg.clear();
    for (Character *ch : m_collectedNotificationsNotifications.keys()) {
        int newNtfsCount = m_collectedNotificationsNotifications.value(ch);
        if (!msg.isEmpty()) {
            Q_EMIT newNotificationsReceivedNotification(msg);
            msg.clear();
            // msg.append(QLatin1String("\n"));
        }
        msg.append(ch->characterName());
        msg.append(tr(" new notifications: "));
        msg.append(QString::number(newNtfsCount));
    }
    m_collectedNotificationsNotifications.clear();
    if (!msg.isEmpty()) {
        Q_EMIT newNotificationsReceivedNotification(msg);
    }
}


Character *CharacterModel::findCharacterById(quint64 char_id)
{
    Character *ret = nullptr;

    {
        QMutexLocker lock(&m_mutex);
        for (Character *ch: m_characters) {
            if (ch && ch->characterId() == char_id) {
                ret = ch;
                break;
            }
        }
    }

    if (ret == nullptr) {
        qCWarning(logCharacterModel) << "model cannot find character by id:" << char_id;
    }
    return ret;
}

} // namespace
