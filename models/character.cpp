#include <QDataStream>
#include <QLocale>
#include "character.h"


EM::Character::Character():
    QObject(nullptr),
    m_characterId(0),
    m_corporationId(0),
    m_allianceId(0),
    m_raceId(0),
    m_ancestryId(0),
    m_bloodlineId(0),
    m_gender(0),
    m_isk(0.0f),
    m_securityStatus(0.0f),
    m_currentSolarSystemId(0),
    m_currentSolarSystemSecurity(0.0f),
    m_currentConstellationId(0),
    m_currentRegionId(0),
    m_currentStationId(0),
    m_currentStructureId(0),
    m_currentShipTypeId(0)
{
    //
}


EM::Character::Character(const EM::Character& other):
    QObject(nullptr)
{
    (*this) = other;
}


EM::Character& EM::Character::operator=(const EM::Character& other)
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


bool EM::Character::operator==(const Character& other)
{
    if (this == &other) return true;
    return (this->m_characterId == other.m_characterId);
}


QString EM::Character::toString() const
{
    QString ret("<EM::Character ");
    ret.append(m_characterName);
    ret.append("; id=");
    ret.append(QString::number(m_characterId));
    ret.append(">");
    return ret;
}


quint64 EM::Character::characterId() const { return m_characterId; }

void EM::Character::setCharacterId(quint64 char_id) {
    if (char_id == m_characterId) return;
    m_characterId = char_id;
    emit characterIdChanged();
}

QString EM::Character::characterName() const { return m_characterName; }

void EM::Character::setCharacterName(const QString& name) {
    if (name == m_characterName) return;
    m_characterName = name;
    emit characterNameChanged();
}

quint64 EM::Character::corporationId() const { return m_corporationId; }

void EM::Character::setCorporationId(quint64 corp_id) {
    if (corp_id == m_corporationId) return;
    m_corporationId = corp_id;
    emit corporationIdChanged();
}

QString EM::Character::corporationName() const { return m_corporationName; }

void EM::Character::setCorporationName(const QString& corp_name) {
    if (corp_name == m_corporationName) return;
    m_corporationName = corp_name;
    emit corporationNameChanged();
}

QString EM::Character::corporationTicker() const { return m_corporationTicker; }

void EM::Character::setCorporationTicker(const QString& ticker) {
    if (ticker == m_corporationTicker) return;
    m_corporationTicker = ticker;
    emit corporationTickerChanged();
}

quint64 EM::Character::allianceId() const { return m_allianceId; }

void EM::Character::setAllianceId(quint64 ally_id) {
    if (ally_id == m_allianceId) return;
    m_allianceId = ally_id;
    emit allianceIdChanged();
}

QString EM::Character::allianceName() const { return m_allianceName; }

void EM::Character::setAllianceName(const QString& ally_name) {
    if (m_allianceName == ally_name) return;
    m_allianceName = ally_name;
    emit allianceNameChanged();
}

QString EM::Character::allianceTicker() const { return m_allianceTicker; }

void EM::Character::setAllianceTicker(const QString& ticker) {
    if (ticker == m_allianceTicker) return;
    m_allianceTicker = ticker;
    emit allianceTickerChanged();
}

quint64 EM::Character::raceId() const { return m_raceId; }
void EM::Character::setRaceId(quint64 race_id) { m_raceId = race_id; }

QString EM::Character::raceName() const { return m_raceName; }

void EM::Character::setRaceName(const QString& race_name) {
    if (race_name == m_raceName) return;
    m_raceName = race_name;
    emit raceNameChanged();
}

quint64 EM::Character::ancestryId() const { return m_ancestryId; }
void EM::Character::setAncestryId(quint64 id) { m_ancestryId = id; }

QString EM::Character::ancestryName() const { return m_ancestryName; }

void EM::Character::setAncestryName(const QString& name) {
    if (name == m_ancestryName) return;
    m_ancestryName = name;
    emit ancestryNameChanged();
}

quint64 EM::Character::bloodlineId() const { return m_bloodlineId; }
void EM::Character::setBloodlineId(quint64 id) { m_bloodlineId = id; }

QString EM::Character::bloodlineName() const { return m_bloodlineName; }

void EM::Character::setBloodlineName(const QString& name) {
    if (name == m_bloodlineName) return;
    m_bloodlineName = name;
    emit bloodlineNameChanged();
}

int EM::Character::gender() const { return m_gender; }

void EM::Character::setGender(int g) {
    if (g == m_gender) return;
    m_gender = g;
    emit genderChanged();
}

QDateTime EM::Character::birthday() const { return m_birthday_dt; }

void EM::Character::setBirthday(const QDateTime& dt) {
    if (dt == m_birthday_dt) return;
    m_birthday_dt = dt;
    emit birthdayChanged();
}

float EM::Character::securityStatus() const { return m_securityStatus; }

QString EM::Character::securityStatusStr() const {
    return QString::number(static_cast<double>(m_securityStatus), 'f', 1);
}

void EM::Character::setSecurityStatus(float ss) {
    if (qFuzzyCompare(ss, m_securityStatus)) return;
    m_securityStatus = ss;
    emit securityStatusChanged();
}

QString EM::Character::bio() const { return m_bio; }

void EM::Character::setBio(const QString& s) {
    if (m_bio == s) return;
    m_bio = s;
    emit bioChanged();
}

// wallet info
float EM::Character::iskAmount() const { return m_isk; }

QString EM::Character::iskAmountStr() const{
    return QLocale::system().toCurrencyString(m_isk, " ISK", 2);
}

void EM::Character::setIskAmount(float isk) {
    if (qFuzzyCompare(isk, m_isk)) return;
    m_isk = isk;
    emit iskAmountChanged();
}

// location info
quint64 EM::Character::currentSolarSystemId() const { return m_currentSolarSystemId; }

void EM::Character::setCurrentSolarSystemId(quint64 id) {
    if (m_currentSolarSystemId == id) return;
    m_currentSolarSystemId = id;
    emit currentSolarSystemChanged();
}

QString EM::Character::currentSolarSystemName() const { return m_currentSolarSystemName; }

void EM::Character::setCurrentSolarSystemName(const QString& name) {
    if (m_currentSolarSystemName == name) return;
    m_currentSolarSystemName = name;
    emit currentSolarSystemChanged();
}

float EM::Character::currentSolarSystemSecurity() const { return m_currentSolarSystemSecurity; }

QString EM::Character::currentSolarSystemSecurityStr() const {
    return QString::number(static_cast<double>(m_currentSolarSystemSecurity), 'f', 2);
}

void EM::Character::setCurrentSolarSystemSecurity(float ss) {
    if (qFuzzyCompare(m_currentSolarSystemSecurity, ss)) return;
    m_currentSolarSystemSecurity = ss;
    emit currentSolarSystemSecurityChanged();
}

quint64 EM::Character::currentConstellationId() const { return m_currentConstellationId; }

void EM::Character::setCurrentConstellationId(quint64 id) {
    if (m_currentConstellationId == id) return;
    m_currentConstellationId = id;
    emit currentConstellationChanged();
}

QString EM::Character::currentConstellationName() const { return m_currentConstellationName; }

void EM::Character::setCurrentConstellationName(const QString& name) {
    if (m_currentConstellationName == name) return;
    m_currentConstellationName = name;
    emit currentConstellationChanged();
}

quint64 EM::Character::currentRegionId() const { return m_currentRegionId; }

void EM::Character::setCurrentRegionId(quint64 id) {
    if (m_currentRegionId == id) return;
    m_currentRegionId = id;
    emit currentRegionChanged();
}

QString EM::Character::currentRegionName() const { return m_currentRegionName; }

void EM::Character::setCurrentRegionName(const QString& name) {
    if (m_currentRegionName == name) return;
    m_currentRegionName = name;
    emit currentRegionChanged();
}

quint64 EM::Character::currentStationId() const { return m_currentStationId; }

void EM::Character::setCurrentStationId(quint64 id) {
    if (m_currentStationId == id) return;
    m_currentStationId = id;
    emit isDockedChanged();
}

quint64 EM::Character::currentStructureId() const { return m_currentStructureId; }

void EM::Character::setCurrentStructureId(quint64 id) {
    if (m_currentStructureId == id) return;
    m_currentStructureId = id;
    emit isDockedChanged();
}

QString EM::Character::currentStructureName() const { return m_currentStructureName; }

void EM::Character::setCurrentStructureName(const QString& name) {
    if (m_currentStructureName == name) return;
    m_currentStructureName = name;
    emit isDockedChanged();
}

bool    EM::Character::isDocked() const { return ((m_currentStationId > 0) || (m_currentStructureId > 0)); }

quint64 EM::Character::currentShipTypeId() const { return m_currentShipTypeId; }

void EM::Character::setCurrentShipTypeId(quint64 id) {
    if (m_currentShipTypeId == id) return;
    m_currentShipTypeId = id;
    emit currentShipChanged();
}

QString EM::Character::currentShipTypeName() const { return m_currentShipTypeName; }

void EM::Character::setCurrentShipTypeName(const QString& name) {
    if (m_currentShipTypeName == name) return;
    m_currentShipTypeName = name;
    emit currentShipChanged();
}

QString EM::Character::currentShipFriendlyName() const { return m_currentShipFriendlyName; }

void EM::Character::setCurrentShipFriendlyName(const QString& name) {
    if (m_currentShipFriendlyName == name) return;
    m_currentShipFriendlyName = name;
    emit currentShipChanged();
}

// skills and related
int EM::Character::attributeCharisma() const { return m_attributeCharisma; }
int EM::Character::attributeIntelligence() const { return m_attributeIntelligence; }
int EM::Character::attributeMemory() const { return m_attributeMemory; }
int EM::Character::attributePerception() const { return m_attributePerception; }
int EM::Character::attributeWillpower() const { return m_attributeWillpower; }

void EM::Character::setAttributeCharisma(int a) {
    if (m_attributeCharisma != a) {
        m_attributeCharisma = a;
        emit attributeCharismaChanged();
    }
}

void EM::Character::setAttributeIntelligence(int a) {
    if (m_attributeIntelligence != a) {
        m_attributeIntelligence = a;
        emit attributeIntelligenceChanged();
    }
}

void EM::Character::setAattributeMemory(int a) {
    if (m_attributeMemory != a) {
        m_attributeMemory = a;
        emit attributeMemoryChanged();
    }
}

void EM::Character::setAttributePerception(int a) {
    if (m_attributePerception != a) {
        m_attributePerception = a;
        emit attributePerceptionChanged();
    }
}

void EM::Character::setAttributeWillpower(int a) {
    if (m_attributeWillpower != a) {
        m_attributeWillpower = a;
        emit attributeWillpowerChanged();
    }
}

// auth info
EM::EveOAuthTokens EM::Character::getAuthTokens() const { return m_tokens; }
void EM::Character::setAuthTokens(const EveOAuthTokens& tokens) { m_tokens = tokens; }

EM::UpdateTimestamps EM::Character::updateTimestamps() const { return m_update_timestamps; }
void EM::Character::setUpdateTimestamps(const EM::UpdateTimestamps& ts) { m_update_timestamps = ts; }

void EM::Character::setUpdateTimestamp(UpdateTimestamps::UTST kind)
{
    m_update_timestamps.updateTs(kind);
}



// increase version number when savedata format changes
static const int SAVEDATA_VERSION = 5;


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
    // auth tokens
    stream >> tokens; character.setAuthTokens(tokens);
    // update timestamps
    stream >> uts;    character.setUpdateTimestamps(uts);
    return stream;
}
