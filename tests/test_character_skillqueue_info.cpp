#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QtTest>

#include "models/character_skillqueue_info.h"


class TestCharacterSkillQueueInfo: public QObject
{
    Q_OBJECT

private:
    QDateTime dtCur;
    EM::CharacterSkillQueueInfo qi1;

private Q_SLOTS:

    void initTestCase() {
        dtCur = QDateTime::currentDateTime();
        qi1.levelStartSp = 10;
        qi1.levelEndSp = 100;
        qi1.startDate = dtCur;
    }

    void cleanupTestCase() { }

    void test_copyConstruct() {
        EM::CharacterSkillQueueInfo qi2(qi1);

        QVERIFY(qi2.levelStartSp == qi1.levelStartSp);
        QVERIFY(qi2.levelEndSp == qi1.levelEndSp);
        QVERIFY(qi2.startDate == qi1.startDate);
    }

    void test_assign() {
        EM::CharacterSkillQueueInfo qi2;
        qi2 = qi1;

        QVERIFY(qi2.levelStartSp == qi1.levelStartSp);
        QVERIFY(qi2.levelEndSp == qi1.levelEndSp);
        QVERIFY(qi2.startDate == qi1.startDate);
    }
};


QTEST_GUILESS_MAIN(TestCharacterSkillQueueInfo)

#include "test_character_skillqueue_info.moc"

