import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import "../"
import "../custom_controls"

Rectangle {
    id: container
    anchors.margins: AppStyle.marginNormal
    clip: true

    border.color: "blue"
    border.width: 0

    signal selectCharacterRequest(int characterId)

    CharacterPickerSidebar {
        id: charPickSidebar
        anchors.top: parent.top
        anchors.left: parent.left
        width: 48

        onCharacterSelected: {
            container.selectCharacterRequest(char_id);
        }
    }

    Image {
        id: profilePic
        anchors.left: charPickSidebar.right
        anchors.leftMargin: AppStyle.marginNormal
        anchors.top: parent.top
        anchors.topMargin: AppStyle.marginNormal
        source: "image://portrait/" + curChar.characterId
        //source: "../../tests/92181726_128.jpg"
        sourceSize.width: 128
        sourceSize.height: 128

        MouseArea {
            id: profilePicMA
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onClicked: {
                if (mouse.button === Qt.LeftButton || mouse.button === Qt.RightButton) {
                    contextMenu.popup(mouse.x, mouse.y);
                }
            }

            onPressAndHold: {
                contextMenu.popup(mouse.x, mouse.y);
            }

            Menu {
                id: contextMenu

                // Menu.popup() function was introduced in Qt 5.10,
                // for 5.7 compatibility we can emulate it somehow
                function popup(x, y) {
                    this.x = x;
                    this.y = y;
                    this.open();
                }

                EMPopupMenuItem {
                    text: qsTr("Update portrait from web")
                    onTriggered: {
                        console.log("Update portrait for: " + curChar.characterId);
                        evemonapp.requestRefreshCharacterPortrait(curChar.characterId);
                        // change image source to force update:
                        // change to "default" image
                        profilePic.source = "qrc:///img/character_icon_128.jpg";
                        // change again to new URL with timestamp, to force reload
                        var curTimeStamp = new Date().getTime();
                        profilePic.source = "image://portrait/" + curChar.characterId + "/tm" + curTimeStamp;
                    }
                }
            }
        }
    }

    Text {
        id: charNameText
        anchors.left: profilePic.right
        anchors.top: parent.top
        anchors.leftMargin: AppStyle.marginNormal
        anchors.topMargin: AppStyle.marginNormal
        text: curChar.characterName
        //text: "Test Char"
        font.family: AppStyle.fontFamily
        font.pointSize: AppStyle.textSizeH2
        font.bold: true
    }
    Text {
        id: charOriginsText
        anchors.left: profilePic.right
        anchors.top: charNameText.bottom
        anchors.leftMargin: AppStyle.marginNormal
        anchors.topMargin: AppStyle.marginSmall
        text: (curChar.gender ? qsTr("Female") : qsTr("Male")) + ", "
              + curChar.raceName + " - "
              + curChar.bloodlineName + " - "
              + curChar.ancestryName
        //text: "Male, Caldari - Achura - Inventors"
        font.family: AppStyle.fontFamily
        font.pointSize: AppStyle.textSizeH3
        color: AppStyle.textLightColor
    }

    TextKeyValue {
        id: charBalanceText
        anchors.left: profilePic.right
        anchors.top: charOriginsText.bottom
        keyText: qsTr("Balance:")
        //valueText: "43 234 123,03 ISK"
        valueText: curChar.iskAmountStr
    }

    TextKeyValue {
        id: charBirthdayText
        anchors.left: profilePic.right
        anchors.top: charBalanceText.bottom
        keyText: qsTr("Birthday:")
        //valueText: "18.10.2015 12:20:41"
        valueText: curChar.birthday
    }

    TextKeyValue {
        id: charCorpText
        anchors.left: profilePic.right
        anchors.top: charBirthdayText.bottom
        keyText: qsTr("Corporation:")
        //valueText: "Through The Event Horizon [T-TEH]"
        valueText: curChar.corporationName + " [" + curChar.corporationTicker + "]"
    }

    TextKeyValue {
        id: charAllyText
        anchors.left: profilePic.right
        anchors.top: charCorpText.bottom
        keyText: qsTr("Alliance:")
        //valueText: "Red Alliance <RED>"
        valueText: (curChar.allianceId > 0 ? curChar.allianceName +
                   " <" + curChar.allianceTicker + ">" : "-")
    }

    TextKeyValue {
        id: charSecStatusText
        anchors.left: profilePic.right
        anchors.top: charAllyText.bottom
        keyText: qsTr("Security Status:")
        //valueText: "-1.2"
        valueText: curChar.securityStatusStr
    }

    TextKeyValue {
        id: charActiveShipText
        anchors.left: profilePic.right
        anchors.top: charSecStatusText.bottom
        keyText: qsTr("Active Ship:")
        //valueText: "Proteus [Scanner Probe]"
        valueText: curChar.currentShipTypeName + " [" + curChar.currentShipFriendlyName + "]"
    }

    TextKeyValue {
        id: charLocationText
        anchors.left: profilePic.right
        anchors.top: charActiveShipText.bottom
        keyText: qsTr("Location:")
        //valueText: "The Forge > Kimotoro > Jita (0.9)"
        valueText: curChar.currentRegionName + " > " +
                   curChar.currentConstellationName + " > " +
                   curChar.currentSolarSystemName + " (" +
                   curChar.currentSolarSystemSecurityStr + ")"
    }

    TextKeyValue {
        id: charDockedInText
        anchors.left: profilePic.right
        anchors.top: charLocationText.bottom
        // keyText: qsTr("Docked in:")
        keyText: curChar.isDocked ? qsTr("Docked in: ") : qsTr("Not docked")
        //valueText: "Jita IV - Moon IV - Caldari Navy Assembly Plant"
        valueText: curChar.isDocked ? curChar.currentStructureName : ""
    }

    Image {
        id: imgRefresh
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 2
        sourceSize.width: 48
        sourceSize.height: 48
        source: "qrc:///img/refresh_48.png"

        MouseArea {
            id: maRefresh
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onClicked: {
                if (mouse.button === Qt.LeftButton) {
                    refreshMenu.popup(mouse.x, mouse.y);
                }
            }

            onPressAndHold: {
                refreshMenu.popup(mouse.x, mouse.y);
            }
        }

        Menu {
            id: refreshMenu
            y: imgRefresh.y

            EMPopupMenuItem {
                text: qsTr("Refresh public information");
                y: parent.y + 1
                onTriggered: { curChar.forceRefreshPublicInfo(); evemonapp.forceRefresh(); }
            }
            EMPopupMenuItem {
                text: qsTr("Refresh location");
                onTriggered: { curChar.forceRefreshLocation(); evemonapp.forceRefresh(); }
            }
            EMPopupMenuItem {
                text: qsTr("Refresh clones");
                onTriggered: { curChar.forceRefreshClones(); evemonapp.forceRefresh(); }
            }
            EMPopupMenuItem {
                text: qsTr("Refresh skills");
                onTriggered: { curChar.forceRefreshSkills(); evemonapp.forceRefresh(); }
            }
            EMPopupMenuItem {
                text: qsTr("Refresh wallet");
                onTriggered: { curChar.forceRefreshWallet(); evemonapp.forceRefresh(); }
            }
            EMPopupMenuItem {
                text: qsTr("Refresh assets");
                onTriggered: { curChar.forceRefreshAssets(); evemonapp.forceRefresh(); }
            }

            function popup(x, y) {
                this.x = x;
                this.y = y;
                this.open();
            }
        }
    }

    Glow {
        anchors.fill: imgRefresh
        source: imgRefresh
        radius: 8
        samples: 17
        spread: 0.5
        color: AppStyle.rectBgHighlightColor
        cached: true
        visible: maRefresh.containsMouse
    }

    /*TabBar {
        anchors.top: charDockedInText.bottom
        width: parent.width
        anchors.topMargin: AppStyle.marginNormal
        TabButton { text: qsTr("Tab 1"); }
        TabButton { text: qsTr("Tab 2"); }
        TabButton { text: qsTr("Tab 3"); }
    }*/
}
