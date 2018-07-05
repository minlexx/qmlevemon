#include <QObject>
#include <QVector>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QtTest>
#include <QDebug>
#include <QLoggingCategory>

#include "eve_api/eve_api.h"

Q_LOGGING_CATEGORY(logTestApi, "test.eveapi")

class TestEveApi: public QObject
{
    Q_OBJECT

private:
    QDateTime dtCur;
    EM::EveApi m_api;
    QVector<quint64> ids_vector;

private Q_SLOTS:

    void initTestCase() {
        ids_vector << 91205062; // Lexx Min
        ids_vector << 95367553; // Alya Kastra
    }

    void cleanupTestCase() { }

    void test_characters_names() {
        QJsonArray replyArr;
        qCDebug(logTestApi) << "calling get_characters_names()";
        bool api_ret = m_api.get_characters_names(replyArr, ids_vector);
        qCDebug(logTestApi) << " get_characters_names() = " << replyArr;
        QVERIFY2(!api_ret, "Api call get_characters_names() should fail");
    }

    void test_universe_names() {
        QJsonArray replyArr;
        qCDebug(logTestApi) << "calling post_universe_names()";
        bool ret = m_api.post_universe_names(replyArr, ids_vector);
        qCDebug(logTestApi) << " post_universe_names() = " << replyArr;
        QVERIFY2(ret, "Api call post_universe_names() should be OK");
    }
};

QTEST_GUILESS_MAIN(TestEveApi)

#include "test_eveapi.moc"
