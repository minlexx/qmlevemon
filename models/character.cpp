#include <utility>
#include <algorithm>
#include <limits.h>

#include <QDataStream>
#include <QLocale>
#include <QLoggingCategory>
#include <QDebug>

#include "character.h"


Q_LOGGING_CATEGORY(logCharacter, "evemon.character")

namespace EM {

Character::Character(QObject *parent)
    : QObject(parent)
{
}


Character::Character(const Character& other)
    : QObject(other.parent())
{
    (*this) = other;
}

Character::Character(Character &&other)
    : QObject(other.parent())
{
    (*this) = std::move(other);
}


Character& Character::operator=(const Character& other)
{
    if (this == &other) return (*this);
    // general info
    setCharacterId(other.m_characterId);
    setCharacterName(other.m_characterName);
    setCorporationId(other.m_corporationId);
    setCorporationName(other.m_corporationName);
    setCorporationTicker(other.m_corporationTicker);
    setAllianceId(other.m_allianceId);
    setAllianceName(other.m_allianceName);
    setAllianceTicker(other.m_allianceTicker);
    // general - origins
    setRaceId(other.m_raceId);
    setRaceName(other.m_raceName);
    setAncestryId(other.m_ancestryId);
    setAncestryName(other.m_ancestryName);
    setBloodlineId(other.m_bloodlineId);
    setBloodlineName(other.m_bloodlineName);
    setGender(other.m_gender);
    setBirthday(other.m_birthday_dt);
    setSecurityStatus(other.m_securityStatus);
    setBio(other.m_bio);
    // wallet info
    setIskAmount(other.m_isk);
    // location info
    setCurrentSolarSystemId(other.m_currentSolarSystemId);
    setCurrentSolarSystemName(other.m_currentSolarSystemName);
    setCurrentSolarSystemSecurity(other.m_currentSolarSystemSecurity);
    setCurrentConstellationId(other.m_currentConstellationId);
    setCurrentConstellationName(other.m_currentConstellationName);
    setCurrentRegionId(other.m_currentRegionId);
    setCurrentRegionName(other.m_currentRegionName);
    setCurrentStationId(other.m_currentStationId);
    setCurrentStructureId(other.m_currentStructureId);
    setCurrentStructureName(other.m_currentStructureName);
    setCurrentShipTypeId(other.m_currentShipTypeId);
    setCurrentShipTypeName(other.m_currentShipTypeName);
    setCurrentShipFriendlyName(other.m_currentShipFriendlyName);
    // skills and related
    setAttributeCharisma(other.m_attributeCharisma);
    setAttributeIntelligence(other.m_attributeIntelligence);
    setAttributeMemory(other.m_attributeMemory);
    setAttributePerception(other.m_attributePerception);
    setAttributeWillpower(other.m_attributeWillpower);
    setNumBonusRemaps(other.m_numBonusRemaps);
    setLastRemapDate(other.m_lastRemapDate);
    setRemapCooldownDate(other.m_remapCooldownDate);
    setTotalSp(other.m_totalSp);
    setIsAlphaClone(other.m_isAlphaClone);
    setSkills(other.m_skills);
    setSkillQueue(other.m_skillQueue);
    // auth info
    m_tokens = other.m_tokens;
    // last update date-times
    m_update_timestamps = other.m_update_timestamps;
    //
    return (*this);
}


Character& Character::operator=(Character&& other)
{
    // general info
    setCharacterId(std::move(other.m_characterId));
    setCharacterName(std::move(other.m_characterName));
    setCorporationId(std::move(other.m_corporationId));
    setCorporationName(std::move(other.m_corporationName));
    setCorporationTicker(std::move(other.m_corporationTicker));
    setAllianceId(std::move(other.m_allianceId));
    setAllianceName(std::move(other.m_allianceName));
    setAllianceTicker(std::move(other.m_allianceTicker));
    // general - origins
    setRaceId(std::move(other.m_raceId));
    setRaceName(std::move(other.m_raceName));
    setAncestryId(std::move(other.m_ancestryId));
    setAncestryName(std::move(other.m_ancestryName));
    setBloodlineId(std::move(other.m_bloodlineId));
    setBloodlineName(std::move(other.m_bloodlineName));
    setGender(std::move(other.m_gender));
    setBirthday(std::move(other.m_birthday_dt));
    setSecurityStatus(std::move(other.m_securityStatus));
    setBio(std::move(other.m_bio));
    // wallet info
    setIskAmount(std::move(other.m_isk));
    // location info
    setCurrentSolarSystemId(std::move(other.m_currentSolarSystemId));
    setCurrentSolarSystemName(std::move(other.m_currentSolarSystemName));
    setCurrentSolarSystemSecurity(std::move(other.m_currentSolarSystemSecurity));
    setCurrentConstellationId(std::move(other.m_currentConstellationId));
    setCurrentConstellationName(std::move(other.m_currentConstellationName));
    setCurrentRegionId(std::move(other.m_currentRegionId));
    setCurrentRegionName(std::move(other.m_currentRegionName));
    setCurrentStationId(std::move(other.m_currentStationId));
    setCurrentStructureId(std::move(other.m_currentStructureId));
    setCurrentStructureName(std::move(other.m_currentStructureName));
    setCurrentShipTypeId(std::move(other.m_currentShipTypeId));
    setCurrentShipTypeName(std::move(other.m_currentShipTypeName));
    setCurrentShipFriendlyName(std::move(other.m_currentShipFriendlyName));
    // skills and related
    setAttributeCharisma(std::move(other.m_attributeCharisma));
    setAttributeIntelligence(std::move(other.m_attributeIntelligence));
    setAttributeMemory(std::move(other.m_attributeMemory));
    setAttributePerception(std::move(other.m_attributePerception));
    setAttributeWillpower(std::move(other.m_attributeWillpower));
    setNumBonusRemaps(std::move(other.m_numBonusRemaps));
    setLastRemapDate(std::move(other.m_lastRemapDate));
    setRemapCooldownDate(std::move(other.m_remapCooldownDate));
    setTotalSp(std::move(other.m_totalSp));
    setIsAlphaClone(std::move(other.m_isAlphaClone));
    setSkills(std::move(other.m_skills));
    setSkillQueue(std::move(other.m_skillQueue));
    // auth info
    m_tokens = std::move(other.m_tokens);
    // last update date-times
    m_update_timestamps = std::move(other.m_update_timestamps);
    //
    return (*this);
}


bool Character::operator==(const Character& other)
{
    if (this == &other) return true;
    return (this->m_characterId == other.m_characterId);
}


QString Character::toString() const
{
    QString ret(QLatin1String("<Character "));
    ret.append(m_characterName);
    ret.append(QLatin1String("; id="));
    ret.append(QString::number(m_characterId));
    ret.append(QLatin1String(">"));
    return ret;
}


quint64 Character::characterId() const { return m_characterId; }

void Character::setCharacterId(quint64 char_id) {
    if (char_id == m_characterId) return;
    m_characterId = char_id;
    Q_EMIT characterIdChanged();
}

QString Character::characterName() const { return m_characterName; }

void Character::setCharacterName(const QString& name) {
    if (name == m_characterName) return;
    m_characterName = name;
    Q_EMIT characterNameChanged();
}

quint64 Character::corporationId() const { return m_corporationId; }

void Character::setCorporationId(quint64 corp_id) {
    if (corp_id == m_corporationId) return;
    m_corporationId = corp_id;
    Q_EMIT corporationIdChanged();
}

QString Character::corporationName() const { return m_corporationName; }

void Character::setCorporationName(const QString& corp_name) {
    if (corp_name == m_corporationName) return;
    m_corporationName = corp_name;
    Q_EMIT corporationNameChanged();
}

QString Character::corporationTicker() const { return m_corporationTicker; }

void Character::setCorporationTicker(const QString& ticker) {
    if (ticker == m_corporationTicker) return;
    m_corporationTicker = ticker;
    Q_EMIT corporationTickerChanged();
}

quint64 Character::allianceId() const { return m_allianceId; }

void Character::setAllianceId(quint64 ally_id) {
    if (ally_id == m_allianceId) return;
    m_allianceId = ally_id;
    Q_EMIT allianceIdChanged();
}

QString Character::allianceName() const { return m_allianceName; }

void Character::setAllianceName(const QString& ally_name) {
    if (m_allianceName == ally_name) return;
    m_allianceName = ally_name;
    Q_EMIT allianceNameChanged();
}

QString Character::allianceTicker() const { return m_allianceTicker; }

void Character::setAllianceTicker(const QString& ticker) {
    if (ticker == m_allianceTicker) return;
    m_allianceTicker = ticker;
    Q_EMIT allianceTickerChanged();
}

quint64 Character::raceId() const { return m_raceId; }
void Character::setRaceId(quint64 race_id) { m_raceId = race_id; }

QString Character::raceName() const { return m_raceName; }

void Character::setRaceName(const QString& race_name) {
    if (race_name == m_raceName) return;
    m_raceName = race_name;
    Q_EMIT raceNameChanged();
}

quint64 Character::ancestryId() const { return m_ancestryId; }
void Character::setAncestryId(quint64 id) { m_ancestryId = id; }

QString Character::ancestryName() const { return m_ancestryName; }

void Character::setAncestryName(const QString& name) {
    if (name == m_ancestryName) return;
    m_ancestryName = name;
    Q_EMIT ancestryNameChanged();
}

quint64 Character::bloodlineId() const { return m_bloodlineId; }
void Character::setBloodlineId(quint64 id) { m_bloodlineId = id; }

QString Character::bloodlineName() const { return m_bloodlineName; }

void Character::setBloodlineName(const QString& name) {
    if (name == m_bloodlineName) return;
    m_bloodlineName = name;
    Q_EMIT bloodlineNameChanged();
}

int Character::gender() const { return m_gender; }

void Character::setGender(int g) {
    if (g == m_gender) return;
    m_gender = g;
    Q_EMIT genderChanged();
}

QDateTime Character::birthday() const { return m_birthday_dt; }

void Character::setBirthday(const QDateTime& dt) {
    if (dt == m_birthday_dt) return;
    m_birthday_dt = dt;
    Q_EMIT birthdayChanged();
}

float Character::securityStatus() const { return m_securityStatus; }

QString Character::securityStatusStr() const {
    return QString::number(static_cast<double>(m_securityStatus), 'f', 1);
}

void Character::setSecurityStatus(float ss) {
    if (qFuzzyCompare(ss, m_securityStatus)) return;
    m_securityStatus = ss;
    Q_EMIT securityStatusChanged();
}

QString Character::bio() const { return m_bio; }

void Character::setBio(const QString& s) {
    if (m_bio == s) return;
    m_bio = s;
    Q_EMIT bioChanged();
}

// wallet info
float Character::iskAmount() const { return m_isk; }

QString Character::iskAmountStr() const{
    return QLocale::system().toCurrencyString(m_isk, QLatin1String(" ISK "), 2);
}

void Character::setIskAmount(float isk) {
    if (qFuzzyCompare(isk, m_isk)) return;
    m_isk = isk;
    Q_EMIT iskAmountChanged();
}

// location info
quint64 Character::currentSolarSystemId() const { return m_currentSolarSystemId; }

void Character::setCurrentSolarSystemId(quint64 id) {
    if (m_currentSolarSystemId == id) return;
    m_currentSolarSystemId = id;
    Q_EMIT currentSolarSystemChanged();
}

QString Character::currentSolarSystemName() const { return m_currentSolarSystemName; }

void Character::setCurrentSolarSystemName(const QString& name) {
    if (m_currentSolarSystemName == name) return;
    m_currentSolarSystemName = name;
    Q_EMIT currentSolarSystemChanged();
}

float Character::currentSolarSystemSecurity() const { return m_currentSolarSystemSecurity; }

QString Character::currentSolarSystemSecurityStr() const {
    return QString::number(static_cast<double>(m_currentSolarSystemSecurity), 'f', 2);
}

void Character::setCurrentSolarSystemSecurity(float ss) {
    if (qFuzzyCompare(m_currentSolarSystemSecurity, ss)) return;
    m_currentSolarSystemSecurity = ss;
    Q_EMIT currentSolarSystemSecurityChanged();
}

quint64 Character::currentConstellationId() const { return m_currentConstellationId; }

void Character::setCurrentConstellationId(quint64 id) {
    if (m_currentConstellationId == id) return;
    m_currentConstellationId = id;
    Q_EMIT currentConstellationChanged();
}

QString Character::currentConstellationName() const { return m_currentConstellationName; }

void Character::setCurrentConstellationName(const QString& name) {
    if (m_currentConstellationName == name) return;
    m_currentConstellationName = name;
    Q_EMIT currentConstellationChanged();
}

quint64 Character::currentRegionId() const { return m_currentRegionId; }

void Character::setCurrentRegionId(quint64 id) {
    if (m_currentRegionId == id) return;
    m_currentRegionId = id;
    Q_EMIT currentRegionChanged();
}

QString Character::currentRegionName() const { return m_currentRegionName; }

void Character::setCurrentRegionName(const QString& name) {
    if (m_currentRegionName == name) return;
    m_currentRegionName = name;
    Q_EMIT currentRegionChanged();
}

quint64 Character::currentStationId() const { return m_currentStationId; }

void Character::setCurrentStationId(quint64 id) {
    if (m_currentStationId == id) return;
    m_currentStationId = id;
    Q_EMIT isDockedChanged();
}

quint64 Character::currentStructureId() const { return m_currentStructureId; }

void Character::setCurrentStructureId(quint64 id) {
    if (m_currentStructureId == id) return;
    m_currentStructureId = id;
    Q_EMIT isDockedChanged();
}

QString Character::currentStructureName() const { return m_currentStructureName; }

void Character::setCurrentStructureName(const QString& name) {
    if (m_currentStructureName == name) return;
    m_currentStructureName = name;
    Q_EMIT isDockedChanged();
}

bool Character::isDocked() const { return ((m_currentStationId > 0) || (m_currentStructureId > 0)); }

quint64 Character::currentShipTypeId() const { return m_currentShipTypeId; }

void Character::setCurrentShipTypeId(quint64 id) {
    if (m_currentShipTypeId == id) return;
    m_currentShipTypeId = id;
    Q_EMIT currentShipChanged();
}

QString Character::currentShipTypeName() const { return m_currentShipTypeName; }

void Character::setCurrentShipTypeName(const QString& name) {
    if (m_currentShipTypeName == name) return;
    m_currentShipTypeName = name;
    Q_EMIT currentShipChanged();
}

QString Character::currentShipFriendlyName() const { return m_currentShipFriendlyName; }

void Character::setCurrentShipFriendlyName(const QString& name) {
    if (m_currentShipFriendlyName == name) return;
    m_currentShipFriendlyName = name;
    Q_EMIT currentShipChanged();
}

// skills and related
int Character::attributeCharisma() const { return m_attributeCharisma; }
int Character::attributeIntelligence() const { return m_attributeIntelligence; }
int Character::attributeMemory() const { return m_attributeMemory; }
int Character::attributePerception() const { return m_attributePerception; }
int Character::attributeWillpower() const { return m_attributeWillpower; }
int Character::numBonusRemaps() const { return m_numBonusRemaps; }
QDateTime Character::lastRemapDate() const { return m_lastRemapDate; }
QDateTime Character::remapCooldownDate() const { return m_remapCooldownDate; }
quint64 Character::totalSp() const { return m_totalSp; }
bool Character::isAlphaClone() const { return m_isAlphaClone; }
QVector<EM::CharacterSkill> Character::skills() const { return m_skills; }

QObject *Character::skillGroupsModel()
{
    return static_cast<QObject *>(&m_skillGroupsModel);
}

CharacterSkillQueue &Character::skillQueue() { return m_skillQueue; }
const CharacterSkillQueue &Character::skillQueue() const { return m_skillQueue; }

const QObject *Character::currentTrainingSkillObj() const
{
    const CharacterSkill *ret = currentTrainingSkill();
    if (!ret) {
        return nullptr;
    }
    return static_cast<const QObject *>(ret);
}

CharacterSkill *Character::currentTrainingSkill()
{
    const quint64 currentTrainingSkillId = m_skillQueue.currentTrainingSkillId();
    if (currentTrainingSkillId == 0) {
        // queue is empty
        return nullptr;
    }
    for (CharacterSkill &skill : m_skills) {
        if (skill.skillId() == currentTrainingSkillId) {
            return &skill;
        }
    }
    return nullptr;
}

const CharacterSkill *Character::currentTrainingSkill() const
{
    const quint64 currentTrainingSkillId = m_skillQueue.currentTrainingSkillId();
    if (currentTrainingSkillId == 0) {
        // queue is empty
        return nullptr;
    }
    for (const CharacterSkill &skill : m_skills) {
        if (skill.skillId() == currentTrainingSkillId) {
            return &skill;
        }
    }
    return nullptr;
}

bool Character::isSkillQueueEmpty() const
{
    return (m_skillQueue.currentTrainingSkillId() == 0);
}

void Character::setAttributeCharisma(int a) {
    if (m_attributeCharisma != a) {
        m_attributeCharisma = a;
        Q_EMIT attributeCharismaChanged();
    }
}

void Character::setAttributeIntelligence(int a) {
    if (m_attributeIntelligence != a) {
        m_attributeIntelligence = a;
        Q_EMIT attributeIntelligenceChanged();
    }
}

void Character::setAttributeMemory(int a) {
    if (m_attributeMemory != a) {
        m_attributeMemory = a;
        Q_EMIT attributeMemoryChanged();
    }
}

void Character::setAttributePerception(int a) {
    if (m_attributePerception != a) {
        m_attributePerception = a;
        Q_EMIT attributePerceptionChanged();
    }
}

void Character::setAttributeWillpower(int a) {
    if (m_attributeWillpower != a) {
        m_attributeWillpower = a;
        Q_EMIT attributeWillpowerChanged();
    }
}

void Character::setNumBonusRemaps(int n) {
    if (m_numBonusRemaps != n) {
        m_numBonusRemaps = n;
        Q_EMIT numBonusRemapsChanged();
    }
}

void Character::setLastRemapDate(const QDateTime& dt) {
    if (m_lastRemapDate != dt) {
        m_lastRemapDate = dt;
        Q_EMIT lastRemapDateChanged();
    }
}

void Character::setRemapCooldownDate(const QDateTime& dt) {
    if (m_remapCooldownDate != dt) {
        m_remapCooldownDate = dt;
        Q_EMIT remapCooldownDateChanged();
    }
}

void Character::setTotalSp(quint64 sp) {
    if (sp != m_totalSp) {
        m_totalSp = sp;
        Q_EMIT totalSpChanged();
    }
}

void Character::setIsAlphaClone(bool alpha) {
    if (m_isAlphaClone != alpha) {
        m_isAlphaClone = alpha;
        Q_EMIT isAlphaCloneChanged();
    }
}

void Character::setSkills(const QVector<CharacterSkill> &vskills)
{
    if (m_skills != vskills) {
        m_skills = vskills;
        // update skill groups model
        m_skillGroupsModel.setFromSkills(m_skills);

//        // find currently trained skill and last skill in queue
//        int indexInVector = 0;
//        int minQueuePos = INT_MAX;  // needed to find first skill in queue
//        int firstSkillIndex = -1;
//        int maxQueuePos = -1;  // needed to fins last skill in queue
//        int lastSkillIndex = -1;
//        for (const CharacterSkill &sk : qAsConst(m_skills)) {
//            int qpos = sk.queueInfo().queuePosition;
//            // is skill in queue?
//            if (qpos > 0) {
//                if (qpos < minQueuePos) {
//                    minQueuePos = qpos;
//                    firstSkillIndex = indexInVector;
//                }
//                if (qpos > maxQueuePos) {
//                    maxQueuePos = qpos;
//                    lastSkillIndex = indexInVector;
//                }
//            }
//            ++indexInVector;
//        }

//        if ((firstSkillIndex == -1) && (lastSkillIndex == -1)) {
//            // skill queue is empty!
//            qCWarning(logCharacter) << "Skill queue is empty!";
//            // zero out zurrent training skill skillId as indicator of empty queue
//            m_currentTrainingSkill.setSkillId(0);
//            m_currentTrainingSkill.setSkillName(QString());
//        } else {
//            // remember current training skill
//            const CharacterSkill &firstSkill = m_skills.at(firstSkillIndex);
//            m_currentTrainingSkill = firstSkill;

//            // remember when the whole skill queue ends
//            const CharacterSkill &lastSkill = m_skills.at(lastSkillIndex);
//            m_skillQueueFinishDate = lastSkill.queueInfo().finishDate;

//            // qCDebug(logCharacter) << "    First skill:" << firstSkill;
//            // qCDebug(logCharacter) << "    Last skill:" << lastSkill;
//            // ^^ looks correct

//            // also update skill groups model, tell where current training skill is
//            m_skillGroupsModel.setActiveTrainingGroupId(firstSkill.skillGroupId());
//        }

        Q_EMIT skillsChanged();
    }
}

void Character::setSkillQueue(const CharacterSkillQueue &queue)
{
    m_skillQueue = queue;
    calcSkillQueue();
}

// auth info
EveOAuthTokens Character::getAuthTokens() const { return m_tokens; }
void Character::setAuthTokens(const EveOAuthTokens& tokens) { m_tokens = tokens; }

UpdateTimestamps Character::updateTimestamps() const { return m_update_timestamps; }
void Character::setUpdateTimestamps(const UpdateTimestamps& ts) { m_update_timestamps = ts; }

void Character::setUpdateTimestamp(UpdateTimestamps::UTST kind)
{
    m_update_timestamps.updateTs(kind);
}

void Character::resetUpdateTimestamp(UpdateTimestamps::UTST kind) {
    m_update_timestamps.resetTs(kind);
}

void Character::forceRefreshPublicInfo() { resetUpdateTimestamp(UpdateTimestamps::UTST::PUBLIC); }
void Character::forceRefreshSkills() { resetUpdateTimestamp(UpdateTimestamps::UTST::SKILLS); }
void Character::forceRefreshWallet() { resetUpdateTimestamp(UpdateTimestamps::UTST::WALLET); }
void Character::forceRefreshLocation() { resetUpdateTimestamp(UpdateTimestamps::UTST::LOCATION); }
void Character::forceRefreshClones() { resetUpdateTimestamp(UpdateTimestamps::UTST::CLONES); }
void Character::forceRefreshAssets() { resetUpdateTimestamp(UpdateTimestamps::UTST::ASSETS); }

QList<QObject *> Character::getSkillsForGroupId(quint64 groupId) const
{
    QList<QObject *> ret;
    for (const CharacterSkill &sk: m_skills) {
        if (sk.skillGroupId() == groupId) {
            // insert a copy of skill to returned list
            ret.append(new CharacterSkill(sk));
            // qCDebug(logCharacter) << Q_FUNC_INFO << "group" << groupId << "; adding skill: " << sk;
        }
    }
    std::sort(ret.begin(), ret.end(), [](QObject *a, QObject *b) -> bool {
        CharacterSkill *sa = qobject_cast<CharacterSkill *>(a);
        CharacterSkill *sb = qobject_cast<CharacterSkill *>(b);
        return sa->skillName() < sb->skillName();
    });
    return ret;
}

void Character::clearSkillQueue()
{
    m_skillQueue.clear();
}

void Character::calcSkillQueue()
{
    m_skillQueue.calc(); // TODO
    Q_EMIT skillsChanged();
    Q_EMIT skillQueueChanged(); // NOTE: this needs to be kept here probably,
    // because this is called from setSkillQueue() and it needs to
    // emit a signal
}


} // namespace


// increase version number when savedata format changes
static const int SAVEDATA_VERSION = 15;


QDataStream& operator<<(QDataStream &stream, const EM::Character &character)
{
    // version
    stream << SAVEDATA_VERSION;
    // general
    stream << character.m_characterId;
    stream << character.m_characterName;
    stream << character.m_corporationId;
    stream << character.m_corporationName;
    stream << character.m_corporationTicker;
    stream << character.m_allianceId;
    stream << character.m_allianceName;
    stream << character.m_allianceTicker;
    // general - origins
    stream << character.m_raceId;
    stream << character.m_raceName;
    stream << character.m_ancestryId;
    stream << character.m_ancestryName;
    stream << character.m_bloodlineId;
    stream << character.m_bloodlineName;
    stream << character.m_gender;
    stream << character.m_birthday_dt;
    stream << character.m_securityStatus;
    stream << character.m_bio;
    // wallet info
    stream << character.m_isk;
    // location info
    stream << character.m_currentSolarSystemId;
    stream << character.m_currentSolarSystemName;
    stream << character.m_currentSolarSystemSecurity;
    stream << character.m_currentConstellationId;
    stream << character.m_currentConstellationName;
    stream << character.m_currentRegionId;
    stream << character.m_currentRegionName;
    stream << character.m_currentStationId;
    stream << character.m_currentStructureId;
    stream << character.m_currentStructureName;
    stream << character.m_currentShipTypeId;
    stream << character.m_currentShipTypeName;
    stream << character.m_currentShipFriendlyName;
    // skills and related
    stream << character.m_attributeCharisma;
    stream << character.m_attributeIntelligence;
    stream << character.m_attributeMemory;
    stream << character.m_attributePerception;
    stream << character.m_attributeWillpower;
    stream << character.m_numBonusRemaps;
    stream << character.m_lastRemapDate;
    stream << character.m_remapCooldownDate;
    stream << character.m_totalSp;
    stream << character.m_isAlphaClone;
    stream << character.m_skills;
    stream << character.m_skillQueue;
    // auth tokens
    stream << character.m_tokens;
    // update timestamps
    stream << character.m_update_timestamps;
    return stream;
}


QDataStream& operator>>(QDataStream &stream, EM::Character &character)
{
    // first read and check version
    int savedata_version = 0;
    stream >> savedata_version;
    if (savedata_version != SAVEDATA_VERSION) {
        // old versions are not supported :(
        character.setCharacterId(0);
        character.setCharacterName(QLatin1String("Not loaded"));
        qCDebug(logCharacter) << "Notice: skipped loading of old savedata version:"
                              << savedata_version;
        return stream;
    }
    // general
    stream >> character.m_characterId;
    stream >> character.m_characterName;
    stream >> character.m_corporationId;
    stream >> character.m_corporationName;
    stream >> character.m_corporationTicker;
    stream >> character.m_allianceId;
    stream >> character.m_allianceName;
    stream >> character.m_allianceTicker;
    // general - origins
    stream >> character.m_raceId;
    stream >> character.m_raceName;
    stream >> character.m_ancestryId;
    stream >> character.m_ancestryName;
    stream >> character.m_bloodlineId;
    stream >> character.m_bloodlineName;
    stream >> character.m_gender;
    stream >> character.m_birthday_dt;
    stream >> character.m_securityStatus;
    stream >> character.m_bio;
    // wallet info
    stream >> character.m_isk;
    // location info
    stream >> character.m_currentSolarSystemId;
    stream >> character.m_currentSolarSystemName;
    stream >> character.m_currentSolarSystemSecurity;
    stream >> character.m_currentConstellationId;
    stream >> character.m_currentConstellationName;
    stream >> character.m_currentRegionId;
    stream >> character.m_currentRegionName;
    stream >> character.m_currentStationId;
    stream >> character.m_currentStructureId;
    stream >> character.m_currentStructureName;
    stream >> character.m_currentShipTypeId;
    stream >> character.m_currentShipTypeName;
    stream >> character.m_currentShipFriendlyName;
    // skills and related
    stream >> character.m_attributeCharisma;
    stream >> character.m_attributeIntelligence;
    stream >> character.m_attributeMemory;
    stream >> character.m_attributePerception;
    stream >> character.m_attributeWillpower;
    stream >> character.m_numBonusRemaps;
    stream >> character.m_lastRemapDate;
    stream >> character.m_remapCooldownDate;
    stream >> character.m_totalSp;
    stream >> character.m_isAlphaClone;
    stream >> character.m_skills;
    stream >> character.m_skillQueue;
    // auth tokens
    stream >> character.m_tokens;
    // update timestamps
    stream >> character.m_update_timestamps;
    return stream;
}


QDebug operator<<(QDebug stream, const EM::Character &character)
{
    stream << character.toString();
    return stream;
}
