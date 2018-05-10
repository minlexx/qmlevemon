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
        anchors.margins: AppStyle.marginSmall

        QQC1.SplitView {
            id: sv1
            anchors.fill: parent
            orientation: Qt.Horizontal
            handleDelegate: Rectangle {
                width: AppStyle.marginNormal
                color: AppStyle.bgLightColor
            }

            function mailLabelClicked(itemIndex, labelId) {
                mailLabelsContainer.currentSelectedIndex = itemIndex;
                mailHeadersLV.model = curChar.filterMailsForLabel(labelId);
                mailHeadersLV.currentIndex = -1;
            }

            Item {
                id: mailLabelsContainer
                width: 200  // inside SplitView sets only initial width
                Layout.minimumWidth: 100
                Layout.maximumWidth: 500

                property int currentSelectedIndex: 999

                Column {
                    id: labelsCol
                    width: parent.width

                    MailLabelRect {
                        width: parent.width - AppStyle.marginSmall
                        labelId: 0
                        labelName: " " + qsTr("All mails")
                        fontBold: isCurrent
                        fontPointSize: AppStyle.textSizeH2
                        itemIndex: 999
                        isCurrent: mailLabelsContainer.currentSelectedIndex == itemIndex

                        onClicked: {
                            sv1.mailLabelClicked(itemIndex, labelId);
                        }
                    }

                    Repeater {
                        width: parent.width
                        model: curChar.mailLabels
                        delegate: MailLabelRect {
                            width: labelsCol.width - AppStyle.marginSmall
                            labelId: model.id
                            labelName: " " + model.name
                            labelColor: model.color
                            unreadCount: model.unreadCount
                            fontPointSize: AppStyle.textSizeH2
                            fontBold: isCurrent
                            itemIndex: index
                            isCurrent: mailLabelsContainer.currentSelectedIndex == itemIndex
                            textColor: labelId > 8 ? AppStyle.textLightColor : AppStyle.textDefaultColor

                            onClicked: {
                                sv1.mailLabelClicked(itemIndex, labelId);
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: mailHeadersRect
                Layout.fillWidth: true

                ListView {
                    id: mailHeadersLV
                    anchors.fill: parent
                    model: curChar.mails
                    currentIndex: -1
                    delegate: Item {
                        id: mailDelegateItem
                        width: mailHeadersRect.width
                        height: col1.height //+ AppStyle.marginSmall*2
                        readonly property bool isCurrentItem: ListView.isCurrentItem

                        Rectangle {
                            id: mailDelegateRect
                            anchors.fill: parent
                            anchors.leftMargin: AppStyle.marginSmall
                            color: isCurrentItem ? AppStyle.rectBgHighlightColor : AppStyle.bgColor

                            Column {
                                id: col1
                                anchors.left: parent.left
                                anchors.top: parent.top
                                anchors.leftMargin: AppStyle.marginSmall

                                // small spacer
                                Item {
                                    width: 1;
                                    height: AppStyle.marginSmall
                                }

                                Row {
                                    id: rowFromDate
                                    Label {
                                        id: lblFrom
                                        text: qsTr("From:") + " " + model.fromName
                                        font.pointSize: AppStyle.textSizeH3
                                        // font.bold: !model.isRead
                                    }
                                    Item {
                                        height: 1
                                        width: (lblFrom.width < 200 ? 200 - lblFrom.width : AppStyle.marginBig)
                                    }
                                    Label {
                                        id: lblTimestamp
                                        text: qsTr("Date:") + " " + Qt.formatDateTime(model.timestamp)
                                        font.pointSize: AppStyle.textSizeH3
                                        // font.bold: !model.isRead
                                    }
                                }

                                Label {
                                    id: lblSubject
                                    text: model.subject
                                    font.pointSize: AppStyle.textSizeH3
                                    font.bold: !model.isRead
                                }

                                // small spacer
                                Item {
                                    width: 1;
                                    height: AppStyle.marginSmall
                                }
                            }

                            MouseArea {
                                id: mailDelegateRectMa
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    mailHeadersLV.currentIndex = index;
                                }
                            }
                        } // delegate rectangle
                    } // delegate item
                }
            }
        }
    }
}
