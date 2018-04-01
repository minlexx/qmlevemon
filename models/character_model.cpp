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
    m_roles.insert(Qt::DisplayRole, "displayName");
    m_roles.insert(Roles::CharacterId, "characterId");
    m_roles.insert(Roles::CharacterName, "characterName");
    m_roles.insert(Roles::CorporationId, "corporationId");
    m_roles.insert(Roles::CorporationName, "corporationName");
    m_roles.insert(Roles::CorporationTicker, "corporationTicker");
    m_roles.insert(Roles::AllianceId, "allianceId");
    m_roles.insert(Roles::AllianceName, "allianceName");
    m_roles.insert(Roles::AllianceTicker, "allianceTicker");
    m_roles.insert(Roles::Race, "race");
    m_roles.insert(Roles::Ancestry, "ancestry");
    m_roles.insert(Roles::Bloodline, "bloodline");
    m_roles.insert(Roles::Gender, "gender"); // 0 for male
    m_roles.insert(Roles::Birthday, "birthday"); // 0 for male
    m_roles.insert(Roles::SecurityStatus, "securityStatus");
    m_roles.insert(Roles::Bio, "bio");
    // wallet info
    m_roles.insert(Roles::ISK, "isk");
    m_roles.insert(Roles::ISKAmountStr, "iskAmountStr");
    // skills info
    m_roles.insert(Roles::TotalSP, "totalSp");
    m_roles.insert(Roles::TrainingSkill, "trainingSkill");
    m_roles.insert(Roles::TrainingSkillTimeLeft, "trainingSkillTimeLeft");
    m_roles.insert(Roles::TrainingSkillEndDateTime, "trainingSkillEndDateTime");
    m_roles.insert(Roles::QueueTimeLeft, "queueTimeLeft");
    m_roles.insert(Roles::QueueFinishDateTime, "queueFinishDateTime");
    m_roles.insert(Roles::IsQueueEmpty, "isQueueEmpty");

    // model init
    m_characterList.clear();
}


CharacterModel::~CharacterModel()
{
    // delete all characters
    QMutexLocker locker(&m_mutex);
    for (Character *ch: m_characterList) {
        if (ch) delete ch;
    }
    m_characterList.clear();
}


QHash<int, QByteArray> CharacterModel::roleNames() const
{
    return m_roles;
}


int CharacterModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    QMutexLocker locker(&m_mutex);
    return m_characterList.size();
}


QVariant CharacterModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) {
        return ret;
    }

    QMutexLocker locker(&m_mutex);
    const int row = index.row();
    if ((row < 0) || (row >= m_characterList.count())) {
        return ret;
    }

    const QDateTime curDt = QDateTime::currentDateTime();
    const Character * const ch = m_characterList.at(row);

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
    case ISK: {
            const QLocale loc;
            ret = loc.toCurrencyString(ch->iskAmount(), QStringLiteral("ISK"), 2);
        } break;
    case ISKAmountStr: ret = ch->iskAmountStr(); break;
        // skills info
    case TotalSP: ret = ch->totalSp(); break;
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
    }
    return ret;
}


void CharacterModel::loadCharacters()
{
    {
        QMutexLocker locker(&m_mutex);
        m_characterList.clear();
        beginResetModel();
        Db *db = globalAppInstance()->database();
        if (db) {
            db->loadCharacters(m_characterList);
        }
        // unlock mutex before emitting any signals
    }
    endResetModel();
}


void CharacterModel::addNewCharacter(Character *character)
{
    {
        QMutexLocker lock(&m_mutex);
        int firstRow = m_characterList.size();  // we will append to list
        beginInsertRows(QModelIndex(), firstRow, firstRow);
        m_characterList.append(character);
        Db *db = globalAppInstance()->database();
        if (db) {
            db->saveCharacters(m_characterList);
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
        for (int i = 0; i < m_characterList.size(); i++) {
            Character *ch = m_characterList.at(i);
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
        Character *toRemoveCh = m_characterList.takeAt(toRemoveRow);
        delete toRemoveCh;

        Db *db = globalAppInstance()->database();
        if (db) {
            db->deletePortrait(char_id);
            db->saveCharacters(m_characterList);
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
QList<Character> CharacterModel::getCharacters() const
{
    QList<Character> ret_list;
    QMutexLocker lock(&m_mutex);
    for (const Character *ch: m_characterList) {
        ret_list << Character(*ch); // copy-construct into returning list
    }
    return ret_list;
}


// emit signal to model clients that some character has changed data
void CharacterModel::updateCharacter(const Character &updatedCharacter)
{
    int row = -1;
    {
        QMutexLocker lock(&m_mutex);

        // find index of character with this char_id
        Character *to_modify = nullptr;
        int cindex = 0; // count index
        for (Character *existingCharacter: m_characterList) {
            if (existingCharacter->characterId() == updatedCharacter.characterId()) {
                to_modify = existingCharacter;
                // row = m_characterList.indexOf(existingCharacter); // whaaat
                row = cindex;
            }
            ++cindex;
        }

        if ((row == -1) || (!to_modify)) {
            // not found
            qCWarning(logCharacterModel) << "updateCharacter(): Character not found after updating "
                    "from network, is it already deleted?" << updatedCharacter.toString();
            return;
        }

        // actually update character
        (*to_modify) = updatedCharacter;  // call lots of operator=()

        // save in DB
        Db *db = globalAppInstance()->database();
        if (db) {
            db->saveCharacter(to_modify);
        }

        // unlock mutex before emitting any signals
    }
    // emit signal for model clients
    QModelIndex topLeft = index(row);
    Q_EMIT dataChanged(topLeft, topLeft); // one item changed only ;)
}


Character *CharacterModel::findCharacterById(quint64 char_id)
{
    Character *ret = nullptr;

    {
        QMutexLocker lock(&m_mutex);
        for (Character *ch: m_characterList) {
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
