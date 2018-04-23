#ifndef H_EM_DB_SQLITE
#define H_EM_DB_SQLITE

#include <QSqlDatabase>
#include "db.h"


namespace EM {


class DbSqlite: public Db
{
public:
    DbSqlite();
    virtual ~DbSqlite();
    static Db* instance();

public:
    virtual bool loadCharacters(QVector<Character *>& charList) override;
    virtual bool saveCharacters(const QVector<Character *>& charList) override;
    virtual bool saveCharacter(const Character *character) override;

    virtual bool loadPortrait(quint64 char_id, QImage& img) override;
    virtual bool savePortrait(quint64 char_id, const QImage& img) override;
    virtual bool deletePortrait(quint64 char_id) override;

    virtual QString raceName(quint64 race_id) override;
    virtual QString bloodlineName(quint64 bloodline_id) override;
    virtual QString ancestryName(quint64 ancestry_id) override;
    virtual QString factionName(quint64 faction_id) override;
    virtual QString typeName(quint64 type_id) override;
    virtual QJsonObject typeInfo(quint64 type_id) override;
    virtual QJsonArray  typeAttributes(quint64 type_id) override;

    virtual QJsonArray loadSkillGroups() override;
    virtual QJsonArray loadSkillsInGroup(quint64 group_id) override;

    virtual QJsonObject loadCachedLocation(quint64 location_id) override;
    virtual bool saveCachedLocation(quint64 location_id, const QJsonObject &location) override;

protected:
    bool open(const QString& db_filename);
    bool open_sde(const QString& db_filename);
    bool open_cache(const QString& db_filename);
    void close();
    bool execSqlFile(QSqlDatabase *db, const QString& filename);

protected:
    QSqlDatabase m_chars_db;
    QSqlDatabase m_eve_sde_db;
    QSqlDatabase m_cache_db;
};


}  // namespace


#endif
