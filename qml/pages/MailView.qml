import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../"
import "../custom_controls"

Item {
    id: mailView
    anchors.margins: AppStyle.marginNormal

    property int    mailId: 0
    property string mailSubject: ""
    property string mailBody: ""
    property string mailDt: ""
    property string mailFromName: ""
    property int    mailFromId: 0
    property bool   mailIsRead: true
    property string mailLabelsStr: ""
    property string mailRecipientsStr: ""

    signal requestSetPageTitle(string title)

    Rectangle {
        anchors.fill: parent
        anchors.margins: AppStyle.marginSmall
        border.color: AppStyle.textDefaultColor
        border.width: 1

        BusyIndicator {
            anchors.centerIn: parent
            running: refresher.isMailDownloadInProgress
            visible: refresher.isMailDownloadInProgress
        }

        Item {
            id: mailHeadersContainer
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: AppStyle.marginNormal
            anchors.topMargin: AppStyle.marginSmall
            height: childrenRect.height
            visible: mailView.mailId > 0

            Image {
                id: fromProfilePic
                anchors.left: parent.left
                anchors.top: parent.top
                source: mailFromId > 0 ? "image://portrait/" + mailFromId : "qrc:///img/character_icon_128.jpg"
                sourceSize.width: 64
                sourceSize.height: 64
                width: 64
                height: 64
            }

            Column {
                id: mailHeadersCol
                anchors.left: fromProfilePic.right
                anchors.top: parent.top
                anchors.leftMargin: AppStyle.marginNormal

                Row {
                    Label {
                        text: qsTr("Subject:") + " "
                        font.bold: true
                    }
                    Label {
                        text: mailSubject
                    }
                }

                Row {
                    Label {
                        text: qsTr("From:") + " "
                        font.bold: true
                    }
                    Label {
                        text: mailFromName
                    }
                }

                Row {
                    Label {
                        text: qsTr("To:") + " "
                        font.bold: true
                    }
                    Label {
                        text: mailRecipientsStr
                    }
                }

                Row {
                    Label {
                        text: qsTr("Date:") + " "
                        font.bold: true
                    }
                    Label {
                        text: mailDt
                    }
                }

                Row {
                    Label {
                        text: qsTr("Labels:") + " "
                        font.bold: true
                    }
                    Label {
                        text: mailLabelsStr
                    }
                }
            }
        }

        Item {
            id: mailBodyContainer
            anchors.top: mailHeadersContainer.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: AppStyle.marginNormal
            visible: mailView.mailId > 0

            Rectangle {
                anchors.fill: parent
                color: AppStyle.mailViewBgColor
                clip: true

                Flickable {
                    anchors.fill: parent
                    contentWidth: parent.width
                    contentHeight: mailBodyText.height + AppStyle.marginBig*2

                    Text {
                        id: mailBodyText
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: AppStyle.marginSmall
                        anchors.topMargin: AppStyle.marginSmall
                        text: "<font color=\"" + AppStyle.mailViewTextDefaultColor + "\">" + mailBody + "</font>"
                        textFormat: Text.StyledText
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }
                }
            }
        }
    }

    Connections {
        target: evemonapp

        // when mail is downloaded/loaded from cache, update view properties
        //      and display mail headers and contents
        onMailReady: {
            if (!refresher.isMailDownloadInProgress) {
                mailView.mailId = curMail.id;
                mailView.mailSubject = curMail.subject;
                mailView.mailBody = curMail.body;
                mailView.mailDt = Qt.formatDateTime(curMail.timestamp);
                mailView.mailFromName = curMail.fromName;
                mailView.mailFromId = curMail.fromId;
                mailView.mailIsRead = curMail.is_read;
                mailView.mailLabelsStr = curMail.labelsString;
                mailView.mailRecipientsStr = curMail.recipientsString;
                mailView.requestSetPageTitle(curChar.characterName + " / " + qsTr("Mail") + ": " + curMail.subject);
            }
        }
    }
}
