import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"


Item {
    id: container
    height: iconWidth
    width: iconWidth + vtext.width

    property int attributeValue: 10
    property int iconWidth: 32
    property string iconSourceUrl: "../../img/char_attributes/intelligence.png"
    property string tooltipText: "Attribute"
    property alias font: vtext.font
    property alias textColor: vtext.color

    Image {
        id: icon
        source: iconSourceUrl
        width: iconWidth
        height: iconWidth
        MouseArea {
            id: iconMa
            anchors.fill: parent
            hoverEnabled: true
        }
        ToolTip {
            visible: iconMa.containsMouse
            text: tooltipText
            font.pointSize: AppStyle.textSizeH3
        }
    }
    Text {
        id: vtext
        anchors.left: icon.right
        anchors.verticalCenter: icon.verticalCenter
        text: attributeValue + " " + qsTr("points")
        color: AppStyle.textLightColor
    }
}
