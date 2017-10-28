pragma Singleton
import QtQuick 2.7

QtObject {
    property int marginNormal: 10
    property int marginSmall: 5
    property int marginDrag: 20

    property color bgColor: "white"
    property color highlightColor: "#ffe8a2"
    property color mainColor: "#657887"

    property color textDefaultColor: "#000000"
    property color textInvertedColor: "#FFFFFF"
    property color textSecondaryColor: "#333333"
    property color textLightColor: "#666666"
    property color textUrlColor: "#EFC247"

    property int textSizeH1: 20
    property int textSizeH2: 15
    property int textSizeH3: 10
    property int textSizeH4: 9
    property int textSizeH5: 7

    property string fontFamily: "Tahoma"

    property color rectHighlightColor: "#66AAFF"
    property color rectBgHighlightColor: "#CCEEFF"

    // popup colors
    property color neutralPopupBorderColor: "#3d3d3d"
    property color neutralPopupBgColor: "#eaeaea"
    property color neutralPopupTextColor: "#1e1e1e"

    property color infoPopupBorderColor: "#66AAFF"
    property color infoPopupBgColor: "#CCEEFF"
    property color infoPopupTextColor: "#021728"

    property color warningPopupBorderColor: "#d38a1d"
    property color warningPopupBgColor: "#efe9d8"
    property color warningPopupTextColor: "#492806"

    property color errorPopupBorderColor: "#c64931"
    property color errorPopupBgColor: "#fccfcf"
    property color errorPopupTextColor: "#5e1414"
}

