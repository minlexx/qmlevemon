import QtQuick 2.7

Rectangle {
    id: container

    property int rectLevel: 0
    property bool isQueued: false
    property bool isInProgress: false
    property int skillLevelTraining: 0
    property int skillLevelTrained: 0
    property int skillLevelActive: 0

    property color queuedColor: "#6595ea"
    property color normalColor: "black"
    property int size: 15

    implicitWidth: size
    implicitHeight: size
    color: (isQueued && (skillLevelTraining == rectLevel)) ? queuedColor : normalColor
    visible: (skillLevelTraining >= rectLevel) || (skillLevelTrained >= rectLevel)
}
