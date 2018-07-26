import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"

Rectangle {
    id: mailLabelRectCntainer
    // border { width: 1; color: "green" }
    implicitHeight: labelsRow.height

    property int labelId: 0
    property string labelName
    property int unreadCount: 0
    property color labelColor: "white"

    property bool fontBold: false
    property int fontPointSize: AppStyle.textSizeH3

    property int itemIndex: -1
    property bool isCurrent: false

    property color highlightColor: AppStyle.rectBgHighlightColor
    property color bgColor: AppStyle.bgColor
    property color textColor: AppStyle.textDefaultColor

    color: isCurrent ? highlightColor : bgColor

    signal clicked

    Row {
        id: labelsRow
        height: lblLabelName.height + AppStyle.marginSmall*2
        Label {
            id: lblLabelName
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: fontPointSize
            font.bold: fontBold
            text: labelName
            color: textColor
        }
        Item { height: 1;  width: AppStyle.marginNormal }
        Label {
            anchors.verticalCenter: parent.verticalCenter
            text: (unreadCount > 0) ? ("(" + unreadCount + ")") : ""
            font.pointSize: fontPointSize
            font.bold: fontBold
            color: textColor
        }
        Rectangle {
            width: 15
            height: 15
            anchors.verticalCenter: parent.verticalCenter
            border { width: 1; color: "black" }
            color: labelColor
            visible: !Qt.colorEqual(labelColor, "white")
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            mailLabelRectCntainer.clicked()
        }
        cursorShape: Qt.PointingHandCursor
    }
}
