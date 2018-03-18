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
    clip: true

    Text { text: "Clones" }
}
