#ifndef H_EM_DATABASE
#define H_EM_DATABASE


#include <QtGlobal>
#include <QVector>
#include <QImage>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>


namespace EM {


class Character;

/**
 * @brief The Db class
 * Very thin interface of storage for the app
 */
class Db
{
public:
    Db();
    virtual ~Db();

public:
    virtual bool loadCharacters(QVector<Character *>& charList) = 0;
    virtual bool saveCharacters(const QVector<Character *>& charList) = 0;
    virtual bool saveCharacter(const Character *character) = 0;

    virtual bool loadPortrait(quint64 char_id, QImage& img) = 0;
    virtual bool savePortrait(quint64 char_id, const QImage& img) = 0;
    virtual bool deletePortrait(quint64 char_id) = 0;

    virtual QString raceName(quint64 race_id) = 0;
    virtual QString bloodlineName(quint64 bloodline_id) = 0;
    virtual QString ancestryName(quint64 ancestry_id) = 0;
    virtual QString factionName(quint64 faction_id) = 0;

    virtual QString     typeName(quint64 type_id) = 0;
    virtual QJsonObject typeInfo(quint64 type_id) = 0;
    virtual QJsonArray  typeAttributes(quint64 type_id) = 0;

    virtual QJsonArray loadSkillGroups() = 0;
    virtual QJsonArray loadSkillsInGroup(quint64 group_id) = 0;

    virtual QJsonObject loadCachedLocation(quint64 location_id) = 0;
    virtual bool        saveCachedLocation(quint64 location_id, const QJsonObject &location) = 0;

    virtual bool loadTypeIcon(quint64 type_id, QImage& img) = 0;
    virtual bool saveTypeIcon(quint64 type_id, const QImage& img) = 0;

    virtual QString findCachedCharacterName(quint64 char_id) = 0;
    virtual QString findCachedCorporationName(quint64 corp_id) = 0;
    virtual QString findCachedAllianceName(quint64 ally_id) = 0;
    virtual bool saveCachedCharacterName(quint64 char_id, const QString &name) = 0;
    virtual bool saveCachedCorporationName(quint64 corp_id, const QString &name) = 0;
    virtual bool saveCachedAllianceName(quint64 ally_id, const QString &name) = 0;

    virtual QJsonObject loadMailBody(quint64 char_id, quint64 mail_id) = 0;
    virtual bool saveMailBody(quint64 char_id, quint64 mail_id, const QJsonObject &mailBody) = 0;

protected:
    Q_DISABLE_COPY(Db)
};


} // namespace


#endif
