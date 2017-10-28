import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../"
import "../custom_controls"

Rectangle {
    id: container
    anchors.margins: AppStyle.marginNormal
    clip: true

    border.color: "blue"
    border.width: 0

    Image {
        id: profilePic
        anchors.left: parent.left
        anchors.leftMargin: AppStyle.marginNormal
        anchors.top: parent.top
        anchors.topMargin: AppStyle.marginNormal
        source: "image://portrait/" + curChar.characterId
        //source: "../../tests/92181726_128.jpg"
        sourceSize.width: 128
        sourceSize.height: 128
    }

    Text {
        id: charNameText
        anchors.left: profilePic.right
        anchors.top: parent.top
        anchors.leftMargin: AppStyle.marginNormal
        anchors.topMargin: AppStyle.marginNormal
        text: curChar.characterName
        //text: "Test Char"
        font.family: AppStyle.fontFamily
        font.pointSize: AppStyle.textSizeH2
        font.bold: true
    }
    Text {
        id: charOriginsText
        anchors.left: profilePic.right
        anchors.top: charNameText.bottom
        anchors.leftMargin: AppStyle.marginNormal
        anchors.topMargin: AppStyle.marginSmall
        text: (curChar.gender ? qsTr("Female") : qsTr("Male")) + ", "
              + curChar.raceName + " - "
              + curChar.ancestryName + " - "
              + curChar.bloodlineName
        //text: "Male, Caldari - Achura - Inventors"
        font.family: AppStyle.fontFamily
        font.pointSize: AppStyle.textSizeH3
        color: AppStyle.textLightColor
    }

    TextKeyValue {
        id: charBalanceText
        anchors.left: profilePic.right
        anchors.top: charOriginsText.bottom
        keyText: qsTr("Balance:")
        //valueText: "43 234 123,03 ISK"
        valueText: curChar.iskAmountStr
    }

    TextKeyValue {
        id: charBirthdayText
        anchors.left: profilePic.right
        anchors.top: charBalanceText.bottom
        keyText: qsTr("Birthday:")
        //valueText: "18.10.2015 12:20:41"
        valueText: curChar.birthday
    }

    TextKeyValue {
        id: charCorpText
        anchors.left: profilePic.right
        anchors.top: charBirthdayText.bottom
        keyText: qsTr("Corporation:")
        //valueText: "Through The Event Horizon [T-TEH]"
        valueText: curChar.corporationName + " [" + curChar.corporationTicker + "]"
    }

    TextKeyValue {
        id: charAllyText
        anchors.left: profilePic.right
        anchors.top: charCorpText.bottom
        keyText: qsTr("Alliance:")
        //valueText: "Red Alliance <RED>"
        valueText: (curChar.allianceId > 0 ? curChar.allianceName +
                   " <" + curChar.allianceTicker + ">" : "-")
    }

    TextKeyValue {
        id: charSecStatusText
        anchors.left: profilePic.right
        anchors.top: charAllyText.bottom
        keyText: qsTr("Security Status:")
        //valueText: "-1.2"
        valueText: curChar.securityStatusStr
    }

    TextKeyValue {
        id: charActiveShipText
        anchors.left: profilePic.right
        anchors.top: charSecStatusText.bottom
        keyText: qsTr("Active Ship:")
        //valueText: "Proteus [Scanner Probe]"
        valueText: curChar.currentShipTypeName + " [" + curChar.currentShipFriendlyName + "]"
    }

    TextKeyValue {
        id: charLocationText
        anchors.left: profilePic.right
        anchors.top: charActiveShipText.bottom
        keyText: qsTr("Location:")
        //valueText: "The Forge > Kimotoro > Jita (0.9)"
        valueText: curChar.currentRegionName + " > " +
                   curChar.currentConstellationName + " > " +
                   curChar.currentSolarSystemName + " (" +
                   curChar.currentSolarSystemSecurityStr + ")"
    }

    TextKeyValue {
        id: charDockedInText
        anchors.left: profilePic.right
        anchors.top: charLocationText.bottom
        // keyText: qsTr("Docked in:")
        keyText: curChar.isDocked ? qsTr("Docked in: ") : qsTr("Not docked")
        //valueText: "Jita IV - Moon IV - Caldari Navy Assembly Plant"
        valueText: curChar.isDocked ? curChar.currentStructureName : ""
    }
}
