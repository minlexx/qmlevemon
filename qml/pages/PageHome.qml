import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../"
import "../custom_controls"

Rectangle {
    id: container
    //anchors.fill: parent // not needed in a StackView
    anchors.margins: AppStyle.marginNormal

    border.color: "red"
    border.width: 0

    signal addCharacterRequest()
    signal selectCharacterRequest(int characterId)
    signal removeCharacterRequest(int characterId)

    Button {
        id: btnAddAccount
        text: qsTr("Add character")
        font.pointSize: AppStyle.textSizeH3
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: AppStyle.marginNormal
        }

        onClicked: {
            container.addCharacterRequest();
        }
    }

    ListModel {
        id: testCharactersModel
        ListElement {
            characterId: 1365367652
            characterName: "Lexx Min"
            iskAmountStr: "43 052 383,23 ISK"
            trainingSkill: "Minmatar Cruiser V"
            trainingSkillTimeLeft: "21d 16h 6m 12s"
            trainingSkillEndDateTime: "Sun 15.10.2017 3:33:42"
            queueTimeLeft: "47d 16h 6m 12s"
        }
        ListElement {
            characterId: 5478468754
            characterName: "Alya Kastra"
            iskAmountStr: "590 137 154,35 ISK"
            trainingSkill: "Drone Interfacing V"
            trainingSkillTimeLeft: "12d 1h 10m 30s"
            trainingSkillEndDateTime: "Sun 15.10.2017 3:33:42"
            queueTimeLeft: "47d 16h 6m 12s"
        }
    }

    GridView {
        id: gridView
        clip: true
        anchors.top: btnAddAccount.bottom
        anchors.margins: AppStyle.marginNormal
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        cellWidth: 430
        cellHeight: 180

        //model: testCharactersModel
        model: characterModel

        delegate: Rectangle {
            width: gridView.cellWidth
            height: gridView.cellHeight

            border.color: AppStyle.rectHighlightColor
            border.width: (mouseArea.containsMouse ? 2 : 0)
            color: (mouseArea.containsMouse ? AppStyle.rectBgHighlightColor: AppStyle.bgColor)

            Image {
                id: profilePic
                anchors.left: parent.left
                anchors.leftMargin: AppStyle.marginNormal
                anchors.top: parent.top
                anchors.topMargin: AppStyle.marginNormal
                source: "image://portrait/" + model.characterId
            }
            Column {
                anchors.left: profilePic.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: AppStyle.marginNormal
                anchors.topMargin: AppStyle.marginNormal
                spacing: 5
                Text {
                    text: model.characterName
                    color: AppStyle.textDefaultColor
                    font.pointSize: AppStyle.textSizeH2
                    font.family: AppStyle.fontFamily
                    font.bold: true
                }
                Text {
                    text: model.iskAmountStr
                    color: AppStyle.textSecondaryColor
                    font.pointSize: AppStyle.textSizeH3
                    font.family: AppStyle.fontFamily
                }
                Text {
                    text: Number(model.totalSp).toLocaleString(Qt.locale(), 'f', 0) + " SP"
                    color: AppStyle.textSecondaryColor
                    font.pointSize: AppStyle.textSizeH3
                    font.family: AppStyle.fontFamily
                }
                Text {
                    text: model.trainingSkill
                    color: AppStyle.textLightColor
                    font.pointSize: AppStyle.textSizeH3
                    font.family: AppStyle.fontFamily
                }
                Text {
                    text: model.trainingSkillTimeLeft
                    color: AppStyle.textLightColor
                    font.pointSize: AppStyle.textSizeH3
                    font.family: AppStyle.fontFamily
                }
                Text {
                    text: model.trainingSkillEndDateTime
                    color: AppStyle.textLightColor
                    font.pointSize: AppStyle.textSizeH3
                    font.family: AppStyle.fontFamily
                }
                Text {
                    text: qsTr("Queue ends in: ") + model.queueTimeLeft
                    color: AppStyle.textLightColor
                    font.pointSize: AppStyle.textSizeH3
                    font.family: AppStyle.fontFamily
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked: {
                    if (mouse.button === Qt.LeftButton) {
                        container.selectCharacterRequest(model.characterId)
                    } else if (mouse.button === Qt.RightButton) {
                        characterPopupMenu.popup(mouse.x, mouse.y);
                    }
                }

                onPressAndHold: {
                    characterPopupMenu.popup(mouse.x, mouse.y);
                }

                Menu {
                    id: characterPopupMenu
                    width: characterMenuItem1.width + 1

                    // Menu.popup() function was introduced in Qt 5.10,
                    // for 5.7 compatibility we can emulate it somehow
                    function popup(x, y) {
                        this.x = x;
                        this.y = y;
                        this.open();
                    }

                    EMPopupMenuItem {
                        id: characterMenuItem1
                        text: qsTr("Delete this character")
                        enabled: !refresher.networkActivity
                        onTriggered: {
                            removeConfirmPopup.characterId = model.characterId;
                            removeConfirmPopup.open();
                        }
                    }
                }
            }
        }

        Popup {
            id: removeConfirmPopup
            visible: false
            clip: true
            closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
            modal: true

            width: 400
            height: 300
            x: parent.width/2 - width/2
            y: parent.height/2 - height/2

            property real characterId: 0

            background: Rectangle {
                implicitWidth: 400
                implicitHeight: 300
                border.width: 3
                border.color: AppStyle.warningPopupBorderColor
                color: AppStyle.warningPopupBgColor
                radius: AppStyle.marginNormal
            }

            Text {
                anchors.fill: parent
                font.family: AppStyle.fontFamily
                font.pointSize: AppStyle.textSizeH2
                color: AppStyle.warningPopupTextColor
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                text: qsTr("Are you sure you want to delete this character?")
            }

            Item {
                id: buttonContainter

                width: btnOk.width + btnCancel.width + 3*AppStyle.marginNormal
                height: btnOk.height + 2*AppStyle.marginNormal
                x: parent.x + parent.width/2 - width/2
                y: parent.y + parent.height - height - AppStyle.marginNormal

                Row {
                    anchors.fill: parent
                    spacing: AppStyle.marginNormal
                    Button {
                        id: btnOk
                        text: qsTr("OK")
                        font.pointSize: AppStyle.textSizeH2
                        onClicked: {
                            removeConfirmPopup.close();
                            container.removeCharacterRequest(removeConfirmPopup.characterId);
                        }
                    }
                    Button {
                        id: btnCancel
                        text: qsTr("Cancel")
                        font.pointSize: AppStyle.textSizeH2
                        onClicked: {
                            removeConfirmPopup.close();
                        }
                    }
                }
            }
        }
    }
}
