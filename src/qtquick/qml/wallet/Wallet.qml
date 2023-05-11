import QtQuick 2.15
import Toolkit.Core 1.0
import Toolkit.Viewport 1.0
import Wallet.Core 1.0
import "../transfer" as Transfer
import "../settings" as Settings
import "../connect" as Connect
import "../components"
import "../globals"

TPage {
    id: page
    color: "#000"

    readonly property real headerHeight: 240
    readonly property real headerRatio: Math.max(0, (headerHeight + mapListener.result.y)/headerHeight)

    property string balanceUSD: "89.6"

    property alias publicKey: wallet.publicKey
    readonly property bool loading: wallet.loading || walletState.loading || tmodel.refreshing

    WalletItem {
        id: wallet
        backend: MainBackend
        onAddressChanged: GlobalValues.address = address;
    }
    WalletState {
        id: walletState
        backend: MainBackend
        address: wallet.address
        onBalanceChanged: GlobalValues.balance = (balance.length? balance : "0.00000");
        onErrorStringChanged: if (errorString.length) GlobalSignals.snackRequest(MaterialIcons.mdi_alert_octagon, qsTr("Faild to load state"), errorString, Colors.foreground)
    }

    PointMapListener {
        id: mapListener
        source: listv.headerItem
        dest: walletScene
    }

    Item {
        id: walletScene
        anchors.fill: parent
        anchors.topMargin: Devices.statusBarHeight
        clip: true

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: headerArea.bottom
            anchors.bottom: parent.bottom
            radius: Constants.roundness
            color: Colors.background

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.verticalCenter
                anchors.bottom: parent.bottom
                color: Colors.background
            }

            Loader {
                anchors.centerIn: parent
                width: 120
                height: width
                active: page.loading && walletState.lastTransactionId == 0
                sourceComponent: StickerItem {
                    anchors.fill: parent
                    autoPlay: true
                    source: "qrc:/ton/common/stickers/Loading.tgs"
                }
            }

            Loader {
                anchors.fill: parent
                active: !page.loading && walletState.lastTransactionId == 0
                sourceComponent: EmptyWalletElement {
                    anchors.fill: parent
                    anchors.margins: 20
                    address: wallet.address
                }
            }
        }

        Item {
            anchors.fill: parent
            anchors.topMargin: Devices.standardTitleBarHeight
            clip: true

            TScrollView {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: walletScene.height
                clip: true

                TransactionsList {
                    id: listv
                    anchors.fill: parent
                    model: TransactionsModel {
                        id: tmodel
                        backend: MainBackend
                        publicKey: page.publicKey
                        offsetTransactionHash: walletState.lastTransactionHash
                        offsetTransactionId: walletState.lastTransactionId
                    }
                    header: Item {
                        width: listv.width
                        height: headerHeight
                    }
                }
            }
        }

        Item {
            id: headerArea
            width: parent.width
            height: Math.max(Devices.standardTitleBarHeight, mapListener.result.y + headerHeight)

            TRow {
                id: connectionRow
                anchors.horizontalCenter: parent.horizontalCenter
                opacity: page.loading? 1 : 0
                y: (opacity - 1) * height + 8
                visible: opacity > 0
                spacing: 4

                Behavior on opacity {
                    NumberAnimation { easing.type: Easing.OutCubic; duration: 250 }
                }

                TBusyIndicator {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -2
                    width: 12
                    height: width
                    running: connectionRow.visible
                    accented: false
                }

                TLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 7 * Devices.fontDensity
                    color: "#fff"
                    text: qsTr("Connecting...")
                }
            }

            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                transform: Item.Top
                height: headerHeight - Devices.standardTitleBarHeight
                scale: 0.8 + 0.2 * Math.min(1, headerRatio)
                opacity: scale

                TColumn {
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: -54 * Devices.density
                    spacing: 0

                    TWalletAddress {
                        anchors.horizontalCenter: parent.horizontalCenter
                        highlightColor: "#fff"
                        color: "#fff"
                        address: wallet.address
                        onClicked: {
                            Devices.setClipboard(address);
                            GlobalSignals.snackRequest(MaterialIcons.mdi_check, qsTr("Copy"), qsTr("Address copied to clipboard successfully."), Colors.green);
                        }
                    }

                    TRow {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 0

                        StickerItem {
                            anchors.verticalCenter: parent.verticalCenter
                            width: 34
                            height: width
                            autoPlay: true
                            source: "qrc:/ton/common/stickers/Main.tgs"
                        }

                        TRow {
                            spacing: 2

                            TLabel {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 4
                                font.pixelSize: 16 * Devices.fontDensity
                                color: "#fff"
                                font.weight: Font.Medium
                                text: {
                                    var b = GlobalValues.balance;
                                    var idx = b.indexOf(".");
                                    if (idx < 0)
                                        return b;
                                    return b.slice(0, idx);
                                }
                            }

                            TLabel {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 6
                                font.pixelSize: 12 * Devices.fontDensity
                                color: "#fff"
                                visible: text.length
                                text: {
                                    var b = GlobalValues.balance;
                                    var idx = b.indexOf(".");
                                    if (idx < 0)
                                        return "";
                                    return b.slice(idx);
                                }
                            }
                        }
                    }
                }

                TButton {
                    anchors.left: parent.left
                    anchors.right: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    anchors.rightMargin: 6
                    text: qsTr("Receive")
                    icon.text: MaterialIcons.mdi_arrow_bottom_left
                    icon.font.pixelSize: 12 * Devices.fontDensity
                    onClicked: TViewport.viewport.append(receive_component, {}, "drawer")
                }

                TButton {
                    anchors.left: parent.horizontalCenter
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    anchors.leftMargin: 6
                    text: qsTr("Send")
                    icon.text: MaterialIcons.mdi_arrow_top_right
                    icon.font.pixelSize: 12 * Devices.fontDensity
                    onClicked: TViewport.viewport.append(send_component, {}, "drawer")
                }
            }

            Rectangle {
                anchors.fill: parent
                opacity: 1 - Math.pow(Math.min(1, headerRatio), 2)
                gradient: Gradient {
                    GradientStop { position: 0.0; color: page.color }
                    GradientStop { position: 0.7; color: page.color }
                    GradientStop { position: 1.0; color: "transparent" }
                }
            }

            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: Devices.standardTitleBarHeight

                TColumn {
                    anchors.centerIn: parent
                    spacing: 0
                    opacity: 1 - Math.min(1, headerRatio)

                    TRow {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 0

                        StickerItem {
                            anchors.verticalCenter: parent.verticalCenter
                            width: 18
                            height: width
                            autoPlay: true
                            source: "qrc:/ton/common/stickers/Main.tgs"
                            loops: 0
                        }

                        TLabel {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.verticalCenterOffset: 2
                            font.pixelSize: 9 * Devices.fontDensity
                            text: GlobalValues.balance
                            color: "#fff"
                        }
                    }

                    TLabel {
                        anchors.horizontalCenter: parent.horizontalCenter
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        maximumLineCount: 1
                        elide: Text.ElideRight
                        color: "#fff"
                        text: "≈ $" + balanceUSD
                        font.pixelSize: 7 * Devices.fontDensity
                        opacity: 0.6
                    }
                }

                TButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 6 * Devices.density
                    flat: true
                    width: height
                    icon.text: MaterialIcons.mdi_settings_outline
                    icon.font.pixelSize: 15 * Devices.fontDensity
                    highlightColor: "#fff"
                    onClicked: TViewport.viewport.append(settings_component, {}, "activity")
                }
            }
        }
    }

    Component {
        id: settings_component
        Settings.SettingsPage {
        }
    }

    Component {
        id: connect_component
        Connect.ConnectPage {
            width: page.width
            closable: true
            onCloseRequest: ViewportType.open = false
        }
    }

    Component {
        id: receive_component
        Transfer.ReceiveDialog {
            width: page.width
            closable: true
            address: wallet.address
            onCloseRequest: ViewportType.open = false
        }
    }

    Component {
        id: send_component
        Transfer.SendCustomDialog {
            width: page.width
            closable: true
            onCloseRequest: ViewportType.open = false
        }
    }
}
