import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"

Item {
    id: container
    height: keyTextItem.height + AppStyle.marginSmall
    width: keyTextItem.width + valueTextItem.width + AppStyle.marginSmall + AppStyle.marginNormal

    property string keyText: "Key text"
    property string valueText: "Value text"
    property color keyTextColor: AppStyle.textLightColor
    property color valueTextColor: AppStyle.textDefaultColor
    property int fontPointSize: AppStyle.textSizeH3
    property string fontFamily: AppStyle.fontFamily

    Text {
        id: keyTextItem
        anchors.left: container.left
        anchors.top: container.top
        anchors.leftMargin: AppStyle.marginNormal
        anchors.topMargin: AppStyle.marginSmall
        text: keyText
        font.family: fontFamily
        font.pointSize: fontPointSize
        color: keyTextColor
    }
    Text {
        id: valueTextItem
        anchors.left: keyTextItem.right
        anchors.baseline: keyTextItem.baseline
        anchors.leftMargin: AppStyle.marginSmall
        anchors.topMargin: AppStyle.marginSmall
        text: valueText
        font.family: fontFamily
        font.pointSize: fontPointSize
        color: valueTextColor
    }
}
