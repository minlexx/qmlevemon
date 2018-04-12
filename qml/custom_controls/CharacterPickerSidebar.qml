import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"


Item {
    id: container
    implicitWidth: isVertical ? 48 : imagesRow.width
    implicitHeight: isVertical ? imagesCol.height : 48

    property bool isVertical: true
    property int tooltipFontPointSize: AppStyle.textSizeH3

    signal characterSelected(int char_id)

    Column {
        id: imagesCol
        visible: isVertical
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

    Row {
        id: imagesRow
        visible: !isVertical

        Repeater {
            model: characterModel

            Image {
                width: container.height
                height: container.height
                source: "image://portrait/" + model.characterId

                MouseArea {
                    id: ma1
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        container.characterSelected(model.characterId)
                    }
                }

                ToolTip {
                    visible: ma1.containsMouse
                    text: model.characterName
                    font.pointSize: tooltipFontPointSize
                }
            }
        }
    }
}
