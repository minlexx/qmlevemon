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

            Item {
                width: 200  // inside SplitView sets only initial width
                Layout.minimumWidth: 200
                Layout.maximumWidth: 600

                Column {
                    id: labelsCol
                    width: parent.width

                    MailLabelRect {
                        width: parent.width
                        labelId: 0
                        labelName: qsTr("All mails")
                        fontBold: true
                        fontPointSize: AppStyle.textSizeH2
                    }

                    Repeater {
                        width: parent.width
                        model: curChar.mailLabels
                        delegate: MailLabelRect {
                            width: labelsCol.width
                            labelId: model.id
                            labelName: model.name
                            labelColor: model.color
                            unreadCount: model.unreadCount
                            fontPointSize: AppStyle.textSizeH2
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                color: "red"
            }
        }
    }
}