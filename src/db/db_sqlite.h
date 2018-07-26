#ifndef H_EM_DB_SQLITE
#define H_EM_DB_SQLITE

#include <QSqlDatabase>
#include <QMutex>
#include "db.h"


namespace EM {


class DbSqlite: public Db
{
public:
    DbSqlite();
    ~DbSqlite() override;
    static Db* instance();

public:
    bool loadCharacters(QVector<Character *> &charList) override;
    bool saveCharacters(const QVector<Character *> &charList) override;
    bool saveCharacter(const Character *character) override;

    bool loadPortrait(quint64 char_id, QImage &img) override;
    bool savePortrait(quint64 char_id, const QImage &img) override;
    bool deletePortrait(quint64 char_id) override;

    QString raceName(quint64 race_id) override;
    QString bloodlineName(quint64 bloodline_id) override;
    QString ancestryName(quint64 ancestry_id) override;
    QString factionName(quint64 faction_id) override;
    QString typeName(quint64 type_id) override;
    QJsonObject typeInfo(quint64 type_id) override;
    QJsonArray  typeAttributes(quint64 type_id) override;

    QJsonArray loadSkillGroups() override;
    QJsonArray loadSkillsInGroup(quint64 group_id) override;

    QJsonObject loadCachedLocation(quint64 location_id) override;
    bool saveCachedLocation(quint64 location_id, const QJsonObject &location) override;

    bool loadTypeIcon(quint64 type_id, QImage &img) override;
    bool saveTypeIcon(quint64 type_id, const QImage &img) override;

    QString findCachedCharacterName(quint64 char_id) override;
    QString findCachedCorporationName(quint64 corp_id) override;
    QString findCachedAllianceName(quint64 ally_id) override;
    bool saveCachedCharacterName(quint64 char_id, const QString &name) override;
    bool saveCachedCorporationName(quint64 corp_id, const QString &name) override;
    bool saveCachedAllianceName(quint64 ally_id, const QString &name) override;

    QJsonObject loadMailBody(quint64 char_id, quint64 mail_id) override;
    bool saveMailBody(quint64 char_id, quint64 mail_id, const QJsonObject &mailBody) override;

protected:
    bool open_chars(const QString &db_filename);
    bool open_sde(const QString &db_filename);
    bool open_cache(const QString &db_filename);
    void close();
    bool execSqlFile(QSqlDatabase *db, const QString &filename);

protected:
    QSqlDatabase m_chars_db;
    QMutex m_chars_mutex;

    QSqlDatabase m_eve_sde_db;
    QMutex m_sde_mutex;

    QSqlDatabase m_cache_db;
    QMutex m_cache_mutex;
};


}  // namespace


#endif
