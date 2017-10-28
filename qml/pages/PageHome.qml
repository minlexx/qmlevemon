import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../"

Rectangle {
    id: content
    //anchors.fill: parent // not needed in a StackView
    anchors.margins: AppStyle.marginNormal

    border.color: "red"
    border.width: 0

    signal addCharacterRequest()
    signal selectCharacterRequest(int characterId)

    Button {
        id: btnAddAccount
        text: qsTr("Add character")
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: AppStyle.marginNormal
        }

        onClicked: {
            content.addCharacterRequest();
        }
    }

    ListModel {
        id: testCharactersModel
        ListElement {
            // profilePicture: "../../tests/92181726_128.jpg"
            characterId: 1365367652
            characterName: "Lexx Min"
            isk: "43 052 383,23"
            trainingSkill: "Minmatar Cruiser V"
            trainingSkillTimeLeft: "21d 16h 6m 12s"
            trainingSkillEndDateTime: "Sun 15.10.2017 3:33:42"
            queueTimeLeft: "47d 16h 6m 12s"
        }
        ListElement {
            // profilePicture: "../../tests/94016386_128.jpg"
            characterId: 5478468754
            characterName: "Alya Kastra"
            isk: "590 137 154,35"
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
                    text: model.isk
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
                hoverEnabled: true

                onClicked: {
                    content.selectCharacterRequest(model.characterId)
                }
            }
        }
    }
}
