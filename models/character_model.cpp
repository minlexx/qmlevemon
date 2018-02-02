#include <QLocale>
#include <QLoggingCategory>
#include <QDebug>

#include "character.h"
#include "character_model.h"
#include "db/db_sqlite.h"

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
    m_roles.insert(Roles::TrainingSkill, "trainingSkill");
    m_roles.insert(Roles::TrainingSkillTimeLeft, "trainingSkillTimeLeft");
    m_roles.insert(Roles::TrainingSkillEndDateTime, "trainingSkillEndDateTime");
    m_roles.insert(Roles::QueueTimeLeft, "queueTimeLeft");

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
    int row = index.row();
    if ((row < 0) || (row >= m_characterList.count())) {
        return ret;
    }
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
            QLocale loc;
            ret = loc.toCurrencyString(ch->iskAmount(), QStringLiteral("ISK"), 2);
        } break;
    case ISKAmountStr: ret = ch->iskAmountStr(); break;
        // skills info
    case TrainingSkill: ret = QLatin1String("-"); break;
    case TrainingSkillTimeLeft: ret = QLatin1String("-"); break;
    case TrainingSkillEndDateTime: ret = QLatin1String("-"); break;
    case QueueTimeLeft: ret = QLatin1String("-"); break;
    }
    return ret;
}


void CharacterModel::loadCharacters()
{
    QMutexLocker locker(&m_mutex);
    m_characterList.clear();
    beginResetModel();
    DbSqlite::instance()->loadCharacters(m_characterList);
    endResetModel();
}


void CharacterModel::addNewCharacter(Character *character)
{
    m_mutex.lock();
    int firstRow = m_characterList.size();  // we will append to list
    beginInsertRows(QModelIndex(), firstRow, firstRow);
    m_characterList.append(character);
    DbSqlite::instance()->saveCharacters(m_characterList);
    m_mutex.unlock();  // unlock before emitting any signals
    endInsertRows();
    Q_EMIT newCharacterAdded();
}


/**
 * @brief CharacterModel::getCharacters
 * @return an explicitly copied characters objects list
 */
QList<Character *> CharacterModel::getCharacters() const
{
    QList<Character *> ret;
    m_mutex.lock();
    ret.reserve(this->m_characterList.size());
    ret.append(this->m_characterList);
    m_mutex.unlock();
    return ret;
}


// emit signal to model clients that some character has changed data
void CharacterModel::markCharacterAsUpdated(Character *character)
{
    if (!character) return;
    m_mutex.lock();
    int row = m_characterList.indexOf(character);
    m_mutex.unlock();  // unlock before emitting any signals
    if (row == -1) return; // not found
    // update in DB
    DbSqlite::instance()->saveCharacter(character);
    // emit signal for model clients
    QModelIndex topLeft = index(row);
    Q_EMIT dataChanged(topLeft, topLeft); // one item changed only ;)
}


Character *CharacterModel::findCharacterById(quint64 char_id)
{
    Character *ret = nullptr;
    m_mutex.lock();
    for (Character *ch: m_characterList) {
        if (ch && ch->characterId() == char_id) {
            ret = ch;
            break;
        }
    }
    m_mutex.unlock();
    if (ret == nullptr) {
        qCWarning(logCharacterModel) << "model cannot find character by id:" << char_id;
    }
    return ret;
}

} // namespace
