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

    ListView {
        id: lvAssetsLocations
        anchors {
            fill: parent
            margins: 2
        }
        model: curChar.assetsLocations
        interactive: true
        currentIndex: -1
        clip: true

        delegate: ItemDelegate {
            id: headerDelegate
            width: lvAssetsLocations.width
            height: ListView.isCurrentItem ? delegateHeaderRect.height + delegateContentRect.height
                                           : delegateHeaderRect.height

            Rectangle {
                id: delegateHeaderRect
                width: parent.width
                height: lblLocName.height
                border {
                    width: 1
                    color: "white"
                }
                color: headerDelegate.ListView.isCurrentItem ? AppStyle.rectBgHighlightColor
                                                             : AppStyle.textLightColor
                Label {
                    id: lblLocName
                    // height is bigger on android
                    height: evemonapp.isDesktopPlatform ? implicitHeight + 2*AppStyle.marginSmall
                                                        : implicitHeight + 2*AppStyle.marginNormal
                    anchors {
                        top: parent.top
                        left: parent.left
                        leftMargin: AppStyle.marginNormal
                    }
                    text: model.locationName + " - " + model.count + " item (s)"
                    maximumLineCount: 1
                    font.pointSize: AppStyle.textSizeH3
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                    color: headerDelegate.ListView.isCurrentItem ? AppStyle.textDefaultColor
                                                                 : AppStyle.textInvertedColor
                }

                MouseArea {
                    id: delegateHeaderRectMa1
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor

                    onClicked: {
                        var curIdx = lvAssetsLocations.currentIndex
                        var clickedIndex = index;
                        if (clickedIndex === curIdx) {
                            // the same item is clicked twice, collapse all
                            lvAssetsLocations.currentIndex = -1;
                        } else {
                            // activate different item
                            delegateContentRepeater.model = curChar.filterAssetsByLocation(model.locationId);
                            lvAssetsLocations.currentIndex = clickedIndex;
                        }
                    }
                }
            }

            Rectangle {
                id: delegateContentRect
                visible: headerDelegate.ListView.isCurrentItem
                width: lvAssetsLocations.width
                height: delegateContentColumn.height
                anchors.top: delegateHeaderRect.bottom
                color: AppStyle.bgColor

                Column {
                    id: delegateContentColumn
                    width: parent.width

                    Repeater {
                        id: delegateContentRepeater
                        model: null
                        delegate: Rectangle {
                            property int index
                            width: delegateContentColumn.width
                            height: txtAssetName.height
                            color: index % 2 == 0 ? AppStyle.bgColor : AppStyle.bgLightColor

                            Text {
                                id: txtAssetName
                                anchors {
                                    top: parent.top
                                    left: parent.left
                                    leftMargin: AppStyle.marginNormal
                                }
                                height: implicitHeight + AppStyle.marginSmall * 2
                                verticalAlignment: Text.AlignVCenter
                                font.pointSize: AppStyle.textSizeH4
                                font.family: AppStyle.fontFamily
                                text: model.quantity + " x " + model.typeName
                            }
                        }
                        onItemAdded: item.index = index;
                    }
                }
            }
        }
    }
}
