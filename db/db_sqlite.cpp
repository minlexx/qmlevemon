#include <QGlobalStatic>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QDataStream>
#include <QBuffer>
#include <QFile>
#include <QTextStream>
#include <QLoggingCategory>
#include <QDebug>

#include "db_sqlite.h"
#include "models/character.h"
#include "qmlevemon_app.h"


Q_LOGGING_CATEGORY(logDb, "evemon.db")


namespace EM {

Q_GLOBAL_STATIC(DbSqlite, g_em_db_sqlite)


DbSqlite::DbSqlite()
{
    QString appdata_dirname = globalAppInstance()->storageDirectory();
    QString db_filename = appdata_dirname + QLatin1String("/characters.db");
    QString sde_db_filename = appdata_dirname + QLatin1String("/eve_sde.db");
    this->open(db_filename);
    this->open_sde(sde_db_filename);
}


DbSqlite::~DbSqlite()
{
    close();
}


Db* DbSqlite::instance()
{
    return g_em_db_sqlite();
}


bool DbSqlite::open(const QString& db_filename)
{
    if (m_chars_db.isOpen()) {
        qCWarning(logDb) << "cannot open DB: already opened!";
        return false;
    }
    m_chars_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), QLatin1String("characters"));
    if (!m_chars_db.isValid()) {
        qCWarning(logDb) << "cannot open DB: failed to load sqlite driver?!";
        return false;
    }
    m_chars_db.setDatabaseName(db_filename);
    if (!m_chars_db.open()) {
        qCWarning(logDb) << "cannot open DB: failed to open file:" << db_filename;
        return false;
    }
    qCDebug(logDb) << "Opened chars database:" << db_filename;

    // check for missing tables
    QStringList existing_tables;
    QSqlQuery q(m_chars_db);
    if (q.exec(QLatin1String("SELECT name FROM sqlite_master WHERE type='table'"))) {
        while(q.next()) {
            existing_tables << q.value(0).toString();
        }
    }
    q.clear();

    if (!existing_tables.contains(QLatin1String("characters"))) {
        qCDebug(logDb) << "Creating table characters...";
        q.exec(QLatin1String("CREATE TABLE characters("
               "    char_id INTEGER PRIMARY KEY NOT NULL,"
               "    char_data BLOB)"));
        q.clear();
    }

    if (!existing_tables.contains(QLatin1String("portraits"))) {
        qCDebug(logDb) << "Creating table portraits...";
        q.exec(QLatin1String("CREATE TABLE portraits("
               "    char_id INTEGER PRIMARY KEY NOT NULL,"
               "    picture BLOB)"));
        q.clear();
    }

    return true;
}


bool DbSqlite::open_sde(const QString& db_filename)
{
    if (m_eve_sde_db.isOpen()) {
        qCWarning(logDb) << "cannot open SDE DB: already opened!";
        return false;
    }
    m_eve_sde_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), QLatin1String("evesde"));
    if (!m_eve_sde_db.isValid()) {
        qCWarning(logDb) << "cannot open SDE DB: failed to load sqlite driver?!";
        return false;
    }
    m_eve_sde_db.setDatabaseName(db_filename);
    if (!m_eve_sde_db.open()) {
        qCWarning(logDb) << "cannot open SDE DB: failed to open file:" << db_filename;
        return false;
    }
    qCDebug(logDb) << "Opened SDE database:" << db_filename;

    // check for missing tables
    QStringList existing_tables;
    QSqlQuery q(m_eve_sde_db);
    if (q.exec(QLatin1String("SELECT name FROM sqlite_master WHERE type='table'"))) {
        while(q.next()) {
            existing_tables << q.value(0).toString();
        }
    }
    q.clear();

    bool ok = true;
    QStringList needed_tables({
        QStringLiteral("invTypes"),
        QStringLiteral("invGroups"),
        QStringLiteral("invCategories"),
        QStringLiteral("chrRaces"),
        QStringLiteral("chrFactions"),
        QStringLiteral("chrBloodlines"),
        QStringLiteral("chrAncestries")
    });
    for (const QString& needed_table: needed_tables) {
        if (!existing_tables.contains(needed_table)) {
            qCDebug(logDb) << "  SDE: Importing table: " << needed_table << "...";
            QString sql_file = QString(QLatin1String(":/sql/%1.sql")).arg(needed_table);
            ok &= this->execSqlFile(&m_eve_sde_db, sql_file);
        }
    }
    return ok;
}


void DbSqlite::close()
{
    if (m_chars_db.isOpen()) {
        m_chars_db.close();
        qCDebug(logDb) << "Closed chars database.";
    }
    if (m_eve_sde_db.isOpen()) {
        m_eve_sde_db.close();
        qCDebug(logDb) << "Closed SDE database.";
    }
}


bool DbSqlite::execSqlFile(QSqlDatabase *db, const QString& filename)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(logDb) << "Failed to open imput SQL file for reading:" << filename;
        return false;
    }
    QSqlDriver *driver = db->driver();
    QSqlQuery q(*db);
    QTextStream ts(&f);
    QString line;
    int nLines = 0;
    int nErrors = 0;
    if (driver->hasFeature(QSqlDriver::Transactions)) {
        bool ok = db->transaction();
        if (!ok) {
            qCWarning(logDb) << "    DB transaction failed to start!";
        }
    }
    while (ts.readLineInto(&line)) {
        nLines++;
        if (line.startsWith(QLatin1String("--"))) {
            // skip SQL comments
            continue;
        }
        if (!line.endsWith(QLatin1String(");"))) {
            QString line2;
            while (!line.endsWith(QLatin1String(");"))) {
                if (!ts.readLineInto(&line2)) break;
                nLines++;
                line.append(line2);
            }
        }
        if (!q.exec(line)) {
            qCWarning(logDb) << filename << " line " << nLines << "; SQL Error:"
                             << q.lastError().text();
            nErrors++;
        }
        if (nLines % 1000 == 0) {
            qCDebug(logDb) << "    " << nLines << " lines...";
        }
    }
    f.close();
    if (driver->hasFeature(QSqlDriver::Transactions)) {
        bool ok = db->commit();
        if (!ok) {
            qCWarning(logDb) << "    DB transaction failed to commit!";
        }
    }
    qCDebug(logDb) << filename << "executed, " << nErrors << "errors, "
                   << nLines << "lines total.";
    return (nErrors == 0);
}


bool DbSqlite::loadCharacters(QList<Character *>& charList)
{
    if (!m_chars_db.isOpen()) return false;
    QSqlQuery q(m_chars_db);
    if (!q.exec(QLatin1String("SELECT char_id, char_data FROM characters"))) {
        return false;
    }
    while (q.next()) {
        quint64 char_id = q.value(0).toULongLong();
        QByteArray char_data = q.value(1).toByteArray();
        Character *character = new Character();
        QDataStream stream(&char_data, QIODevice::ReadOnly);
        stream >> (*character);
        // check that character was loaded properly
        // this may fail if savedata format changed
        if (character->characterId() == char_id) {
            charList.append(character);
        } else {
            qCDebug(logDb) << "Character id" << char_id
                           << " was not correctly loaded from dataabse; did save format change?";
        }
    }
    return true;
}


bool DbSqlite::saveCharacters(const QList<Character *>& charList)
{
    if (!m_chars_db.isOpen()) return false;
    QSqlQuery q(m_chars_db);
    q.exec(QLatin1String("DELETE FROM characters"));
    q.clear();
    for (const Character *character: charList) {
        QByteArray char_data;
        QDataStream stream(&char_data, QIODevice::WriteOnly);
        stream << (*character);
        q.prepare(QLatin1String("INSERT OR REPLACE INTO characters(char_id, char_data) VALUES (?, ?)"));
        q.addBindValue(character->characterId(), QSql::In);
        q.addBindValue(char_data, QSql::In | QSql::Binary);
        q.exec();
        q.clear();
    }
    return true;
}


bool DbSqlite::saveCharacter(const Character *character)
{
    if (!character) return false;
    if (!m_chars_db.isOpen()) return false;
    // serialize character to BLOB
    QByteArray char_data;
    QDataStream stream(&char_data, QIODevice::WriteOnly);
    stream << (*character);
    // save into DB
    QSqlQuery q(m_chars_db);
    q.prepare(QLatin1String("INSERT OR REPLACE INTO characters(char_id, char_data) VALUES (?, ?)"));
    q.addBindValue(character->characterId(), QSql::In);
    q.addBindValue(char_data, QSql::In | QSql::Binary);
    return q.exec();
}


bool DbSqlite::loadPortrait(quint64 char_id, QImage& img)
{
    if (!m_chars_db.isOpen()) {
        return false;
    }
    QSqlQuery q(m_chars_db);
    q.prepare(QLatin1String("SELECT char_id, picture FROM portraits WHERE char_id=?"));
    q.addBindValue(char_id, QSql::In);
    if (q.exec()) {
        if (q.next()) {
            QByteArray data = q.value(1).toByteArray();
            QBuffer buf(&data);
            buf.open(QIODevice::ReadOnly);
            bool ret = img.load(&buf, "JPG");
            return ret;
        }
    }
    return false;
}


bool DbSqlite::savePortrait(quint64 char_id, const QImage& img)
{
    if (!m_chars_db.isOpen() || img.isNull()) {
        return false;
    }
    QSqlQuery q(m_chars_db);
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    if (img.save(&buf, "JPG")) {
        q.prepare(QLatin1String("INSERT OR REPLACE INTO portraits(char_id, picture) VALUES (?, ?)"));
        q.addBindValue(char_id, QSql::In);
        q.addBindValue(buf.buffer(), QSql::In | QSql::Binary);
        return q.exec();
    }
    return false;
}


bool DbSqlite::deletePortrait(quint64 char_id)
{
    if (!m_chars_db.isOpen()) {
        return false;
    }
    QSqlQuery q(m_chars_db);
    q.prepare(QLatin1String("DELETE FROM portraits WHERE char_id = ?"));
    q.addBindValue(char_id, QSql::In);
    return q.exec();
}


QString DbSqlite::raceName(quint64 race_id)
{
    QString ret;
    if (!m_eve_sde_db.isOpen()) {
        return ret;
    }
    QSqlQuery q(m_eve_sde_db);
    q.prepare(QLatin1String("SELECT raceName FROM chrRaces WHERE raceID = ?"));
    q.addBindValue(race_id, QSql::In);
    if (q.exec()) {
        if (q.next()) {
            ret = q.value(0).toString();
        }
    }
    return ret;
}


QString DbSqlite::bloodlineName(quint64 bloodline_id)
{
    QString ret;
    if (!m_eve_sde_db.isOpen()) {
        return ret;
    }
    QSqlQuery q(m_eve_sde_db);
    q.prepare(QLatin1String("SELECT bloodlineName FROM chrBloodlines WHERE bloodlineID = ?"));
    q.addBindValue(bloodline_id, QSql::In);
    if (q.exec()) {
        if (q.next()) {
            ret = q.value(0).toString();
        }
    }
    return ret;
}


QString DbSqlite::ancestryName(quint64 ancestry_id)
{
    QString ret;
    if (!m_eve_sde_db.isOpen()) {
        return ret;
    }
    QSqlQuery q(m_eve_sde_db);
    q.prepare(QLatin1String("SELECT ancestryName FROM chrAncestries WHERE ancestryID = ?"));
    q.addBindValue(ancestry_id, QSql::In);
    if (q.exec()) {
        if (q.next()) {
            ret = q.value(0).toString();
        }
    }
    return ret;
}


QString DbSqlite::factionName(quint64 faction_id)
{
    QString ret;
    if (!m_eve_sde_db.isOpen()) {
        return ret;
    }
    QSqlQuery q(m_eve_sde_db);
    q.prepare(QLatin1String("SELECT factionName FROM chrFactions WHERE factionID = ?"));
    q.addBindValue(faction_id, QSql::In);
    if (q.exec()) {
        if (q.next()) {
            ret = q.value(0).toString();
        }
    }
    return ret;
}


QString DbSqlite::typeName(quint64 type_id)
{
    QString ret;
    if (!m_eve_sde_db.isOpen()) {
        return ret;
    }
    QSqlQuery q(m_eve_sde_db);
    q.prepare(QLatin1String("SELECT typeName FROM invTypes WHERE typeID = ?"));
    q.addBindValue(type_id, QSql::In);
    if (q.exec()) {
        if (q.next()) {
            ret = q.value(0).toString();
        }
    }
    return ret;
}


QJsonArray DbSqlite::loadSkillGroups()
{
    QJsonArray ret;
    if (!m_eve_sde_db.isOpen()) {
        return ret;
    }

    // 1. find "Skills" category
    quint64 skills_category_id = 0;
    QSqlQuery q(m_eve_sde_db);
    if (q.exec(QLatin1String("SELECT categoryID FROM invCategories WHERE categoryName = 'Skill'"))) {
        if (q.next()) {
            skills_category_id = q.value(0).toULongLong();
        }
    }
    if (skills_category_id == 0) {
        qCWarning(logDb) << "DbSqlite::loadSkillGroups(): failed to find 'Skills' category!";
        return ret;
    }
    q.clear();

    // 2. find all inv groups in category 'Skills'
    q.prepare(QLatin1String("SELECT groupID, groupName FROM invGroups WHERE categoryID = ?"));
    q.addBindValue(skills_category_id, QSql::In);
    if (q.exec()) {
        while(q.next()) {
            QJsonObject obj;
            obj.insert(QLatin1String("id"), q.value(0).toString());
            obj.insert(QLatin1String("name"), q.value(1).toString());
            ret.append(QJsonValue(obj));
        }
    }

    return ret;
}


QJsonArray DbSqlite::loadSkillsInGroup(quint64 group_id)
{
    QJsonArray ret;
    if (!m_eve_sde_db.isOpen()) {
        return ret;
    }

    QSqlQuery q(m_eve_sde_db);
    // find all inv types in group
    q.prepare(QLatin1String("SELECT typeID, typeName FROM invTypes WHERE groupID = ?"));
    q.addBindValue(group_id, QSql::In);
    if (q.exec()) {
        while(q.next()) {
            QJsonObject obj;
            obj.insert(QLatin1String("id"), q.value(0).toString());
            obj.insert(QLatin1String("name"), q.value(1).toString());
            ret.append(QJsonValue(obj));
        }
    }

    return ret;
}

} // namespace
