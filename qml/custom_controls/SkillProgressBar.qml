import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"

Rectangle {
    id: root

    // public interface

    property int skillQueueNum: 1
    property int skillRank: 1
    property int skillLevel: 1
    property string skillName: "Skill name"
    property string skillSpCurrent: "0"
    property string skillSpTotal: "1000"
    property int skillSpPerHour: 2200
    property string skillTrainingTimeLeft: "1d 12h 56s"
    property bool isInProgress: true
    property bool isQueued: false

    property double progressStart: 0.0
    property double progressEnd: 0.5

    property color textColor: AppStyle.textDefaultColor
    property color textQueuedColor: "#6595ea"
    property color backColor: AppStyle.bgColor
    property color backColorAlt: "#d3d3d3"
    property bool useAltBackColor: false
    property string fontFamily: AppStyle.fontFamily
    property int fontSize: AppStyle.textSizeH4

    // private part
    color: useAltBackColor ? backColorAlt : backColor

    Rectangle {
        width: 100
        height: 30
        border { width: 1; color: "black" }
        anchors {
            top: parent.top
            right: parent.right
            margins: 5
        }
    }

    Text {
        id: txtSkillName
        anchors {
            top: parent.top
            left: parent.left
            topMargin: AppStyle.marginSmall
            rightMargin: AppStyle.marginSmall
            bottomMargin: AppStyle.marginSmall
            leftMargin: (skillQueueNum > 0) ? AppStyle.marginSmall : AppStyle.marginBig
        }
        font {
            bold: true
            family: fontFamily
            pointSize: fontSize
        }
        text: (skillQueueNum > 0) ? (skillQueueNum + ". " + skillName) : skillName
        color: isQueued ? textQueuedColor : textColor
    }

    Text {
        id: txtRank
        anchors {
            top: parent.top
            left: txtSkillName.right
            margins: AppStyle.marginSmall
        }
        font {
            bold: false
            pointSize: fontSize
        }
        text: "(" + qsTr("Rank") + " " + skillRank + ")"
        color: isQueued ? textQueuedColor : textColor
    }

    Text {
        id: txtSp
        anchors {
            top: txtSkillName.bottom
            left: txtSkillName.left
            topMargin: AppStyle.marginSmall
            rightMargin: AppStyle.marginSmall
            bottomMargin: AppStyle.marginSmall
            leftMargin: (skillQueueNum > 0) ? AppStyle.marginBig : 0
            //leftMargin: AppStyle.marginBig
        }
        font {
            bold: false
            pointSize: fontSize
        }
        text: "SP: " + skillSpCurrent + " / " + skillSpTotal
        color: isQueued ? textQueuedColor : textColor
    }

    Text {
        id: txtSpPerHour
        anchors {
            top: txtSkillName.bottom
            left: txtSp.right
            topMargin: AppStyle.marginSmall
            rightMargin: AppStyle.marginSmall
            bottomMargin: AppStyle.marginSmall
            leftMargin: AppStyle.marginBig
        }
        font {
            bold: false
            pointSize: fontSize
        }
        text: qsTr("SP/Hour") + ": " + skillSpPerHour
        color: isQueued ? textQueuedColor : textColor
    }

    Text {
        id: txtTrainingTime
        anchors {
            top: txtSkillName.bottom
            left: txtSpPerHour.right
            topMargin: AppStyle.marginSmall
            rightMargin: AppStyle.marginSmall
            bottomMargin: AppStyle.marginSmall
            leftMargin: AppStyle.marginBig
        }
        font {
            bold: false
            pointSize: fontSize
        }
        text: qsTr("Training time") + ": " + skillTrainingTimeLeft
        color: isQueued ? textQueuedColor : textColor
    }
}
