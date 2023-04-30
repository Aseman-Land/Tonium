import QtQuick 2.15
import AsemanQml.Viewport 2.0
import TonToolkit 1.0
import "../components"
import "../globals"

TPage {
    id: page

    property alias backable: spage.backable
    property alias digitsCount: passField.digitsCount

    property string confirmMode

    SimplePageTemplate {
        id: spage
        anchors.fill: parent
        backable: true
        sticker: "qrc:/ton/common/stickers/Password.tgs"
        title: confirmMode.length? qsTr("Confirm a Passcode") : qsTr("Set a Passcode")
        body: qsTr("Enter the %1 digitis in the passcode.").arg(digitsCount)

        onCloseRequest: page.ViewportType.open = false

        secondaryButton {
            z: 20
            text: qsTr("Passcode options")
            onClicked: menu.open()
        }

        TPasswordField {
            id: passField
            width: 160
            onTextChanged: {
                if (text.length != digitsCount)
                    return;

                if (confirmMode.length) {
                    confirmItem = Viewport.viewport.append(doneComponent, {}, "stack")
                } else {
                    var cmp = Qt.createComponent("PasscodePage.qml");
                    confirmItem = Viewport.viewport.append(cmp, {"confirmMode": passField.text, "digitsCount": digitsCount}, "stack");
                }
            }

            property Item confirmItem
            onConfirmItemChanged: {
                if (!confirmItem) {
                    passField.text = "";
                    passField.focus = true;
                    passField.forceActiveFocus();
                }
            }

            Component.onCompleted: {
                focus = true;
                forceActiveFocus();
            }
        }
    }

    TMenu {
        id: menu
        y: -height
        width: spage.secondaryButton.width
        parent: spage.secondaryButton
        model: [qsTr("4-digit code"), qsTr("6-digit code")]
        onVisibleChanged: {
            if (!visible) {
                passField.focus = true;
                passField.forceActiveFocus();
            }
        }
        onItemClicked: {
            switch (index) {
            case 0:
                digitsCount = 4;
                break;
            case 1:
                digitsCount = 6;
                break;
            }
        }
    }

    Component {
        id: doneComponent
        DonePage {
            anchors.fill: parent
            backable: true
        }
    }
}
