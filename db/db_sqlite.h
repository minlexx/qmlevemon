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
    virtual bool loadCharacters(QList<Character *>& charList) override;
    virtual bool saveCharacters(const QList<Character *>& charList) override;
    virtual bool saveCharacter(const Character *character) override;

    virtual bool loadPortrait(quint64 char_id, QImage& img) override;
    virtual bool savePortrait(quint64 char_id, const QImage& img) override;
    virtual bool deletePortrait(quint64 char_id) override;

    virtual QString typeName(quint64 type_id) override;

protected:
    bool open(const QString& db_filename);
    bool open_sde(const QString& db_filename);
    void close();
    bool execSqlFile(QSqlDatabase *db, const QString& filename);

protected:
    QSqlDatabase m_chars_db;
    QSqlDatabase m_eve_sde_db;
};


}  // namespace


#endif
