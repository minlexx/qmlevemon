import QtQuick 2.7

Rectangle {
    id: container

    property int rectLevel: 0  // rectangle number, which skill level it should display
    property bool isQueued: false
    property bool isInProgress: false
    property int skillLevelTraining: 0
    property int skillLevelTrained: 0
    property int skillLevelActive: 0

    property color queuedColor: "#6595ea"
    property color normalColor: "black"
    property color alphaColor: "orange"
    property int size: 15

    property int animationDuration: 500

    property bool _isCurrentlyTraining: isInProgress && (rectLevel == skillLevelTraining)

    implicitWidth: size
    implicitHeight: size

    color: (isQueued && (skillLevelTraining == rectLevel))
           ? queuedColor
           : (rectLevel > skillLevelActive
              ? alphaColor
              : normalColor)
    visible: (skillLevelTraining >= rectLevel) || (skillLevelTrained >= rectLevel)

    SequentialAnimation {
        running: _isCurrentlyTraining
        loops: Animation.Infinite
        ColorAnimation {
            target: container
            property: "color"
            to: normalColor
            duration: animationDuration
            alwaysRunToEnd: true
        }
        ColorAnimation {
            target: container
            property: "color"
            to: queuedColor
            duration: animationDuration
            alwaysRunToEnd: true
        }
    }
}
