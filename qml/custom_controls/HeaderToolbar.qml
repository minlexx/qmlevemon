import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../"

ToolBar {
    id: toolbar

    signal hamburgerClicked(string iconState)
    signal characterSelected(int char_id)

    property alias navTitleText: navTitle.text
    property alias iconState: hamburgerIcon.iconState

    RowLayout {
        anchors.fill: parent

        // main menu icon
        HamburgerIcon {
            id: hamburgerIcon
            color: "white"
            colorArrows: "black"
            iconState: "menu"
            onClicked: hamburgerClicked(hamburgerIcon.iconState)
        }

        Text {
            id: navTitle
            text: qsTr("Overview")
            font.family: AppStyle.fontFamily
            font.pointSize: AppStyle.textSizeH3
        }

        CharacterPickerSidebar {
            id: toolbarCharacterPicker
            isVertical: false
            visible: evemonapp.isPortrait && (evemonapp.curCharId > 0)
            onCharacterSelected: toolbar.characterSelected(char_id);
        }

        // spacer
        Item { Layout.fillWidth: true }

        Text {
            id: serverPlayersOnline
            text: "TQ: " + refresher.serverPlayersOnline + qsTr(" players") + " "
            font.pointSize: AppStyle.textSizeH3
            visible: evemonapp.isDesktopPlatform ? true : (!refresher.networkActivity)
            anchors.rightMargin: AppStyle.marginSmall
        }

        BusyIndicator {
            id: networkActivityIndicator
            implicitWidth: 48
            implicitHeight: 48
            running: refresher.networkActivity
            visible: refresher.networkActivity
        }
    }
}
