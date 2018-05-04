import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"

Rectangle {
    // border { width: 1; color: "green" }
    implicitHeight: labelsRow.height

    property int labelId: 0
    property string labelName
    property int unreadCount: 0
    property color labelColor: "white"

    property bool fontBold: false
    property int fontPointSize: AppStyle.textSizeH3

    Row {
        id: labelsRow
        height: lblLabelName.height + AppStyle.marginSmall*2
        Label {
            id: lblLabelName
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: fontPointSize
            font.bold: fontBold
            text: labelName + " "
        }
        Label {
            anchors.verticalCenter: parent.verticalCenter
            text: (unreadCount > 0) ? ("(" + unreadCount + ")") : ""
            font.pointSize: fontPointSize
            font.bold: fontBold
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
}
