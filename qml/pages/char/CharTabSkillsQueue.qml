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
    width: subInfoSwipeView.width
    height: subInfoSwipeView.height
    //anchors { left: parent.left; right: parent.right; top: parent.top; bottom: parent.bottom }
    // ^^ causes hang
    clip: true

    Text { text: "Skills queue" }

    // exploring parents chain
    Component.onCompleted: {
        console.log(parent);                // QQuickItem(0x251b0b97b70)
        console.log(parent.parent);         // QQuickListView(0x251b1054440)
        console.log(parent.parent.parent);  // QQuickSwipeView(0x251b1054260) (bingo)
        console.log(subInfoSwipeView);      // QQuickSwipeView(0x251b1054260)
    }
}
