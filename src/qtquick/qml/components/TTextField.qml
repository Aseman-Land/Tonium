import QtQuick 2.15
import AsemanQml.Base 2.0
import AsemanQml.Modern 2.0
import TonToolkit.private 1.0
import "../globals"

TControlElement {
    id: dis
    scale: marea.pressed? 0.97 : 1
    height: Constants.itemsHeight

    property alias font: input.font
    property alias color: input.color
    property alias text: input.text

    property real leftPadding
    property real rightPadding
    property real topPadding
    property real bottomPadding

    onFocusChanged: {
        if (focus) {
            input.focus = true;
            input.forceActiveFocus();
        }
    }

    TextInput {
        id: input
        anchors.fill: parent
        anchors.leftMargin: dis.leftPadding
        anchors.rightMargin: dis.rightPadding
        anchors.topMargin: dis.topPadding
        anchors.bottomMargin: dis.bottomPadding
        font.family: Fonts.globalFont
        font.pixelSize: Fonts.globalFontSize
        selectByMouse: true
        selectedTextColor: "#fff"
        selectionColor: Colors.accent
        color: Colors.foreground
        leftPadding: 2
        rightPadding: 2
        bottomPadding: 10
        topPadding: 8
        clip: true

        onFocusChanged: checkMenu()
        onTextChanged: checkMenu()

        Keys.onTabPressed: dis.tabPressed()
        Keys.onDownPressed: if (suggestionsMenu) suggestionsMenu.list.currentIndex = Math.min(suggestionsMenu.list.count-1, suggestionsMenu.list.currentIndex+1)
        Keys.onUpPressed: if (suggestionsMenu) suggestionsMenu.list.currentIndex = Math.max(0, suggestionsMenu.list.currentIndex-1)
        Keys.onReturnPressed: if (suggestionsMenu) suggestionsMenu.list.currentItem.select()
        Keys.onEnterPressed: if (suggestionsMenu) suggestionsMenu.list.currentItem.select()

        function checkMenu() {
            if (singalBlocker)
                return;
            if (focus && suggestions.length && text.length) {
                if (suggestionsMenu)
                    return;

                suggestionsMenu = suggestionsMenu_component.createObject(dis);
            } else if (suggestionsMenu) {
                suggestionsMenu.destroy();
            }
        }

        property bool singalBlocker
    }

    Connections {
        target: GlobalSignals
        function onDiscardMenus() {
            if (suggestionsMenu)
                suggestionsMenu.destroy();
        }
    }

    property variant suggestionsMenu
    property variant suggestions: new Array

    Behavior on scale {
        NumberAnimation { easing.type: Easing.OutCubic; duration: 200 }
    }

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 2

        Rectangle {
            anchors.fill: parent
            color: Colors.foreground
            opacity: 0.1
        }

        Rectangle {
            anchors.centerIn: parent
            height: parent.height
            width: input.focus || marea.pressed? parent.width : 0
            radius: height/2
            color: Colors.accent

            Behavior on width {
                NumberAnimation { easing.type: Easing.OutCubic; duration: 200 }
            }
        }
    }

    MouseArea {
        id: marea
        hoverEnabled: true
        acceptedButtons: Qt.RightButton
        cursorShape: Qt.IBeamCursor
        anchors.fill: parent
        onClicked: {
            input.focus = true;
            input.forceActiveFocus();

            if (Devices.isDesktop) {
                menu.x = mouseX;
                menu.y = mouseY;
                menu.open();
            }
        }
    }

    QmlWidgetMenu {
        id: menu

        QmlWidgetMenuItem {
            text: qsTr("Select All") + Translations.refresher
            shortcut: "Ctrl+A"
            onClicked: input.selectAll()
        }
        QmlWidgetMenuItem {
            text: qsTr("Deselect") + Translations.refresher
            onClicked: input.deselect()
        }
        QmlWidgetMenuItem{}
        QmlWidgetMenuItem {
            text: qsTr("Copy") + Translations.refresher
            shortcut: "Ctrl+C"
            onClicked: input.copy()
        }
        QmlWidgetMenuItem {
            text: qsTr("Cut") + Translations.refresher
            shortcut: "Ctrl+X"
            onClicked: input.cut()
        }
        QmlWidgetMenuItem {
            text: qsTr("Paste") + Translations.refresher
            shortcut: "Ctrl+V"
            onClicked: input.paste()
            enabled: input.canPaste
        }
        QmlWidgetMenuItem{}
        QmlWidgetMenuItem {
            text: qsTr("Undo") + Translations.refresher
            shortcut: "Ctrl+Z"
            onClicked: input.cut()
            enabled: input.canUndo
        }
        QmlWidgetMenuItem {
            text: qsTr("Redo") + Translations.refresher
            shortcut: "Ctrl+Shift+Z"
            onClicked: input.paste()
            enabled: input.canRedo
        }
        QmlWidgetMenuItem{}
        QmlWidgetMenuItem {
            text: qsTr("Delete") + Translations.refresher
            shortcut: "Delete"
            enabled: input.selectionStart && input.selectionStart != input.selectionEnd
            onClicked: input.remove(input.selectionStart, input.selectionEnd)
        }
    }

    Component {
        id: suggestionsMenu_component

        Item {
            id: suggestionsMenuItem
            x: 0
            y: dis.height
            width: dis.width
            height: Math.min(suggestionsList.count * Constants.itemsHeight, 100)
            visible: suggestionsList.count > 0

            property alias list: suggestionsList

            Component.onCompleted: {
                BackHandler.pushHandler(suggestionsMenuItem, suggestionsMenuItem.destroy);
            }

            FastDropShadow {
                anchors.fill: parent
                anchors.margins: 1
                radius: 5
                color: Colors.foreground
                source: suggestionsMenuBack
            }

            Rectangle {
                id: suggestionsMenuBack
                anchors.fill: parent
                radius: Constants.controlsRoundness
                color: Colors.background

                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.verticalCenter
                    color: Colors.background
                }
            }

            ListView {
                id: suggestionsList
                anchors.fill: parent
                model: {
                    var res = new Array;
                    suggestions.forEach(function(f){
                        if (f.toLowerCase().indexOf(dis.text.toLowerCase()) == 0)
                            res[res.length] = f;
                    });
                    suggestions.forEach(function(f){
                        if (f.toLowerCase().indexOf(dis.text.toLowerCase()) > 0)
                            res[res.length] = f;
                    });
                    return res;
                }

                clip: true

                highlightMoveDuration: 200
                highlightRangeMode: ListView.StrictlyEnforceRange
                preferredHighlightBegin: 0
                preferredHighlightEnd: height
                highlight: Rectangle {
                    width: suggestionsList.width
                    height: Constants.itemsHeight
                    opacity: 0.1
                    color: Colors.foreground
                }

                delegate: TItemDelegate {
                    width: suggestionsList.width
                    height: Constants.itemsHeight
                    onClicked: select()

                    function select() {
                        suggestionsMenuItem.destroy();
                        input.text = modelData;
                    }

                    TLabel {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 10 * Devices.density
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        text: modelData
                    }
                }
            }
        }
    }
}
