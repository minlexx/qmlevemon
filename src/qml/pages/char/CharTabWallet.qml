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
                text: Number(model.amount).toLocaleString(Qt.locale(), 'f', 0)
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
                text: model.description
            }
        }

        ScrollIndicator.vertical: ScrollIndicator { }
    }
}
