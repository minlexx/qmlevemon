import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../"

Rectangle {
    id: container
    clip: true

    Rectangle {
        id: innerRect
        anchors.fill: parent
        anchors.margins: AppStyle.marginNormal

        Flickable {
            anchors.fill: parent
            contentWidth: settingsCol1.width // contentItem.childrenRect.width;
            contentHeight: settingsCol1.height // contentItem.childrenRect.height

            Column {
                id: settingsCol1
                width: innerRect.width
                spacing: AppStyle.marginSmall

                Item {
                    // proxy enable
                    anchors.left: parent.left
                    anchors.right: parent.right
                    implicitHeight: swEnableProxy.height
                    Switch {
                        id: swEnableProxy
                        text: qsTr("Connect using proxy")
                        checked: evemonapp.settings.isProxyEnabled
                        onClicked: evemonapp.settings.isProxyEnabled = checked;
                    }
                }

                Item {
                    // proxy type
                    enabled: evemonapp.settings.isProxyEnabled
                    anchors.left: parent.left
                    anchors.right: parent.right
                    implicitHeight: cbProxyType.height
                    Row {
                        height: cbProxyType.height
                        spacing: AppStyle.marginNormal

                        Label {
                            anchors.verticalCenter: cbProxyType.verticalCenter
                            text: qsTr("Proxy type:") + " "
                        }

                        ComboBox {
                            id: cbProxyType
                            textRole: "text"
                            model: ListModel {
                                id: cbProxyTypeModel
                                ListElement {
                                    text: "HTTP";
                                    value: 3  // QNetworkProxy::HttpProxy
                                }
                                ListElement {
                                    text: "SOCKS5";
                                    value: 1  // QNetworkProxy::Socks5Proxy
                                }
                            }
                            currentIndex: proxyTypeToIndex(evemonapp.settings.proxyType);

                            function proxyTypeToIndex(typ) {
                                switch (typ) {
                                case 3: return 0;
                                case 1: return 1;
                                }
                                return -1;
                            }

                            onActivated: {
                                if (currentIndex >= 0) {
                                    evemonapp.settings.proxyType = cbProxyTypeModel.get(currentIndex).value;
                                }
                            }
                        }
                    }
                }

                Item {
                    // proxy addr
                    enabled: evemonapp.settings.isProxyEnabled
                    anchors.left: parent.left
                    anchors.right: parent.right
                    implicitHeight: teProxyHost.height
                    Row {
                        spacing: AppStyle.marginSmall
                        Label {
                            anchors.verticalCenter: teProxyHost.verticalCenter
                            text: qsTr("Proxy address:")
                        }
                        TextField {
                            id: teProxyHost
                            width: 100
                            text: evemonapp.settings.proxyHost
                            onEditingFinished: evemonapp.settings.proxyHost = text;
                        }
                        Label {
                            anchors.verticalCenter: teProxyHost.verticalCenter
                            text: ":"
                        }
                        TextField {
                            id: teProxyPort
                            width: 50
                            text: evemonapp.settings.proxyPort
                            onEditingFinished: evemonapp.settings.proxyPort = text;
                            validator: IntValidator {
                                bottom: 1
                                top: 65535
                            }
                        }
                    }
                }

                Item {
                    // proxy user
                    enabled: evemonapp.settings.isProxyEnabled
                    anchors.left: parent.left
                    anchors.right: parent.right
                    implicitHeight: teProxyLogin.height
                    Row {
                        spacing: AppStyle.marginSmall
                        Label {
                            anchors.verticalCenter: teProxyLogin.verticalCenter
                            text: qsTr("Proxy username:")
                        }
                        TextField {
                            id: teProxyLogin
                            width: 100
                            text: evemonapp.settings.proxyUser
                            onEditingFinished: evemonapp.settings.proxyUser = text;
                        }
                    }
                }

                Item {
                    // proxy pass
                    enabled: evemonapp.settings.isProxyEnabled
                    anchors.left: parent.left
                    anchors.right: parent.right
                    implicitHeight: teProxyPass.height
                    Row {
                        spacing: AppStyle.marginSmall
                        Label {
                            anchors.verticalCenter: teProxyPass.verticalCenter
                            text: qsTr("Proxy password:")
                        }
                        TextField {
                            id: teProxyPass
                            width: 100
                            echoMode: TextInput.Password
                            text: evemonapp.settings.proxyPassword
                            onEditingFinished: evemonapp.settings.proxyPassword = text;
                        }
                    }
                }
            }
        }
    }
}
