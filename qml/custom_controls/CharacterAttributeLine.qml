import QtQuick 2.7
import QtQuick.Controls 2.2
import "../"


Item {
    id: container

    property int intelligence: 10
    property int memory: 10
    property int perception: 10
    property int willpower: 10
    property int charisma: 10

    property int iconWidth: 32

    Column {
        Image {
            id: imgInt
            source: "../../img/char_attributes/intelligence.png"
            width: iconWidth
            height: iconWidth
        }
        Image {
            id: imgMem
            source: "../../img/char_attributes/memory.png"
            width: iconWidth
            height: iconWidth
        }
        Image {
            id: imgPerc
            source: "../../img/char_attributes/perception.png"
            width: iconWidth
            height: iconWidth
        }
        Image {
            id: imgWillp
            source: "../../img/char_attributes/willpower.png"
            width: iconWidth
            height: iconWidth
        }
        Image {
            id: imgCharisma
            source: "../../img/char_attributes/charisma.png"
            width: iconWidth
            height: iconWidth
        }
    }
}
