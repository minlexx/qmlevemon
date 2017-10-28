#include <QCoreApplication>
#include <QObject>
#include <QtTest>
#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>

#include "eve_api/eve_api_tokens.h"
#include "models/character.h"


class CharacterTest: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    // will be called before the first test function is executed.
    void initTestCase() {
        // create an example character
        ch = new EM::Character();
        ch->setCharacterId(123);
        ch->setCharacterName("Lexx Min");
        ch->setIskAmount(123.45f);
        EM::EveOAuthTokens token;
        token.access_token = "access_token";
        token.refresh_token = "refresh_token";
        token.expire_dt = QDateTime(QDate(2017, 10, 12), QTime(1, 14, 00), Qt::LocalTime);
        ch->setAuthTokens(token);

        // save object to stream
        QDataStream stream(&ba, QIODevice::WriteOnly);
        stream << *ch;
    }

    // will be called after the last test function was executed.
    void cleanupTestCase() {
        delete ch;
    }

    void test_QDataStream() {
        QDataStream stream(&ba, QIODevice::ReadOnly);
        // read character from stream
        EM::Character ch2;
        stream >> ch2;

        qDebug() << ch2.toString();

        QVERIFY(ch->characterId() == ch2.characterId());
        QVERIFY(ch->characterName() == ch2.characterName());
        QVERIFY(ch->iskAmount() == ch2.iskAmount());
        // check auth tokens
        QVERIFY(ch->getAuthTokens() == ch2.getAuthTokens());
    }

protected:
    EM::Character *ch;
    QByteArray ba;
};


QTEST_GUILESS_MAIN(CharacterTest)

#include "test_character_datastream.moc"
