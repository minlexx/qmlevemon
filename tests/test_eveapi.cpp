#include <QObject>
#include <QVector>
#include <QtTest>

#include "eve_api/eve_api.h"

class TestEveApi: public QObject
{
    Q_OBJECT

private:
    QDateTime dtCur;
    EM::EveApi m_api;

private Q_SLOTS:

    void initTestCase() { }

    void cleanupTestCase() { }

    void test_universe_names() {
        //QVERIFY(qi2.startDate == qi1.startDate);
    }
};

QTEST_GUILESS_MAIN(TestEveApi)

#include "test_eveapi.moc"
