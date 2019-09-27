import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../"

Rectangle {
    id: container
    //anchors.fill: parent // not needed in a StackView
    anchors.margins: AppStyle.marginNormal
    clip: true

    border.color: "blue"
    border.width: 1

    Rectangle {
        id: aboutRect
        anchors.fill: parent
        anchors.margins: AppStyle.marginNormal

        height: aboutText.contentHeight + 2*AppStyle.marginNormal

        radius: AppStyle.marginNormal
        border.width: 2
        border.color: AppStyle.infoPopupBorderColor
        color: AppStyle.infoPopupBgColor

        Text {
            id: aboutText
            anchors.fill: parent
            anchors.margins: AppStyle.marginNormal
            font.family: AppStyle.fontFamily
            font.pointSize: AppStyle.textSizeH3
            color: AppStyle.infoPopupTextColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            clip: true
            text: qsTr("QML EVEMon version: ") + Qt.application.version
                  + (evemonapp.isFlatpakPlatform ? " (Flatpak)" : "") + "\n"
                  + qsTr("Qt version: ") + qVersion + qsTr(" (compiled with ") + QT_VERSION_STR + ")"
        }
    }
}
