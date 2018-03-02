import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"


Item {
    id: container
    implicitWidth: 48
    implicitHeight: imagesCol.height

    property int tooltipFontPointSize: AppStyle.textSizeH3

    signal characterSelected(int char_id)

    Column {
        id: imagesCol
        Repeater {
            model: characterModel

            Image {
                width: container.width
                height: container.width
                source: "image://portrait/" + model.characterId

                MouseArea {
                    id: ma
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        container.characterSelected(model.characterId)
                    }
                }

                ToolTip {
                    visible: ma.containsMouse
                    text: model.characterName
                    font.pointSize: tooltipFontPointSize
                }
            }
        }
    }
}
