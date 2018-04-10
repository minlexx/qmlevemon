import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import "../../"
import "../../custom_controls"

Rectangle {
    id: container
    border { width: 1; color: AppStyle.mainColor }
    color: AppStyle.bgColor
    clip: true

    ListView {
        id: lvSkillQueue
        anchors.fill: parent
        anchors.margins: 1
        model: curChar.skillQueueModel
        currentIndex: -1
        interactive: true

        delegate: SkillProgressBar {
            id: skillPB
            modeSkillQueue: true
            skillQueueNum: model.positionInQueue + 1 // 0
            skillName: model.skillName
            skillId: model.skillId
            skillRank: model.skillTimeConstant
            skillLevelActive: model.activeLevel
            skillLevelTrained: model.trainedLevel
            skillLevelTraining: model.trainingLevel
            skillSpCurrent: model.skillPointsInSkill
            skillSpTotal: model.skillPointsForNextLevel
            skillSpPerHour: model.skillPointsPerHour
            isInProgress: (model.positionInQueue === 0)
            isQueued: model.isInQueue
            skillTrainingTimeLeft: model.trainingTimeLeft
            skillPrimaryAttribute: model.primaryAttribute
            skillSecondaryAttribute: model.secondaryAttribute
            trainPercent: model.trainPercent

            width: lvSkillQueue.width
            useAltBackColor: (model.positionInQueue % 2 == 1)
        }
    }
}
