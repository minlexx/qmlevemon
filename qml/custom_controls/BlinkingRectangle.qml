import QtQuick 2.7

Rectangle {
    id: targetRect
    color: color1

    property color color1: "white"
    property color color2: "black"
    property int   animationDuration: 300
    property bool  animationRunning: true

    SequentialAnimation {
        running: animationRunning
        loops: Animation.Infinite
        ColorAnimation {
            target: targetRect
            property: "color"
            to: color2
            duration: animationDuration
        }
        ColorAnimation {
            target: targetRect
            property: "color"
            to: color1
            duration: animationDuration
        }
    }
}
