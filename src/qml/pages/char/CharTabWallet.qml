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

    Item {
        id: walletTabToolbar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: AppStyle.marginSmall
        anchors.rightMargin: AppStyle.marginSmall
        anchors.topMargin: AppStyle.marginSmall
        height: viewSelectionSwitch.height + AppStyle.marginSmall

        Row {
            spacing: AppStyle.marginSmall
            Label {
                text: "  " + qsTr("View") + ":  "
                verticalAlignment: Text.AlignVCenter
                height: viewSelectionSwitch.height
            }
            Label {
                text: qsTr("Journal")
                verticalAlignment: Text.AlignVCenter
                height: viewSelectionSwitch.height
            }
            Switch {
                id: viewSelectionSwitch
                indicator: Rectangle {
                    implicitWidth: 48
                    implicitHeight: 24
                    x: viewSelectionSwitch.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: height / 2
                    color: AppStyle.textSecondaryColor

                    Rectangle {
                        x: viewSelectionSwitch.checked ? parent.width - width : 0
                        width: parent.height
                        height: parent.height
                        radius: width / 2
                        color: viewSelectionSwitch.down ? "#cccccc" : "#ffffff"
                        border.color: AppStyle.textHighlightColor
                    }
                }
            }
            Label {
                text: qsTr("Transactions")
                verticalAlignment: Text.AlignVCenter
                height: viewSelectionSwitch.height
            }
        }
    }

    ListView {
        id: listViewWalletJournal
        visible: !viewSelectionSwitch.checked
        anchors {
            top: walletTabToolbar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 2
        }
        model: curChar.walletJournal
        interactive: true
        currentIndex: -1
        clip: true

        delegate: ItemDelegate {
            implicitWidth: listViewWalletJournal.width
            implicitHeight: lblDate.height + AppStyle.marginSmall
            Label {
                id: lblDate
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: AppStyle.marginSmall
                anchors.topMargin: AppStyle.marginSmall/2
                width: 140
                verticalAlignment: Text.AlignVCenter
                font.family: "Courier"
                color: AppStyle.textLightColor
                text: Qt.formatDateTime(model.date, "yyyy.dd.MM hh:mm")
            }
            Label {
                id: lblAmount
                anchors.left: lblDate.right
                anchors.top: parent.top
                anchors.topMargin: AppStyle.marginSmall/2
                width: 130
                height: lblDate.height
                font.family: "Courier"
                font.bold: true
                horizontalAlignment: Text.AlignRight
                color: model.amount >= 0 ? AppStyle.iskPositiveChangeColor : AppStyle.iskNegativeChangeColor
                text: (model.amount >= 0 ? "+" : "") + Number(model.amount).toLocaleString(Qt.locale(), 'f', 0)
            }
            Label {
                id: lblBalance
                anchors.left: lblAmount.right
                anchors.top: parent.top
                anchors.topMargin: AppStyle.marginSmall/2
                width: 140
                height: lblDate.height
                font.family: "Courier"
                font.bold: true
                horizontalAlignment: Text.AlignRight
                color: AppStyle.textHighlightColor
                text: Number(model.balance).toLocaleString(Qt.locale(), 'f', 0)
            }
            Label {
                id: lblDescription
                anchors.left: lblBalance.right
                anchors.leftMargin: AppStyle.marginNormal
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: AppStyle.marginSmall/2
                height: lblDate.height
                font.family: AppStyle.fontFamily
                font.pointSize: AppStyle.textSizeH4
                text: model.description
            }
        }

        ScrollIndicator.vertical: ScrollIndicator { }
    }

    ListView {
        id: listViewWalletTransactions
        visible: viewSelectionSwitch.checked
        anchors {
            top: walletTabToolbar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 2
        }
        model: curChar.walletTransactions
        interactive: true
        currentIndex: -1
        clip: true

        delegate: ItemDelegate {
            implicitWidth: listViewWalletJournal.width
            implicitHeight: 32

            property bool isClientNPCCorp: (model.clientId >= 1000000) && (model.clientId <= 2000000)
            // ^^ https://gist.github.com/a-tal/5ff5199fdbeb745b77cb633b7f4400bb

            Label {
                id: lblTDate
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: AppStyle.marginSmall
                anchors.topMargin: AppStyle.marginSmall/2
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                font.family: "Courier"
                color: AppStyle.textLightColor
                text: Qt.formatDateTime(model.date, "yyyy.dd.MM hh:mm")
            }
            Label {
                id: lblTBalance
                anchors.left: lblTDate.right
                anchors.top: parent.top
                anchors.topMargin: AppStyle.marginSmall/2
                width: 110
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                font.family: "Courier"
                font.bold: true
                horizontalAlignment: Text.AlignRight
                color: model.isBuy ? AppStyle.iskNegativeChangeColor : AppStyle.iskPositiveChangeColor
                text: (model.isBuy ? "-" : "+") + Number(model.quantity * model.unitPrice).toLocaleString(Qt.locale(), 'f', 0)
            }
            Label {
                id: lblTTypeName
                anchors.left: lblTBalance.right
                anchors.top: parent.top
                anchors.topMargin: AppStyle.marginSmall/2
                anchors.leftMargin: AppStyle.marginSmall
                width: 330
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                font.family: AppStyle.fontFamily
                font.pointSize: AppStyle.textSizeH4
                text: model.quantity + " x " + model.typeName
                wrapMode: Text.Wrap
            }
            Label {
                id: lblTLocation
                anchors.left: lblTTypeName.right
                anchors.leftMargin: AppStyle.marginNormal
                anchors.right: lblTClientName.left
                anchors.top: parent.top
                anchors.topMargin: AppStyle.marginSmall/2
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                font.family: AppStyle.fontFamily
                font.pointSize: AppStyle.textSizeH4
                text: model.locationName
                wrapMode: Text.Wrap
            }
            Item {
                id: lblTClientName
                anchors.leftMargin: AppStyle.marginSmall
                anchors.top: parent.top
                anchors.right: parent.right
                width: 32
                height: 32
                Image {
                    id: clientPic
                    anchors.fill: parent
                    source: isClientNPCCorp ? ("image://corporation/" + model.clientId)
                                            : ("image://portrait/" + model.clientId)
                    sourceSize.width: 32
                    sourceSize.height: 32
                    MouseArea {
                        id: clientPicMa
                        hoverEnabled: true
                        anchors.fill: parent
                    }
                    ToolTip.visible: clientPicMa.containsMouse
                    ToolTip.text: model.clientName
                }
            }
        }

        ScrollIndicator.vertical: ScrollIndicator { }
    }
}
