import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"

MenuItem {
    property color highlightColor: AppStyle.rectBgHighlightColor
    property color bgColor: AppStyle.bgColor

    text: "Menu item";
    hoverEnabled: true

    background: Rectangle {
        //x: parent.x + 1
        anchors.margins: 1
        anchors.left: parent.left
        anchors.right: parent.right
        //width: parent.width - 2
        implicitHeight: 30
        color: parent.hovered ? highlightColor : bgColor

        //border.color: "black" // Qt.rgba(0, 0, 0, 0)
        //border.width: 2
    }
}
