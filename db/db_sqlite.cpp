#include <QGlobalStatic>
#include <QSqlError>
#include <QSqlQuery>
#include <QDataStream>
#include <QBuffer>
#include <QLoggingCategory>
#include <QDebug>

#include "db_sqlite.h"
#include "models/character.h"
#include "qmlevemon_app.h"


Q_GLOBAL_STATIC(EM::DbSqlite, g_em_db_sqlite)
Q_LOGGING_CATEGORY(logDb, "evemon.db")


EM::DbSqlite::DbSqlite()
{
    QString appdata_dirname = globalAppInstance()->storageDirectory();
    QString db_filename = appdata_dirname + QLatin1String("/characters.db");
    this->open(db_filename);
}


EM::DbSqlite::~DbSqlite()
{
    close();
}


EM::Db* EM::DbSqlite::instance()
{
    return g_em_db_sqlite();
}


bool EM::DbSqlite::open(const QString& db_filename)
{
    if (m_chars_db.isOpen()) {
        qCWarning(logDb) << "cannot open DB: already opened!";
        return false;
    }
    m_chars_db = QSqlDatabase::addDatabase("QSQLITE", "characters");
    if (!m_chars_db.isValid()) {
        qCWarning(logDb) << "cannot open DB: failed to load sqlite driver?!";
        return false;
    }
    m_chars_db.setDatabaseName(db_filename);
    if (!m_chars_db.open()) {
        qCWarning(logDb) << "cannot open DB: failed to open file:" << db_filename;
        return false;
    }
    qCDebug(logDb) << "Opened database:" << db_filename;

    // check for missing tables
    QStringList existing_tables;
    QSqlQuery q(m_chars_db);
    if (q.exec("SELECT name FROM sqlite_master WHERE type='table'")) {
        while(q.next()) {
            existing_tables << q.value(0).toString();
        }
    }
    q.clear();

    if (!existing_tables.contains("characters")) {
        qCDebug(logDb) << "Creating table characters...";
        q.exec("CREATE TABLE characters("
               "    char_id INTEGER PRIMARY KEY NOT NULL,"
               "    char_data BLOB)");
        q.clear();
    }

    if (!existing_tables.contains("portraits")) {
        qCDebug(logDb) << "Creating table portraits...";
        q.exec("CREATE TABLE portraits("
               "    char_id INTEGER PRIMARY KEY NOT NULL,"
               "    picture BLOB)");
        q.clear();
    }

    return true;
}


void EM::DbSqlite::close()
{
    if (!m_chars_db.isOpen()) return;
    m_chars_db.close();
    qCDebug(logDb) << "Closed database.";
}


bool EM::DbSqlite::loadCharacters(QList<Character *>& charList)
{
    if (!m_chars_db.isOpen()) return false;
    QSqlQuery q(m_chars_db);
    if (!q.exec("SELECT char_id, char_data FROM characters")) {
        return false;
    }
    while (q.next()) {
        quint64 char_id = q.value(0).toULongLong();
        QByteArray char_data = q.value(1).toByteArray();
        EM::Character *character = new EM::Character();
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


bool EM::DbSqlite::saveCharacters(const QList<Character *>& charList)
{
    if (!m_chars_db.isOpen()) return false;
    QSqlQuery q(m_chars_db);
    q.exec("DELETE FROM characters");
    q.clear();
    for (const EM::Character *character: charList) {
        QByteArray char_data;
        QDataStream stream(&char_data, QIODevice::WriteOnly);
        stream << (*character);
        q.prepare("INSERT OR REPLACE INTO characters(char_id, char_data) VALUES (?, ?)");
        q.addBindValue(character->characterId(), QSql::In);
        q.addBindValue(char_data, QSql::In | QSql::Binary);
        q.exec();
        q.clear();
    }
    return true;
}


bool EM::DbSqlite::saveCharacter(const Character *character)
{
    if (!character) return false;
    if (!m_chars_db.isOpen()) return false;
    // serialize character to BLOB
    QByteArray char_data;
    QDataStream stream(&char_data, QIODevice::WriteOnly);
    stream << (*character);
    // save into DB
    QSqlQuery q(m_chars_db);
    q.prepare("INSERT OR REPLACE INTO characters(char_id, char_data) VALUES (?, ?)");
    q.addBindValue(character->characterId(), QSql::In);
    q.addBindValue(char_data, QSql::In | QSql::Binary);
    return q.exec();
}


bool EM::DbSqlite::loadPortrait(quint64 char_id, QImage& img)
{
    if (!m_chars_db.isOpen()) {
        return false;
    }
    QSqlQuery q(m_chars_db);
    q.prepare("SELECT char_id, picture FROM portraits WHERE char_id=?");
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


bool EM::DbSqlite::savePortrait(quint64 char_id, const QImage& img)
{
    if (!m_chars_db.isOpen() || img.isNull()) {
        return false;
    }
    QSqlQuery q(m_chars_db);
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    if (img.save(&buf, "JPG")) {
        q.prepare("INSERT OR REPLACE INTO portraits(char_id, picture) VALUES (?, ?)");
        q.addBindValue(char_id, QSql::In);
        q.addBindValue(buf.buffer(), QSql::In | QSql::Binary);
        return q.exec();
    }
    return false;
}
