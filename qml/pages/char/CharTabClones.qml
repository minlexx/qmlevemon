import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import "../../"
import "../../custom_controls"

Rectangle {
    id: container
    border { width: 1; color: AppStyle.mainColor }
    color: AppStyle.bgColor
    clip: true

    Column {
        id: col1
        anchors.top: container.top
        width: parent.width

        Item {
            width: 1
            height: AppStyle.marginNormal
        }

        Text {
            anchors.left: parent.left
            anchors.leftMargin: AppStyle.marginNormal
            text: qsTr("Active clone:")
            font.bold: true
            font.pointSize: AppStyle.textSizeH2
        }

        TextKeyValue {
            keyText: qsTr("Home location:")
            valueText: curChar.homeLocation.name
        }

        TextKeyValue {
            keyText: qsTr("Last clone jump date:")
            valueText: Qt.formatDateTime(curChar.lastCloneJumpDate)
        }

        TextKeyValue {
            keyText: qsTr("Implants:")
            valueText: ""
        }

        Item {
            width: 1
            height: AppStyle.marginSmall
        }

        Repeater {
            width: col1.width
            model: curChar.currentClone.implantsModel
            delegate: Item {
                implicitHeight: lblCurImpName.height + AppStyle.marginSmall
                implicitWidth: col1.width

                Image {
                    id: implantIcon
                    anchors.left: parent.left
                    anchors.leftMargin: AppStyle.marginBig + AppStyle.marginNormal
                    sourceSize.width: 32
                    sourceSize.height: 32
                    width: 16
                    height: 16
                    source: "image://typeid/" + model.typeId
                }
                Label {
                    anchors.verticalCenter: implantIcon.verticalCenter
                    anchors.left: implantIcon.right
                    anchors.leftMargin: AppStyle.marginNormal
                    id: lblCurImpName
                    text: model.displayName
                }
            }
        }

        Item {
            width: 1
            height: AppStyle.marginNormal
        }

        Text {
            anchors.left: parent.left
            anchors.leftMargin: AppStyle.marginNormal
            text: qsTr("Other clones:")
            font.bold: true
            font.pointSize: AppStyle.textSizeH2
        }

        Item {
            width: 1
            height: AppStyle.marginNormal
        }

        Repeater {
            width: col1.width
            model: curChar.clonesModel
            delegate: Item {
                implicitHeight: lblCloneName.height
                implicitWidth: col1.width

                Label {
                    id: lblCloneName
                    anchors.left: parent.left
                    anchors.leftMargin: AppStyle.marginBig // + AppStyle.marginNormal
                    text: model.cloneName !== "" ? model.cloneName : "<no name>"
                }
            }
        }
    }
}
