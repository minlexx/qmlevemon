import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../"

Item {
    id: container
    //anchors.fill: parent // not needed in a StackView
    anchors.margins: AppStyle.marginNormal
    clip: true

    signal loginWithEveOnline()
    signal loginCancel()
    signal loginWasOk()

    Rectangle {
        id: helpRect
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: AppStyle.marginNormal

        height: ssoHelpText.contentHeight + 2*AppStyle.marginNormal

        radius: AppStyle.marginNormal
        border.width: 2
        border.color: AppStyle.infoPopupBorderColor
        color: AppStyle.infoPopupBgColor

        Text {
            id: ssoHelpText
            anchors.fill: parent
            anchors.margins: AppStyle.marginNormal
            font.family: AppStyle.fontFamily
            font.pointSize: AppStyle.textSizeH3
            color: AppStyle.infoPopupTextColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            textFormat: Text.StyledText
            clip: true
            text: qsTr("You must login with your EVE Online account. \
When you click <b>\"LOG IN with EVE Online\"</b>, your default browser \
should open, and you will be redirected to <b>https://login.eveonline.com</b>, \
where you should enter your account details. After successfull \
authorization you should select character to login with, confirm \
permissions, and then you should see a page saying that your login \
was successful. Then switch back to this application.")
        }
    }

    Rectangle {
        id: btnLoginWithEve
        width: 300
        height: 50
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: helpRect.bottom
            topMargin: AppStyle.marginNormal
        }

        Image {
            anchors.centerIn: parent
            source: "../../img/eve-sso/EVE_SSO_Login_Buttons_Large_White.png"
        }

        MouseArea {
            id: buttonMa
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor

            onClicked: {
                waitingAuthPopup.open();
                container.loginWithEveOnline();
            }
        }
    }

    Button {
        id: btnCancel
        anchors.horizontalCenter: parent.horizontalCenter
        y: btnLoginWithEve.y + btnLoginWithEve.height + AppStyle.marginNormal
        text: qsTr("Cancel")
        font.pointSize: AppStyle.textSizeH3
        onClicked: container.loginCancel();
    }

    Popup {
        id: waitingAuthPopup
        visible: false
        clip: true
        closePolicy: Popup.NoAutoClose

        width: 400
        height: 300
        x: parent.width/2 - width/2
        y: parent.height/2 - height/2

        background: Rectangle {
            implicitWidth: 400
            implicitHeight: 300
            border.width: 3
            border.color: AppStyle.neutralPopupBorderColor
            color: AppStyle.neutralPopupBgColor
            radius: AppStyle.marginNormal
        }

        BusyIndicator {
            anchors.centerIn: parent
            running: true
        }

        Text {
            anchors.fill: parent
            font.family: AppStyle.fontFamily
            font.pointSize: AppStyle.textSizeH2
            color: AppStyle.neutralPopupTextColor
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            text: qsTr("Waiting for an authorization...")
        }

        Button {
            x: parent.x + parent.width/2 - width/2
            y: parent.y + parent.height - height - AppStyle.marginNormal
            text: qsTr("Cancel")
            font.pointSize: AppStyle.textSizeH3
            onClicked: {
                waitingAuthPopup.close();
                eveSsoLoginManager.slotCancelSsoAuth();
            }
        }
    }


    Popup {
        id: authErrorPopup
        visible: false
        clip: true
        closePolicy: Popup.NoAutoClose

        width: 400
        height: 300
        x: parent.width/2 - width/2
        y: parent.height/2 - height/2

        background: Rectangle {
            implicitWidth: 400
            implicitHeight: 300
            border.width: 3
            border.color: AppStyle.errorPopupBorderColor
            color: AppStyle.errorPopupBgColor
            radius: AppStyle.marginNormal
        }

        Text {
            id: authErrorPopupText
            anchors.fill: parent
            font.family: AppStyle.fontFamily
            font.pointSize: AppStyle.textSizeH2
            color: AppStyle.neutralPopupTextColor
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            text: qsTr("Error message here...")
        }

        Button {
            x: parent.x + parent.width/2 - width/2
            y: parent.y + parent.height - height - AppStyle.marginNormal
            text: qsTr("Close")
            onClicked: {
                authErrorPopup.close();
            }
        }
    }


    Connections {
        target: eveSsoLoginManager
        onSsoAuthOk: {
            waitingAuthPopup.close();
            console.log("SSO Auth OK");
            container.loginWasOk();
        }
        onSsoAuthError: {
            waitingAuthPopup.close();
            console.warn("SSO Auth ERROR: " + message);
            authErrorPopupText.text = message;
            authErrorPopup.open();
        }
    }
}
