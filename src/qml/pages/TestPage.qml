import QtQuick 2.7
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../"

Rectangle {
    id: container
    //anchors.fill: parent // not needed in a StackView
    anchors.margins: AppStyle.marginNormal
    clip: true

    border.color: "blue"
    border.width: 1

    Rectangle {
        id: innerRect
        anchors.fill: parent
        anchors.margins: AppStyle.marginNormal
        radius: AppStyle.marginNormal
        border.width: 2
        border.color: AppStyle.infoPopupBorderColor
        color: AppStyle.infoPopupBgColor

        /* QQC1.TreeView {
            anchors.fill: parent
            anchors.margins: 20
            model: skillTreeModel
            selectionMode: QQC1.SelectionMode.SingleSelection

            QQC1.TableViewColumn {
                width: 500
                role: "name"
                title: qsTr("Name")
            }
            QQC1.TableViewColumn {
                width: 100
                role: "id"
                title: qsTr("Id")
            }
        } */

        /*Rectangle {
            id: rc1
            width: 100
            height: 100
            anchors.centerIn: parent
            color: "red"

            SequentialAnimation {
                running: true
                loops: Animation.Infinite
                ColorAnimation {
                    target: rc1
                    property: "color"
                    from: "white"
                    to: "black"
                    duration: 1000
                }
                ColorAnimation {
                    target: rc1
                    property: "color"
                    from: "black"
                    to: "white"
                    duration: 1000
                }
            }
        } */

        Image {
            id: testIcon
            anchors.centerIn: parent
            sourceSize.width: 32
            sourceSize.height: 32;
            source: "image://typeid/14264"
        }

        Column {
            spacing: 5

            Row {
                spacing: 5

                Button {
                    id: testNotifyButton
                    text: "Test notification"
                    font.pointSize: AppStyle.textSizeH3
                    onClicked: evemonapp.showNotification("QML EVEMon", "Hello from QML!")
                }

                Button {
                    id: testErrorPopupButton
                    text: "Test error popup"
                    font.pointSize: AppStyle.textSizeH3
                    onClicked: evemonapp.testErrorHandler("error", "Test error")
                }

                Button {
                    id: testInfoPopupButton
                    text: "Test info popup"
                    font.pointSize: AppStyle.textSizeH3
                    onClicked: evemonapp.testErrorHandler("info", "Test info")
                }

                Button {
                    id: testWarningPopupButton
                    text: "Test warning popup"
                    font.pointSize: AppStyle.textSizeH3
                    onClicked: evemonapp.testErrorHandler("warning", "Test warning")
                }
            }

            Row {
                spacing: 5
                visible: !evemonapp.isDesktopPlatform // visible only on Android

                Button {
                    id: testAndroidServiceStart
                    text: "Start android service"
                    font.pointSize: AppStyle.textSizeH3
                    onClicked: evemonapp.startAndroidService()
                }
                Button {
                    id: testAndroidServiceStop
                    text: "Stop android service"
                    font.pointSize: AppStyle.textSizeH3
                    onClicked: evemonapp.stopAndroidService()
                }
            }
        }

    }
}
