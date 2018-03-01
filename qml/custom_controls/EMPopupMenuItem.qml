import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"

/**
  Customized Menu item control, that can color background
  when mouse hovered over this item.
  Inherits MenuItem;
  Has additional properties:
   - property color bgColor: normal color
   - property color highlightColor: color when mouse hovered
 */
MenuItem {
    id: em_menuItemContainter

    property color highlightColor: AppStyle.rectBgHighlightColor
    property color bgColor: AppStyle.bgColor

    text: "EM Menu item very very long"  // default text
    font.pointSize: AppStyle.textSizeH2
    hoverEnabled: true

    background: Item {
        Rectangle {
            anchors.margins: 2
            anchors.fill: parent
            color: (em_menuItemContainter.highlighted || em_menuItemContainter.hovered)
                   ? highlightColor
                   : bgColor
        }
    }
}
