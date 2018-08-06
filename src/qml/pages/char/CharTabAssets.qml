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
    clip: true

    ListView {
        id: lvAssetsLocations
        anchors {
            fill: parent
            margins: 2
        }
        model: curChar.assetsLocations
        interactive: true
        currentIndex: -1
        clip: true

        delegate: ItemDelegate {
            width: lvAssetsLocations.width
            height: lblLocName.height + AppStyle.marginSmall * 2
            Label {
                id: lblLocName
                text: model.locationName + " - " + model.count + " item (s)"
            }
        }
    }
}
