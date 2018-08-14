import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
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

    header: HeaderToolbar {
        id: headerToolbar
        onHamburgerClicked: {
            // onHamburgerClicked(string iconState)
            // state of hamburger arrow icon: "menu" / "back"
            if (mainDrawer.visible) {
                mainDrawer.close();
            } else {
                if (iconState === "menu") {
                    mainDrawer.open();
                } else {
                    nav_back();
                }
            }
        }

        onCharacterSelected: {
            nav_to("select_character", char_id);
        }
    }

    footer: InlineMessage {
        id: footerMessage
        visible: false
        showCloseButton: true
        messageFontPointSize: evemonapp.isDesktopPlatform ? AppStyle.textSizeH3 : AppStyle.textSizeH2
        closeButtonSize: evemonapp.isDesktopPlatform ? 17 : 20
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
                font.pointSize: evemonapp.isDesktopPlatform ? AppStyle.textSizeH3 : AppStyle.textSizeH2
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
                ListElement { title: qsTr("Settings"); action: "settings" }
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
        id: pageSettings
        Settings { }
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

            onRequestOpenMail: {
                nav_to("mail_view", mailId);
            }
        }
    }

    Component {
        id: pageMailView
        MailView {
            onRequestSetPageTitle: {
                headerToolbar.navTitleText = title;
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
      - "mail_view" - view character mail body
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
            headerToolbar.navTitleText = navTitleStack[navTitleStack.length - 1]
            if (mainStack.depth == 1) {
                headerToolbar.iconState = "menu";
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
            if (evemonapp.curCharId === characterId) {
                return;
            }
            // current page is viewing other character
            if (navState == "select_character" || navState =="mail_view") {
                if (navState =="mail_view") {
                    nav_back(); // extra step back
                }
                nav_back();
                evemonapp.setCurrentCharacter(characterId);
                mainStack.push(pageCharacterMain)
            } else {
                evemonapp.setCurrentCharacter(characterId);
                mainStack.push(pageCharacterMain);
            }
            nav_title = qsTr("Char: ") + curChar.characterName
            handled = true;
        } else if (page === "mail_view") {
            var mailId = p1;
            evemonapp.requestOpenMail(mailId);
            nav_title = qsTr("Mail");
            mainStack.push(pageMailView);
            handled = true;
        } else if (page === "settings") {
            nav_title = qsTr("Settings")
            mainStack.push(pageSettings);
            handled = true;
        } else if (page === "about") {
            nav_title = qsTr("About QMLEveMon")
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
            headerToolbar.navTitleText = nav_title
            headerToolbar.iconState = "back";
            // console.log(navStack, navState);
        } else {
            console.warn("Unhandled nav page: " + page);
        }
    }

    Connections {
        target: evemonapp
        onMessagePopup: {
            footerMessage.addMessage(type, message);
        }
    }

    onClosing: {
        console.log("Application window is closing! close.accepted = ", close.accepted)
    }

//    onDestroyed: {
//        console.log("Application window is destroyed!")
//    }

    onVisibleChanged: {
        console.log("Application window is visible:", visible)
    }

    onVisibilityChanged: {
        console.log("Application window onVisibilityChanged:", visibility);
        if (evemonapp.isFlatpak) {
            // when window is closed under flatpak - it is like hidden. But we need to quit
            if (visibility === Window.Hidden) {
                Qt.quit();
            }
        }
    }
}
