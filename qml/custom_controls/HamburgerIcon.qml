import QtQuick 2.7

Rectangle {
    id: hamburgerIconContainer
    width: 48
    height: 48
    color: "black"
    property color colorArrows: "white"
    property alias iconState: menuBackIcon.state
    property bool autoChangeState: false

    signal clicked

    MouseArea {
        anchors.fill: parent
        onClicked: {
            hamburgerIconContainer.clicked();
            if (autoChangeState) {
                menuBackIcon.state = menuBackIcon.state === "menu" ? "back" : "menu";
            }
        }
    }

    HamburgerMenuBackIcon {
        id: menuBackIcon
        anchors.centerIn: parent
    }
}
