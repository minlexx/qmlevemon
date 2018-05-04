import QtQuick 2.7
import QtQuick.Controls 1.4 as QQC1
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

    Item {
        anchors.fill: parent
        anchors.margins: AppStyle.marginNormal

        QQC1.SplitView {
            anchors.fill: parent
            orientation: Qt.Horizontal
            handleDelegate: Rectangle {
                width: AppStyle.marginNormal
                color: AppStyle.bgLightColor
            }

            Rectangle {
                width: 200  // inside SplitView sets only initial width
                Layout.minimumWidth: 200
                Layout.maximumWidth: 600
                color: "green"
            }

            Rectangle {
                Layout.fillWidth: true
                color: "red"
            }
        }
    }
}
