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
        width: parent.width

        TextKeyValue {
            keyText: qsTr("Home location:")
            valueText: curChar.homeLocation.name
        }

        TextKeyValue {
            keyText: qsTr("Last clone jump date:")
            valueText: Qt.formatDateTime(curChar.lastCloneJumpDate)
        }

        ListView {
            width: parent.width
            model: curChar.clonesModel

            delegate: Item {
                Label {
                    text: model.cloneName
                }
            }
        }
    }
}
