import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../"
import "../custom_controls"

Rectangle {
    id: container
    //anchors.fill: parent // not needed in a StackView
    anchors.margins: AppStyle.marginNormal
    border.color: "red"

    property int mailId: 0

    Text {
        text: "MailView: mailId: " + mailId
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: refresher.isMailDownloadInProgress
        visible: refresher.isMailDownloadInProgress
    }

    Connections {
        target: refresher
        onIsMailDownloadInProgressChanged: {
            console.log("signal: isMailDownloadInProgress changed: ", refresher.isMailDownloadInProgress);
            if (!refresher.isMailDownloadInProgress) {
                container.mailId = curMail.id;
                console.log("body: " + curMail.body);
            }
        }
    }

    property bool hasMailId: typeof(curMail) != "undefined"
    onHasMailIdChanged: {
        if (hasMailId) {
            container.mailId = curMail.id;
            console.log("here!")
        }
    }
}
