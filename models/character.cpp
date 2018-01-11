#include <QDataStream>
#include <QLocale>
#include <QLoggingCategory>
#include <QDebug>
#include "character.h"


Q_LOGGING_CATEGORY(logCharacter, "evemon.character")

namespace EM {

Character::Character():
    QObject(nullptr)
{
    //
}


Character::Character(const Character& other):
    QObject(nullptr)
{
    (*this) = other;
}


Character& Character::operator=(const Character& other)
{
    if (this == &other) return (*this);
    // general info
    m_characterId = other.m_characterId;
    m_characterName = other.m_characterName;
    m_corporationId = other.m_corporationId;
    m_corporationName = other.m_corporationName;
    m_corporationTicker = other.m_corporationTicker;
    m_allianceId = other.m_allianceId;
    m_allianceName = other.m_allianceName;
    m_allianceTicker = other.m_allianceTicker;
    // general - origins
    m_raceId = other.m_raceId;
    m_raceName = other.m_raceName;
    m_ancestryId = other.m_ancestryId;
    m_ancestryName = other.m_ancestryName;
    m_bloodlineId = other.m_bloodlineId;
    m_bloodlineName = other.m_bloodlineName;
    m_gender = other.m_gender;
    m_birthday_dt = other.m_birthday_dt;
    m_securityStatus = other.m_securityStatus;
    m_bio = other.m_bio;
    // wallet info
    m_isk = other.m_isk;
    // location info
    m_currentSolarSystemId = other.m_currentSolarSystemId;
    m_currentSolarSystemName = other.m_currentSolarSystemName;
    m_currentStationId = other.m_currentStationId;
    m_currentStructureId = other.m_currentStructureId;
    m_currentStructureName = other.m_currentStructureName;
    m_currentShipTypeId = other.m_currentShipTypeId;
    m_currentShipTypeName = other.m_currentShipTypeName;
    m_currentShipFriendlyName = other.m_currentShipFriendlyName;
    // auth info
    m_tokens = other.m_tokens;
    // last update date-times
    m_update_timestamps = other.m_update_timestamps;
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
    emit characterIdChanged();
}

QString Character::characterName() const { return m_characterName; }

void Character::setCharacterName(const QString& name) {
    if (name == m_characterName) return;
    m_characterName = name;
    emit characterNameChanged();
}

quint64 Character::corporationId() const { return m_corporationId; }

void Character::setCorporationId(quint64 corp_id) {
    if (corp_id == m_corporationId) return;
    m_corporationId = corp_id;
    emit corporationIdChanged();
}

QString Character::corporationName() const { return m_corporationName; }

void Character::setCorporationName(const QString& corp_name) {
    if (corp_name == m_corporationName) return;
    m_corporationName = corp_name;
    emit corporationNameChanged();
}

QString Character::corporationTicker() const { return m_corporationTicker; }

void Character::setCorporationTicker(const QString& ticker) {
    if (ticker == m_corporationTicker) return;
    m_corporationTicker = ticker;
    emit corporationTickerChanged();
}

quint64 Character::allianceId() const { return m_allianceId; }

void Character::setAllianceId(quint64 ally_id) {
    if (ally_id == m_allianceId) return;
    m_allianceId = ally_id;
    emit allianceIdChanged();
}

QString Character::allianceName() const { return m_allianceName; }

void Character::setAllianceName(const QString& ally_name) {
    if (m_allianceName == ally_name) return;
    m_allianceName = ally_name;
    emit allianceNameChanged();
}

QString Character::allianceTicker() const { return m_allianceTicker; }

void Character::setAllianceTicker(const QString& ticker) {
    if (ticker == m_allianceTicker) return;
    m_allianceTicker = ticker;
    emit allianceTickerChanged();
}

quint64 Character::raceId() const { return m_raceId; }
void Character::setRaceId(quint64 race_id) { m_raceId = race_id; }

QString Character::raceName() const { return m_raceName; }

void Character::setRaceName(const QString& race_name) {
    if (race_name == m_raceName) return;
    m_raceName = race_name;
    emit raceNameChanged();
}

quint64 Character::ancestryId() const { return m_ancestryId; }
void Character::setAncestryId(quint64 id) { m_ancestryId = id; }

QString Character::ancestryName() const { return m_ancestryName; }

void Character::setAncestryName(const QString& name) {
    if (name == m_ancestryName) return;
    m_ancestryName = name;
    emit ancestryNameChanged();
}

quint64 Character::bloodlineId() const { return m_bloodlineId; }
void Character::setBloodlineId(quint64 id) { m_bloodlineId = id; }

QString Character::bloodlineName() const { return m_bloodlineName; }

void Character::setBloodlineName(const QString& name) {
    if (name == m_bloodlineName) return;
    m_bloodlineName = name;
    emit bloodlineNameChanged();
}

int Character::gender() const { return m_gender; }

void Character::setGender(int g) {
    if (g == m_gender) return;
    m_gender = g;
    emit genderChanged();
}

QDateTime Character::birthday() const { return m_birthday_dt; }

void Character::setBirthday(const QDateTime& dt) {
    if (dt == m_birthday_dt) return;
    m_birthday_dt = dt;
    emit birthdayChanged();
}

float Character::securityStatus() const { return m_securityStatus; }

QString Character::securityStatusStr() const {
    return QString::number(static_cast<double>(m_securityStatus), 'f', 1);
}

void Character::setSecurityStatus(float ss) {
    if (qFuzzyCompare(ss, m_securityStatus)) return;
    m_securityStatus = ss;
    emit securityStatusChanged();
}

QString Character::bio() const { return m_bio; }

void Character::setBio(const QString& s) {
    if (m_bio == s) return;
    m_bio = s;
    emit bioChanged();
}

// wallet info
float Character::iskAmount() const { return m_isk; }

QString Character::iskAmountStr() const{
    return QLocale::system().toCurrencyString(m_isk, QLatin1String(" ISK"), 2);
}

void Character::setIskAmount(float isk) {
    if (qFuzzyCompare(isk, m_isk)) return;
    m_isk = isk;
    emit iskAmountChanged();
}

// location info
quint64 Character::currentSolarSystemId() const { return m_currentSolarSystemId; }

void Character::setCurrentSolarSystemId(quint64 id) {
    if (m_currentSolarSystemId == id) return;
    m_currentSolarSystemId = id;
    emit currentSolarSystemChanged();
}

QString Character::currentSolarSystemName() const { return m_currentSolarSystemName; }

void Character::setCurrentSolarSystemName(const QString& name) {
    if (m_currentSolarSystemName == name) return;
    m_currentSolarSystemName = name;
    emit currentSolarSystemChanged();
}

float Character::currentSolarSystemSecurity() const { return m_currentSolarSystemSecurity; }

QString Character::currentSolarSystemSecurityStr() const {
    return QString::number(static_cast<double>(m_currentSolarSystemSecurity), 'f', 2);
}

void Character::setCurrentSolarSystemSecurity(float ss) {
    if (qFuzzyCompare(m_currentSolarSystemSecurity, ss)) return;
    m_currentSolarSystemSecurity = ss;
    emit currentSolarSystemSecurityChanged();
}

quint64 Character::currentConstellationId() const { return m_currentConstellationId; }

void Character::setCurrentConstellationId(quint64 id) {
    if (m_currentConstellationId == id) return;
    m_currentConstellationId = id;
    emit currentConstellationChanged();
}

QString Character::currentConstellationName() const { return m_currentConstellationName; }

void Character::setCurrentConstellationName(const QString& name) {
    if (m_currentConstellationName == name) return;
    m_currentConstellationName = name;
    emit currentConstellationChanged();
}

quint64 Character::currentRegionId() const { return m_currentRegionId; }

void Character::setCurrentRegionId(quint64 id) {
    if (m_currentRegionId == id) return;
    m_currentRegionId = id;
    emit currentRegionChanged();
}

QString Character::currentRegionName() const { return m_currentRegionName; }

void Character::setCurrentRegionName(const QString& name) {
    if (m_currentRegionName == name) return;
    m_currentRegionName = name;
    emit currentRegionChanged();
}

quint64 Character::currentStationId() const { return m_currentStationId; }

void Character::setCurrentStationId(quint64 id) {
    if (m_currentStationId == id) return;
    m_currentStationId = id;
    emit isDockedChanged();
}

quint64 Character::currentStructureId() const { return m_currentStructureId; }

void Character::setCurrentStructureId(quint64 id) {
    if (m_currentStructureId == id) return;
    m_currentStructureId = id;
    emit isDockedChanged();
}

QString Character::currentStructureName() const { return m_currentStructureName; }

void Character::setCurrentStructureName(const QString& name) {
    if (m_currentStructureName == name) return;
    m_currentStructureName = name;
    emit isDockedChanged();
}

bool Character::isDocked() const { return ((m_currentStationId > 0) || (m_currentStructureId > 0)); }

quint64 Character::currentShipTypeId() const { return m_currentShipTypeId; }

void Character::setCurrentShipTypeId(quint64 id) {
    if (m_currentShipTypeId == id) return;
    m_currentShipTypeId = id;
    emit currentShipChanged();
}

QString Character::currentShipTypeName() const { return m_currentShipTypeName; }

void Character::setCurrentShipTypeName(const QString& name) {
    if (m_currentShipTypeName == name) return;
    m_currentShipTypeName = name;
    emit currentShipChanged();
}

QString Character::currentShipFriendlyName() const { return m_currentShipFriendlyName; }

void Character::setCurrentShipFriendlyName(const QString& name) {
    if (m_currentShipFriendlyName == name) return;
    m_currentShipFriendlyName = name;
    emit currentShipChanged();
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

void Character::setAttributeCharisma(int a) {
    if (m_attributeCharisma != a) {
        m_attributeCharisma = a;
        emit attributeCharismaChanged();
    }
}

void Character::setAttributeIntelligence(int a) {
    if (m_attributeIntelligence != a) {
        m_attributeIntelligence = a;
        emit attributeIntelligenceChanged();
    }
}

void Character::setAttributeMemory(int a) {
    if (m_attributeMemory != a) {
        m_attributeMemory = a;
        emit attributeMemoryChanged();
    }
}

void Character::setAttributePerception(int a) {
    if (m_attributePerception != a) {
        m_attributePerception = a;
        emit attributePerceptionChanged();
    }
}

void Character::setAttributeWillpower(int a) {
    if (m_attributeWillpower != a) {
        m_attributeWillpower = a;
        emit attributeWillpowerChanged();
    }
}

void Character::setNumBonusRemaps(int n) {
    if (m_numBonusRemaps != n) {
        m_numBonusRemaps = n;
        emit numBonusRemapsChanged();
    }
}

void Character::setLastRemapDate(const QDateTime& dt) {
    if (m_lastRemapDate != dt) {
        m_lastRemapDate = dt;
        emit lastRemapDateChanged();
    }
}

void Character::setRemapCooldownDate(const QDateTime& dt) {
    if (m_remapCooldownDate != dt) {
        m_remapCooldownDate = dt;
        emit remapCooldownDateChanged();
    }
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


} // namespace


// increase version number when savedata format changes
static const int SAVEDATA_VERSION = 7;


QDataStream& operator<<(QDataStream& stream, const EM::Character& character)
{
    // version
    stream << SAVEDATA_VERSION;
    // general
    stream << character.characterId();
    stream << character.characterName();
    stream << character.corporationId();
    stream << character.corporationName();
    stream << character.corporationTicker();
    stream << character.allianceId();
    stream << character.allianceName();
    stream << character.allianceTicker();
    // general - origins
    stream << character.raceId();
    stream << character.raceName();
    stream << character.ancestryId();
    stream << character.ancestryName();
    stream << character.bloodlineId();
    stream << character.bloodlineName();
    stream << character.gender();
    stream << character.birthday();
    stream << character.securityStatus();
    stream << character.bio();
    // wallet info
    stream << character.iskAmount();
    // location info
    stream << character.currentSolarSystemId();
    stream << character.currentSolarSystemName();
    stream << character.currentSolarSystemSecurity();
    stream << character.currentConstellationId();
    stream << character.currentConstellationName();
    stream << character.currentRegionId();
    stream << character.currentRegionName();
    stream << character.currentStationId();
    stream << character.currentStructureId();
    stream << character.currentStructureName();
    stream << character.currentShipTypeId();
    stream << character.currentShipTypeName();
    stream << character.currentShipFriendlyName();
    // skills and related
    stream << character.attributeCharisma();
    stream << character.attributeIntelligence();
    stream << character.attributeMemory();
    stream << character.attributePerception();
    stream << character.attributeWillpower();
    stream << character.numBonusRemaps();
    stream << character.lastRemapDate();
    stream << character.remapCooldownDate();
    // auth tokens
    stream << character.getAuthTokens();
    // update timestamps
    stream << character.updateTimestamps();
    return stream;
}


QDataStream& operator>>(QDataStream& stream, EM::Character& character)
{
    int savedata_version = 0;
    quint64 ui64 = 0;
    int i = 0;
    QString s;
    float fl = 0.0f;
    QDateTime dt;
    EM::EveOAuthTokens tokens;
    EM::UpdateTimestamps uts;

    // first read and check version
    stream >> savedata_version;
    if (savedata_version != SAVEDATA_VERSION) {
        // old versions not supported :(
        character.setCharacterId(0);
        character.setCharacterName(QLatin1String("Not loaded"));
        qCDebug(logCharacter) << "Notice: skipped loading of old savedata version:"
                              << savedata_version;
        return stream;
    }
    //
    stream >> ui64;   character.setCharacterId(ui64);
    stream >> s;      character.setCharacterName(s);
    stream >> ui64;   character.setCorporationId(ui64);
    stream >> s;      character.setCorporationName(s);
    stream >> s;      character.setCorporationTicker(s);
    stream >> ui64;   character.setAllianceId(ui64);
    stream >> s;      character.setAllianceName(s);
    stream >> s;      character.setAllianceTicker(s);
    stream >> ui64;   character.setRaceId(ui64);
    stream >> s;      character.setRaceName(s);
    stream >> ui64;   character.setAncestryId(ui64);
    stream >> s;      character.setAncestryName(s);
    stream >> ui64;   character.setBloodlineId(ui64);
    stream >> s;      character.setBloodlineName(s);
    stream >> i;      character.setGender(i);
    stream >> dt;     character.setBirthday(dt);
    stream >> fl;     character.setSecurityStatus(fl);
    stream >> s;      character.setBio(s);
    // wallet info
    stream >> fl;     character.setIskAmount(fl);
    // location info
    stream >> ui64;   character.setCurrentSolarSystemId(ui64);
    stream >> s;      character.setCurrentSolarSystemName(s);
    stream >> fl;     character.setCurrentSolarSystemSecurity(fl);
    stream >> ui64;   character.setCurrentConstellationId(ui64);
    stream >> s;      character.setCurrentConstellationName(s);
    stream >> ui64;   character.setCurrentRegionId(ui64);
    stream >> s;      character.setCurrentRegionName(s);
    stream >> ui64;   character.setCurrentStationId(ui64);
    stream >> ui64;   character.setCurrentStructureId(ui64);
    stream >> s;      character.setCurrentStructureName(s);
    stream >> ui64;   character.setCurrentShipTypeId(ui64);
    stream >> s;      character.setCurrentShipTypeName(s);
    stream >> s;      character.setCurrentShipFriendlyName(s);
    // skills and related
    stream >> i;      character.setAttributeCharisma(i);
    stream >> i;      character.setAttributeIntelligence(i);
    stream >> i;      character.setAttributeMemory(i);
    stream >> i;      character.setAttributePerception(i);
    stream >> i;      character.setAttributeWillpower(i);
    stream >> i;      character.setNumBonusRemaps(i);
    stream >> dt;     character.setLastRemapDate(dt);
    stream >> dt;     character.setRemapCooldownDate(dt);
    // auth tokens
    stream >> tokens; character.setAuthTokens(tokens);
    // update timestamps
    stream >> uts;    character.setUpdateTimestamps(uts);
    return stream;
}

