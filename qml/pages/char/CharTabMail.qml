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
                id: mailLabelsContainer
                width: 200  // inside SplitView sets only initial width
                Layout.minimumWidth: 200
                Layout.maximumWidth: 600

                property int currentSelectedIndex: 999

                Column {
                    id: labelsCol
                    width: parent.width

                    MailLabelRect {
                        width: parent.width - AppStyle.marginNormal
                        labelId: 0
                        labelName: qsTr("All mails")
                        fontBold: true
                        fontPointSize: AppStyle.textSizeH2
                        itemIndex: 999
                        isCurrent: mailLabelsContainer.currentSelectedIndex == itemIndex

                        onClicked: {
                            mailLabelsContainer.currentSelectedIndex = itemIndex;
                        }
                    }

                    Repeater {
                        width: parent.width
                        model: curChar.mailLabels
                        delegate: MailLabelRect {
                            width: labelsCol.width - AppStyle.marginNormal
                            labelId: model.id
                            labelName: model.name
                            labelColor: model.color
                            unreadCount: model.unreadCount
                            fontPointSize: AppStyle.textSizeH2
                            itemIndex: index
                            isCurrent: mailLabelsContainer.currentSelectedIndex == itemIndex
                            textColor: labelId > 8 ? AppStyle.textLightColor : AppStyle.textDefaultColor

                            onClicked: {
                                mailLabelsContainer.currentSelectedIndex = itemIndex;
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: mailHeadersRect
                Layout.fillWidth: true
                //color: "red"

                ListView {
                    id: mailHeadersLV
                    anchors.fill: parent
                    model: curChar.mails
                    delegate: Item {
                        implicitWidth: mailHeadersRect.width
                        implicitHeight: col1.height + AppStyle.marginSmall*2

                        Column {
                            anchors.left: parent.left
                            anchors.leftMargin: AppStyle.marginNormal
                            id: col1
                            Row {
                                Label {
                                    id: lblFrom
                                    text: qsTr("From:") + " " + model.fromName
                                    font.pointSize: AppStyle.textSizeH3
                                    //font.bold: !model.isRead
                                }
                                Item { height: 1;  width: AppStyle.marginBig }
                                Label {
                                    id: lblTimestamp
                                    text: qsTr("Date:") + " " + Qt.formatDateTime(model.timestamp)
                                    font.pointSize: AppStyle.textSizeH3
                                    //font.bold: !model.isRead
                                }
                            }
                            Label {
                                id: lblSubject
                                text: model.subject
                                font.pointSize: AppStyle.textSizeH3
                                font.bold: !model.isRead
                            }
                        }
                    }
                }
            }
        }
    }
}
