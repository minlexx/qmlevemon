#ifndef H_EM_CHARACTER
#define H_EM_CHARACTER

#include <QString>
#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QList>
#include <QDebug>

#include "character_skill.h"
#include "character_skillqueue.h"
#include "character_skillgroups_model.h"
#include "character_skillqueue_model.h"
#include "character_clone.h"
#include "character_clones_model.h"
#include "eve_location.h"
#include "mail_models.h"
#include "notification.h"
#include "eve_api/eve_api_tokens.h"
#include "update_timestamps.h"


class QDataStream;


// forward-declare class EM::Character
namespace EM { class Character; }
// forward-declare stream operators
QDataStream& operator<<(QDataStream &stream, const EM::Character &character);
QDataStream& operator>>(QDataStream &stream, EM::Character &character);


namespace EM {


class Character: public QObject
{
    Q_OBJECT
    // to allow QML bindings
    // general info
    Q_PROPERTY(quint64   characterId        READ characterId        NOTIFY characterIdChanged)
    Q_PROPERTY(QString   characterName      READ characterName      NOTIFY characterNameChanged)
    Q_PROPERTY(quint64   corporationId      READ corporationId      NOTIFY corporationIdChanged)
    Q_PROPERTY(QString   corporationName    READ corporationName    NOTIFY corporationNameChanged)
    Q_PROPERTY(QString   corporationTicker  READ corporationTicker  NOTIFY corporationTickerChanged)
    Q_PROPERTY(quint64   allianceId         READ allianceId         NOTIFY allianceIdChanged)
    Q_PROPERTY(QString   allianceName       READ allianceName       NOTIFY allianceNameChanged)
    Q_PROPERTY(QString   allianceTicker     READ allianceTicker     NOTIFY allianceTickerChanged)
    Q_PROPERTY(QString   raceName           READ raceName           NOTIFY raceNameChanged)
    Q_PROPERTY(QString   ancestryName       READ ancestryName       NOTIFY ancestryNameChanged)
    Q_PROPERTY(QString   bloodlineName      READ bloodlineName      NOTIFY bloodlineNameChanged)
    Q_PROPERTY(int       gender             READ gender             NOTIFY genderChanged)
    Q_PROPERTY(QDateTime birthday           READ birthday           NOTIFY birthdayChanged)
    Q_PROPERTY(float     securityStatus     READ securityStatus     NOTIFY securityStatusChanged)
    Q_PROPERTY(QString   securityStatusStr  READ securityStatusStr  NOTIFY securityStatusChanged)
    Q_PROPERTY(QString   bio                READ bio                NOTIFY bioChanged)
    // wallet info
    Q_PROPERTY(float     iskAmount          READ iskAmount          NOTIFY iskAmountChanged)
    Q_PROPERTY(QString   iskAmountStr       READ iskAmountStr       NOTIFY iskAmountChanged)
    // location info
    Q_PROPERTY(quint64   currentSolarSystemId       READ currentSolarSystemId       NOTIFY currentSolarSystemChanged)
    Q_PROPERTY(QString   currentSolarSystemName     READ currentSolarSystemName     NOTIFY currentSolarSystemChanged)
    Q_PROPERTY(float     currentSolarSystemSecurity READ currentSolarSystemSecurity NOTIFY currentSolarSystemSecurityChanged)
    Q_PROPERTY(QString   currentSolarSystemSecurityStr READ currentSolarSystemSecurityStr NOTIFY currentSolarSystemSecurityChanged)
    Q_PROPERTY(quint64   currentConstellationId     READ currentConstellationId     NOTIFY currentConstellationChanged)
    Q_PROPERTY(QString   currentConstellationName   READ currentConstellationName   NOTIFY currentConstellationChanged)
    Q_PROPERTY(quint64   currentRegionId            READ currentRegionId            NOTIFY currentRegionChanged)
    Q_PROPERTY(QString   currentRegionName          READ currentRegionName          NOTIFY currentRegionChanged)
    Q_PROPERTY(quint64   currentStationId           READ currentStationId           NOTIFY isDockedChanged)
    Q_PROPERTY(quint64   currentStructureId         READ currentStructureId         NOTIFY isDockedChanged)
    Q_PROPERTY(QString   currentStructureName       READ currentStructureName       NOTIFY isDockedChanged)
    Q_PROPERTY(bool      isDocked                   READ isDocked                   NOTIFY isDockedChanged)
    Q_PROPERTY(quint64   currentShipTypeId          READ currentShipTypeId          NOTIFY currentShipChanged)
    Q_PROPERTY(QString   currentShipTypeName        READ currentShipTypeName        NOTIFY currentShipChanged)
    Q_PROPERTY(QString   currentShipFriendlyName    READ currentShipFriendlyName    NOTIFY currentShipChanged)
    // skills and related
    Q_PROPERTY(int        attributeCharisma        READ attributeCharisma        NOTIFY attributeCharismaChanged)
    Q_PROPERTY(int        attributeIntelligence    READ attributeIntelligence    NOTIFY attributeIntelligenceChanged)
    Q_PROPERTY(int        attributeMemory          READ attributeMemory          NOTIFY attributeMemoryChanged)
    Q_PROPERTY(int        attributePerception      READ attributePerception      NOTIFY attributePerceptionChanged)
    Q_PROPERTY(int        attributeWillpower       READ attributeWillpower       NOTIFY attributeWillpowerChanged)
    Q_PROPERTY(int        numBonusRemaps           READ numBonusRemaps           NOTIFY numBonusRemapsChanged)
    Q_PROPERTY(QDateTime  lastRemapDate            READ lastRemapDate            NOTIFY lastRemapDateChanged)
    Q_PROPERTY(QDateTime  remapCooldownDate        READ remapCooldownDate        NOTIFY remapCooldownDateChanged)
    Q_PROPERTY(quint64    totalSp                  READ totalSp                  NOTIFY totalSpChanged)
    Q_PROPERTY(bool       isAlphaClone             READ isAlphaClone             NOTIFY isAlphaCloneChanged)
    Q_PROPERTY(QObject*   skillGroupsModel         READ skillGroupsModel         NOTIFY skillsChanged)
    Q_PROPERTY(int        totalSkills              READ totalSkills              NOTIFY skillsChanged)
    Q_PROPERTY(int        totalSkills5             READ totalSkills5             NOTIFY skillsChanged)
    // current training skill and queue info
    Q_PROPERTY(const QObject* currentTrainingSkill READ currentTrainingSkillObj  NOTIFY skillQueueChanged)
    Q_PROPERTY(bool       isSkillQueueEmpty        READ isSkillQueueEmpty        NOTIFY skillQueueChanged)
    Q_PROPERTY(QObject*   skillQueueModel          READ skillQueueModel          NOTIFY skillQueueChanged)
    // clones, implants, home station
    Q_PROPERTY(QObject*   currentClone             READ currentCloneObj          NOTIFY currentCloneChanged)
    Q_PROPERTY(QObject*   clonesModel              READ clonesModelObj           CONSTANT)
    Q_PROPERTY(QDateTime  lastCloneJumpDate        READ lastCloneJumpDate        NOTIFY lastCloneJumpDateChanged)
    Q_PROPERTY(QObject*   homeLocation             READ homeLocationObj          NOTIFY homeLocationChanged)
    // fatigue
    Q_PROPERTY(QDateTime  jumpFatigueExpireDate    READ jumpFatigueExpireDate    NOTIFY jumpFatigueExpireDateChanged)
    Q_PROPERTY(QDateTime  lastJumpDate             READ lastJumpDate             NOTIFY lastJumpDateChanged)
    // mail
    Q_PROPERTY(QObject*   mailLabels               READ mailLabelsObj            NOTIFY mailLabelsChanged)
    Q_PROPERTY(QObject*   mails                    READ mailsObj                 NOTIFY mailsChanged)
    Q_PROPERTY(QObject*   mailNotifications        READ mailsNotificationsObj    NOTIFY mailNotificationsChanged)

public:
    Character(QObject *parent = nullptr);
    Character(const Character& other);
    Character(Character&& other);
    Character& operator=(const Character& other);
    Character& operator=(Character&& other);
    bool operator==(const Character& other);

    QString toString() const;

public:
    quint64 characterId() const;
    void setCharacterId(quint64 char_id);

    QString characterName() const;
    void setCharacterName(const QString& name);

    quint64 corporationId() const;
    void setCorporationId(quint64 corp_id);

    QString corporationName() const;
    void setCorporationName(const QString& corp_name);

    QString corporationTicker() const;
    void setCorporationTicker(const QString& ticker);

    quint64 allianceId() const;
    void setAllianceId(quint64 ally_id);

    QString allianceName() const;
    void setAllianceName(const QString& ally_name);

    QString allianceTicker() const;
    void setAllianceTicker(const QString& ticker);

    quint64 raceId() const;
    void setRaceId(quint64 race_id);

    QString raceName() const;
    void setRaceName(const QString& race_name);

    quint64 ancestryId() const;
    void setAncestryId(quint64 id);

    QString ancestryName() const;
    void setAncestryName(const QString& name);

    quint64 bloodlineId() const;
    void setBloodlineId(quint64 id);

    QString bloodlineName() const;
    void setBloodlineName(const QString& name);

    int gender() const;  // 0 - male
    void setGender(int g);

    QDateTime birthday() const;
    void setBirthday(const QDateTime& dt);

    float securityStatus() const;
    QString securityStatusStr() const;
    void setSecurityStatus(float ss);

    QString bio() const;
    void setBio(const QString& s);

    // wallet info
    float iskAmount() const;
    QString iskAmountStr() const;
    void setIskAmount(float isk);

    // location info
    quint64 currentSolarSystemId() const;
    void setCurrentSolarSystemId(quint64 id);

    QString currentSolarSystemName() const;
    void setCurrentSolarSystemName(const QString& name);

    float currentSolarSystemSecurity() const;
    QString currentSolarSystemSecurityStr() const;
    void setCurrentSolarSystemSecurity(float ss);

    quint64 currentConstellationId() const;
    void setCurrentConstellationId(quint64 id);

    QString currentConstellationName() const;
    void setCurrentConstellationName(const QString& name);

    quint64 currentRegionId() const;
    void setCurrentRegionId(quint64 id);

    QString currentRegionName() const;
    void setCurrentRegionName(const QString& name);

    quint64 currentStationId() const;
    void setCurrentStationId(quint64 id);

    quint64 currentStructureId() const;
    void setCurrentStructureId(quint64 id);

    QString currentStructureName() const;
    void setCurrentStructureName(const QString& name);

    bool    isDocked() const;

    quint64 currentShipTypeId() const;
    void setCurrentShipTypeId(quint64 id);

    QString currentShipTypeName() const;
    void setCurrentShipTypeName(const QString& name);

    QString currentShipFriendlyName() const;
    void setCurrentShipFriendlyName(const QString& name);

    // skills and related
    int attributeCharisma() const;
    int attributeIntelligence() const;
    int attributeMemory() const;
    int attributePerception() const;
    int attributeWillpower() const;
    int getAttributeValueByAttributeCode(int code) const;
    int numBonusRemaps() const;
    QDateTime lastRemapDate() const;
    QDateTime remapCooldownDate() const;
    quint64 totalSp() const;
    int totalSkills() const;
    int totalSkills5() const;
    bool isAlphaClone() const;
    QVector<CharacterSkill> skills() const;
    const QVector<CharacterSkill> &skillsRef() const;
    QObject *skillGroupsModel();
    // current training skill and queue info
    const QObject  *currentTrainingSkillObj() const;
    CharacterSkill *currentTrainingSkill();
    const CharacterSkill *currentTrainingSkill() const;
    CharacterSkillQueue& skillQueue();
    const CharacterSkillQueue& skillQueue() const;
    bool isSkillQueueEmpty() const;
    QObject *skillQueueModel();

    // skills and related - setters
    void setAttributeCharisma(int a);
    void setAttributeIntelligence(int a);
    void setAttributeMemory(int a);
    void setAttributePerception(int a);
    void setAttributeWillpower(int a);
    void setNumBonusRemaps(int n);
    void setLastRemapDate(const QDateTime& dt);
    void setRemapCooldownDate(const QDateTime& dt);
    void setTotalSp(quint64 sp);
    void setIsAlphaClone(bool alpha);
    void setSkills(const QVector<CharacterSkill> &vskills);
    void setSkillQueue(const CharacterSkillQueue &queue);

    // clones, implants, home station
    QObject *currentCloneObj();
    CharacterClone *currentClone();
    const CharacterClone *findCloneById(quint64 cloneId) const;
    void setCurrentClone(const CharacterClone &clon);

    CharacterClonesModel *clonesModel();
    const CharacterClonesModel *clonesModel() const;
    QObject *clonesModelObj();
    void setClones(const QVector<CharacterClone> &clones);

    QDateTime lastCloneJumpDate() const;
    void setLastCloneJumpDate(const QDateTime &dt);

    QObject *homeLocationObj();
    EveLocation *homeLocation();
    void setHomeLocation(const EveLocation &loc);

    // fatigue
    QDateTime jumpFatigueExpireDate() const;
    void setJumpFatigueExpireDate(const QDateTime &dt);
    QDateTime lastJumpDate() const;
    void setLastJumpDate(const QDateTime &dt);

    // mails
    QObject *mailLabelsObj();
    CharacterMailLabels *mailLabels();
    const CharacterMailLabels *mailLabels() const;
    void setMailLabels(const CharacterMailLabels &newLabels);

    QObject *mailsObj();
    CharacterMails *mails();
    const CharacterMails *mails() const;
    void setMails(const CharacterMails &newMails);
    Q_INVOKABLE QObject *filterMailsForLabel(quint64 mailLabelId);
    QObject *mailsNotificationsObj();
    CharacterNotifications *mailNotifications();
    const CharacterNotifications *mailNotifications() const;
    void setMailNotifications(const CharacterNotifications &ntfs);

    const QVector<MailRecipient> &mailingLists() const;
    void setMailingLists(const QVector<MailRecipient> &lists);

    void setMailBody(quint64 mailId, const QString &body);
    bool findMailById(quint64 mailId, Mail &mail) const;

    // auth info
    EveOAuthTokens getAuthTokens() const;
    void setAuthTokens(const EveOAuthTokens& tokens);

    UpdateTimestamps updateTimestamps() const;
    void setUpdateTimestamps(const UpdateTimestamps& ts);
    void setUpdateTimestamp(UpdateTimestamps::UTST kind);
    void resetUpdateTimestamp(UpdateTimestamps::UTST kind);

public Q_SLOTS:
    void forceRefreshPublicInfo();
    void forceRefreshSkills();
    void forceRefreshWallet();
    void forceRefreshWalletJournal();
    void forceRefreshLocation();
    void forceRefreshClones();
    void forceRefreshAssets();
    void forceRefreshFatigue();
    void forceRefreshMail();
    void forceRefreshNotifications();
    // ^^ add more, if needed

    QList<QObject *> getSkillsForGroupId(quint64 groupId) const;
    void clearSkillQueue();
    void calcSkillQueue();

protected:
    void updateSkillGroupsModel();
    void updateSkillQueueModel();
    CharacterSkill *int_findSkill(quint64 skillId);
    const CharacterSkill *int_findSkill(quint64 skillId) const;

Q_SIGNALS:
    // general info
    void characterIdChanged();
    void characterNameChanged();
    void corporationIdChanged();
    void corporationNameChanged();
    void corporationTickerChanged();
    void allianceIdChanged();
    void allianceNameChanged();
    void allianceTickerChanged();
    void raceNameChanged();
    void ancestryNameChanged();
    void bloodlineNameChanged();
    void genderChanged();
    void birthdayChanged();
    void securityStatusChanged();
    void bioChanged();
    // wallet info
    void iskAmountChanged();
    // location info
    void currentSolarSystemChanged();
    void currentSolarSystemSecurityChanged();
    void currentConstellationChanged();
    void currentRegionChanged();
    void isDockedChanged();
    void currentShipChanged();
    // skills and related
    void attributeCharismaChanged();
    void attributeIntelligenceChanged();
    void attributeMemoryChanged();
    void attributePerceptionChanged();
    void attributeWillpowerChanged();
    void numBonusRemapsChanged();
    void lastRemapDateChanged();
    void remapCooldownDateChanged();
    void totalSpChanged();
    void isAlphaCloneChanged();
    // current training skill and queue info
    void skillsChanged();
    void skillQueueChanged();
    // clones, home station
    void currentCloneChanged();
    void lastCloneJumpDateChanged();
    void homeLocationChanged();
    // fatigue
    void jumpFatigueExpireDateChanged();
    void lastJumpDateChanged();
    // mails
    void mailLabelsChanged();
    void mailsChanged();
    void mailNotificationsChanged();

    // for manual emitting, not for QML properties
Q_SIGNALS:
    void skillTrainingCompleted(Character *ch, const CharacterSkill &skill);

protected:
    // general info
    quint64   m_characterId = 0;
    QString   m_characterName;
    quint64   m_corporationId = 0;
    QString   m_corporationName;
    QString   m_corporationTicker;
    quint64   m_allianceId = 0;
    QString   m_allianceName;
    QString   m_allianceTicker;
    // general - origins
    quint64   m_raceId = 0;
    QString   m_raceName;
    quint64   m_ancestryId = 0;
    QString   m_ancestryName;
    quint64   m_bloodlineId = 0;
    QString   m_bloodlineName;
    int       m_gender = 0;  // 0 - male
    QDateTime m_birthday_dt;
    float     m_securityStatus = 0.0f;
    QString   m_bio;

    // wallet info
    float     m_isk = 0.0f;

    // location info
    quint64   m_currentSolarSystemId = 0;
    QString   m_currentSolarSystemName;
    float     m_currentSolarSystemSecurity = 0.0f;
    quint64   m_currentConstellationId = 0;
    QString   m_currentConstellationName;
    quint64   m_currentRegionId = 0;
    QString   m_currentRegionName;
    // location - station
    quint64   m_currentStationId = 0;
    quint64   m_currentStructureId = 0;
    QString   m_currentStructureName;
    // location - ship
    quint64   m_currentShipTypeId = 0;
    QString   m_currentShipTypeName;
    QString   m_currentShipFriendlyName;

    // skills and related
    // attributes
    int m_attributeCharisma = 0;
    int m_attributeIntelligence = 0;
    int m_attributeMemory = 0;
    int m_attributePerception = 0;
    int m_attributeWillpower = 0;
    int m_numBonusRemaps = 0;
    QDateTime m_lastRemapDate;
    QDateTime m_remapCooldownDate;
    quint64 m_totalSp = 0;
    int m_totalSkills = 0;
    int m_totalSkills5 = 0; // skills at level 5
    bool m_isAlphaClone = false;
    QVector<CharacterSkill> m_skills;
    CharacterSkillGroupsModel m_skillGroupsModel;
    // skill queue
    CharacterSkillQueue m_skillQueue;
    CharacterSkillQueueModel m_skillQueueModel;

    // clones, implants, home station
    CharacterClone m_currentClone;
    CharacterClonesModel m_clonesModel;
    QDateTime m_lastCloneJumpDate;
    EveLocation m_homeLocation;
    // fatigue
    QDateTime m_jumpFatigueExpireDate;
    QDateTime m_lastJumpDate;

    // mails
    CharacterMailLabels m_mailLabels;
    CharacterMails m_mails;
    QVector<MailRecipient> m_mailingLists;
    MailLabelFilteredMailsModel *m_mailsModel = nullptr;
    CharacterNotifications m_mailNotifications;

    // auth info
    EveOAuthTokens m_tokens;

    // last update date-times
    UpdateTimestamps m_update_timestamps;

    friend QDataStream& (::operator<<)(QDataStream &stream, const EM::Character &character);
    friend QDataStream& (::operator>>)(QDataStream &stream, EM::Character &character);
};


} // namespace


// serializing functions
QDataStream& operator<<(QDataStream &stream, const EM::Character &character);
QDataStream& operator>>(QDataStream &stream, EM::Character &character);

// debug output function
QDebug operator<<(QDebug stream, const EM::Character &character);

#endif
