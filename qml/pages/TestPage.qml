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

        QQC1.TreeView {
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
        }
    }
}
