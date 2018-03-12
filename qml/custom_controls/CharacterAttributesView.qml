import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"


Item {
    id: container
    implicitWidth: col1.width + col2.width
    implicitHeight: col1.height

    property int intelligence: 10
    property int memory: 10
    property int perception: 10
    property int willpower: 10
    property int charisma: 10
    property int numBonusRemaps: 0
    property date lastRemapDate
    property date remapCooldownDate
    property int totalSp: 0

    property int iconWidth: 64

    property int fontSize: AppStyle.textSizeH4
    property string fontFamily: AppStyle.fontFamily

    Column {
        id: col1

        CharacterAttributeLine {
            id: attrIntelligence
            iconWidth: iconWidth
            attributeValue: intelligence
            iconSourceUrl: "qrc:///img/char_attributes/intelligence.png"
            font.pointSize: fontSize
            font.family: fontFamily
            tooltipText: qsTr("Intelligence")
        }
        CharacterAttributeLine {
            id: attrMemory
            iconWidth: iconWidth
            attributeValue: memory
            iconSourceUrl: "qrc:///img/char_attributes/memory.png"
            font.pointSize: fontSize
            font.family: fontFamily
            tooltipText: qsTr("Memory")
        }
        CharacterAttributeLine {
            id: attrPerception
            iconWidth: iconWidth
            attributeValue: perception
            iconSourceUrl: "qrc:///img/char_attributes/perception.png"
            font.pointSize: fontSize
            font.family: fontFamily
            tooltipText: qsTr("Perception")
        }
        CharacterAttributeLine {
            id: attrWillpower
            iconWidth: iconWidth
            attributeValue: willpower
            iconSourceUrl: "qrc:///img/char_attributes/willpower.png"
            font.pointSize: fontSize
            font.family: fontFamily
            tooltipText: qsTr("Willpower")
        }
        CharacterAttributeLine {
            id: attrCharisma
            iconWidth: iconWidth
            attributeValue: charisma
            iconSourceUrl: "qrc:///img/char_attributes/charisma.png"
            font.pointSize: fontSize
            font.family: fontFamily
            tooltipText: qsTr("Charisma")
        }
    }

    Column {
        id: col2
        anchors {
            left: col1.right
            top: parent.top
            topMargin: 3
            leftMargin: AppStyle.marginBig
        }

        TextKeyValue {
            height: 32
            keyText: qsTr("Bonus remaps:")
            valueText: numBonusRemaps
            fontPointSize: container.fontSize
            fontFamily: container.fontFamily
        }
        TextKeyValue {
            height: 32
            keyText: qsTr("Last remap date:")
            valueText: Qt.formatDate(lastRemapDate)
            fontPointSize: container.fontSize
            fontFamily: container.fontFamily
        }
        TextKeyValue {
            height: 32
            keyText: qsTr("Remap cooldown:")
            valueText: Qt.formatDate(remapCooldownDate)
            fontPointSize: container.fontSize
            fontFamily: container.fontFamily
        }
        TextKeyValue {
            height: 32
            keyText: qsTr("Total SP:")
            valueText: Number(totalSp).toLocaleString(Qt.locale(), 'f', 0)
            fontPointSize: container.fontSize
            fontFamily: container.fontFamily
        }
    }
}
