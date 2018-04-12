import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "."
import "./custom_controls"
import "./pages"
import "./pages/char"


ApplicationWindow {
    id: mainWindow
    objectName: "mainWindow"
    visible: true
    width: 1024
    height: 768
    title: qsTr("QML EVEMon")

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            // main menu icon
            HamburgerIcon {
                id: mainHamburger
                color: "white"
                colorArrows: "black"
                iconState: "menu"
                onClicked: {
                    if (mainDrawer.visible) {
                        mainDrawer.close();
                    } else {
                        if (mainHamburger.iconState == "menu") {
                            mainDrawer.open();
                        } else {
                            nav_back();
                        }
                    }
                }
            }

            Text {
                id: navTitle
                text: qsTr("Overview")
                font.family: AppStyle.fontFamily
                font.pointSize: AppStyle.textSizeH3
            }

            CharacterPickerSidebar {
                id: toolbarCharacterPicker
                isVertical: false
                visible: evemonapp.isPortrait
                onCharacterSelected: {
                    nav_to("select_character", char_id);
                }
            }

            // spacer
            Item { Layout.fillWidth: true }

            Text {
                id: serverPlayersOnline
                text: "TQ: " + refresher.serverPlayersOnline + qsTr(" players")
                font.pointSize: AppStyle.textSizeH3
            }

            BusyIndicator {
                id: networkActivityIndicator
                implicitWidth: 48
                implicitHeight: 48
                running: refresher.networkActivity
            }
        }
    }

    Drawer {
        id: mainDrawer
        y: header.height
        width: Math.max(mainWindow.width * 0.5, 300)
        height: mainWindow.height
        edge: Qt.LeftEdge
        dragMargin: AppStyle.marginDrag
        // disable Drawer reacting to swipes on desktop platform
        interactive: !evemonapp.isDesktopPlatform

        onClosed: {
            // mainHamburger.iconState = "menu";
        }

        ListView {
            id: mainMenuView
            focus: true
            currentIndex: -1
            anchors.fill: parent

            signal mainMenuClicked(string action)

            delegate: ItemDelegate {
                width: parent.width
                text: model.title
                font.pointSize: AppStyle.textSizeH3
                highlighted: ListView.isCurrentItem
                onClicked: {
                    ListView.currentIndex = index
                    mainMenuView.mainMenuClicked(model.action);
                    mainDrawer.close()
                }
            }

            model: ListModel {
                id: mainMenuModel
                ListElement { title: qsTr("Add character"); action: "add_character" }
                ListElement { title: qsTr("About"); action: "about" }
                ListElement { title: qsTr("Test page"); action: "test_page" }
                ListElement { title: qsTr("Quit"); action: "quit" }
            }

            onMainMenuClicked: {
                menuAction(action);
            }
        }
    }

    Component {
        id: pageHome
        Home {
            onAddCharacterRequest: {
                menuAction("add_character")
            }
            onSelectCharacterRequest: {
                nav_to("select_character", characterId);
            }
            onRemoveCharacterRequest: {
                evemonapp.requestDeleteCharacter(characterId);
            }
        }
    }

    Component {
        id: pageAddCharacter
        AddCharacter {
            onLoginWithEveOnline: {
                eveSsoLoginManager.slotStartSsoAuth();
            }
            onLoginCancel: {
                eveSsoLoginManager.slotCancelSsoAuth();
                nav_back();
            }
            onLoginWasOk: {
                nav_back();
            }
        }
    }

    Component {
        id: pageAbout
        About { }
    }

    Component {
        id: pageTest
        TestPage { }
    }

    Component {
        id: pageCharacterMain
        CharMain {
            onSelectCharacterRequest: {
                nav_to("select_character", characterId);
            }
            onRemoveCharacterRequest: {
                evemonapp.requestDeleteCharacter(characterId);
                nav_back();
            }
        }
    }

    // Main content
    // stack view and navigation functions

    /*
      Array of navStates
      Possible values for navState:
      - "home" - home screen
      - "add_character" - add character screen
      - "select_character" - view character screen
      - "about" - About... window
    */
    property var navStack: ["home"]
    property var navTitleStack: [qsTr("Overview")]
    property string navState: "home"  // current state

    StackView {
        id: mainStack
        anchors.fill: parent
        initialItem: pageHome

        transform: Translate {
            x: mainDrawer.position * mainWindow.width * 0.33
        }
    }

    // menu actions handler
    function menuAction(action) {
        // console.log("Menu action clicked: " + action);
        if (action === "quit") {
            Qt.quit();
        } else {
            nav_to(action);
        }
    }

    // navigation functions:

    // pop last stack view, navigate back
    function nav_back() {
        // console.log("depth = " + mainStack.depth);
        // console.log(navStack, navState);
        if (mainStack.depth > 1) {
            mainStack.pop();
            navStack.pop();
            navTitleStack.pop();
            navState = navStack[navStack.length - 1];
            navTitle.text = navTitleStack[navTitleStack.length - 1]
            if (mainStack.depth == 1) {
                mainHamburger.iconState = "menu";
                evemonapp.setCurrentCharacter(0);
            }
        }
    }

    function nav_to(page, p1, p2, p3) {
        var handled = false;
        var nav_title = "";
        if (page === "add_character") {
            mainStack.push(pageAddCharacter);
            nav_title = qsTr("Add character")
            handled = true;
        } else if (page === "select_character") {
            var characterId = p1;
            // console.log("Select character: " + characterId);
            // already selected as current - do nothing
            if (evemonapp.curCharId() === characterId) {
                return;
            }
            // current page is viewing other character - navigate back first
            if (navState == "select_character") {
                nav_back();
            }
            evemonapp.setCurrentCharacter(characterId);
            mainStack.push(pageCharacterMain);
            nav_title = qsTr("Char: ") + curChar.characterName
            handled = true;
        } else if (page === "about") {
            nav_title = qsTr("About...")
            mainStack.push(pageAbout);
            handled = true;
        } else if (page === "test_page") {
            nav_title = qsTr("Test page")
            mainStack.push(pageTest);
            handled = true;
        }

        if (handled) {
            navStack.push(page);
            navState = page;
            navTitleStack.push(nav_title)
            navTitle.text = nav_title
            mainHamburger.iconState = "back";
            // console.log(navStack, navState);
        } else {
            console.warn("Unhandled nav page: " + page);
        }
    }
}
