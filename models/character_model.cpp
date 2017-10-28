#include <QLocale>
#include <QLoggingCategory>
#include <QDebug>

#include "character.h"
#include "character_model.h"
#include "db/db_sqlite.h"

Q_LOGGING_CATEGORY(logCharacterModel, "evemon.character_model")


EM::CharacterModel::CharacterModel(QObject *parent):
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
    // skills info
    m_roles.insert(Roles::TrainingSkill, "trainingSkill");
    m_roles.insert(Roles::TrainingSkillTimeLeft, "trainingSkillTimeLeft");
    m_roles.insert(Roles::TrainingSkillEndDateTime, "trainingSkillEndDateTime");
    m_roles.insert(Roles::QueueTimeLeft, "queueTimeLeft");

    // model init
    m_characterList.clear();
}


EM::CharacterModel::~CharacterModel()
{
    // delete all characters
    QMutexLocker locker(&m_mutex);
    for (Character *ch: m_characterList) {
        if (ch) delete ch;
    }
    m_characterList.clear();
}


QHash<int, QByteArray> EM::CharacterModel::roleNames() const
{
    return m_roles;
}


int EM::CharacterModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    QMutexLocker locker(&const_cast<EM::CharacterModel *>(this)->m_mutex);
    return m_characterList.size();
}


QVariant EM::CharacterModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid()) {
        return ret;
    }
    QMutexLocker locker(&const_cast<EM::CharacterModel *>(this)->m_mutex);
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
            //ret = QString::number(static_cast<double>(ch->iskAmount()), 'f', 2);
            QLocale loc;
            ret = loc.toCurrencyString(ch->iskAmount(), QStringLiteral("ISK"), 2);
        } break;
        // skills info
    case TrainingSkill: ret = QLatin1String("-"); break;
    case TrainingSkillTimeLeft: ret = QLatin1String("-"); break;
    case TrainingSkillEndDateTime: ret = QLatin1String("-"); break;
    case QueueTimeLeft: ret = QLatin1String("-"); break;
    }
    return ret;
}


void EM::CharacterModel::loadCharacters()
{
    QMutexLocker locker(&m_mutex);
    m_characterList.clear();
    beginResetModel();
    EM::DbSqlite::instance()->loadCharacters(m_characterList);
    endResetModel();
}


void EM::CharacterModel::addNewCharacter(EM::Character *character)
{
    m_mutex.lock();
    int firstRow = m_characterList.size();  // we will append to list
    beginInsertRows(QModelIndex(), firstRow, firstRow);
    m_characterList.append(character);
    EM::DbSqlite::instance()->saveCharacters(m_characterList);
    m_mutex.unlock();  // unlock before emitting any signals
    endInsertRows();
    emit newCharacterAdded();
}


QList<EM::Character *> EM::CharacterModel::getCharacters() const
{
    QList<EM::Character *> ret;
    EM::CharacterModel *non_const_this = const_cast<EM::CharacterModel *>(this);
    non_const_this->m_mutex.lock();
    ret = this->m_characterList; // const this!
    non_const_this->m_mutex.unlock();
    return ret;
}


// emit signal to model clients that some character has changed data
void EM::CharacterModel::markCharacterAsUpdated(EM::Character *character)
{
    if (!character) return;
    m_mutex.lock();
    int row = m_characterList.indexOf(character);
    m_mutex.unlock();  // unlock before emitting any signals
    if (row == -1) return; // not found
    // update in DB
    EM::DbSqlite::instance()->saveCharacter(character);
    // emit signal for model clients
    QModelIndex topLeft = index(row);
    emit dataChanged(topLeft, topLeft); // one item changed only ;)
}


EM::Character *EM::CharacterModel::findCharacterById(int char_id)
{
    EM::Character *ret = nullptr;
    m_mutex.lock();
    for (EM::Character *ch: m_characterList) {
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
