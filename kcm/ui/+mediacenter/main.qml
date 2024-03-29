/*
    SPDX-FileCopyrightText: 2020 Aditya Mehra <aix.m@outlook.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

*/

import QtQuick.Layouts 1.14
import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kcmutils as KCM
import org.kde.mycroft.bigscreen 1.0 as BigScreen
import org.kde.private.kcm.remotecontrollers 1.0
import "+mediacenter/delegates" as Delegates

KCM.SimpleKCM {
    id: root

    title: i18n("Remote Controllers")
    background: null
    leftPadding: Kirigami.Units.smallSpacing
    topPadding: 0
    rightPadding: Kirigami.Units.smallSpacing
    bottomPadding: 0
    property var supportedControllers: kcm.devicesModel

    Component.onCompleted: {
        connectionView.forceActiveFocus();
    }

    Connections {
        target: kcm.devicesModel
        onDevicesChanged: {
            if(connectionView.count > 0) {
                deviceSetupView.currentDevice = connectionView.currentItem
                deviceSetupView.deviceType = connectionView.currentItem.deviceType
            }
        }
    }

    contentItem: FocusScope {

        Rectangle {
            id: headerAreaTop
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: -Kirigami.Units.largeSpacing
            anchors.rightMargin: -Kirigami.Units.largeSpacing
            height: parent.height * 0.075
            z: 10
            gradient: Gradient {
                GradientStop { position: 0.1; color: Qt.rgba(0, 0, 0, 0.5) }
                GradientStop { position: 0.9; color: Qt.rgba(0, 0, 0, 0.25) }
            }

            Kirigami.Heading {
                level: 1
                anchors.fill: parent
                anchors.topMargin: Kirigami.Units.largeSpacing
                anchors.leftMargin: Kirigami.Units.largeSpacing * 2
                anchors.bottomMargin: Kirigami.Units.largeSpacing
                color: Kirigami.Theme.textColor
                text: i18n("Remote Controllers")
            }
        }

        Item {
            id: footerMain
            anchors.left: parent.left
            anchors.right: deviceSetupView.left
            anchors.leftMargin: -Kirigami.Units.largeSpacing
            anchors.bottom: parent.bottom
            implicitHeight: Kirigami.Units.gridUnit * 2

            Button {
                id: kcmcloseButton
                implicitHeight: Kirigami.Units.gridUnit * 2
                width: supportedControllers.count > 0 ? parent.width : (root.width + Kirigami.Units.largeSpacing)

                background: Rectangle {
                    color: kcmcloseButton.activeFocus ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor
                }

                contentItem: Item {
                    RowLayout {
                        anchors.centerIn: parent
                        Kirigami.Icon {
                            Layout.preferredWidth: Kirigami.Units.iconSizes.small
                            Layout.preferredHeight: Kirigami.Units.iconSizes.small
                            source: "window-close"
                        }
                        Label {
                            text: i18n("Exit")
                        }
                    }
                }

                Keys.onUpPressed: connectionView.forceActiveFocus()

                onClicked: {
                    Window.window.close()
                }

                Keys.onReturnPressed: {
                    Window.window.close()
                }
            }
        }

        Item {
            clip: true
            anchors.left: parent.left
            anchors.top: headerAreaTop.bottom
            anchors.bottom: footerMain.top
            width: parent.width - deviceSetupView.width

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: Kirigami.Units.largeSpacing
                anchors.topMargin: Kirigami.Units.largeSpacing * 2

                BigScreen.TileView {
                    id: connectionView
                    focus: true
                    model:  supportedControllers
                    Layout.alignment: Qt.AlignTop
                    cellWidth: (Kirigami.Units.iconSizes.huge + Kirigami.Units.largeSpacing*6)
                    title: supportedControllers.count > 0 ? i18n("Found Devices") : i18n("No Devices Found")
                    currentIndex: 0
                    delegate: Delegates.DeviceDelegate{}
                    navigationDown: kcmcloseButton
                    Behavior on x {
                        NumberAnimation {
                            duration: Kirigami.Units.longDuration * 2
                            easing.type: Easing.InOutQuad
                        }
                    }
                    
                    onCurrentItemChanged: {
                        deviceSetupView.currentDevice = currentItem.device
                        deviceSetupView.deviceType = currentItem.deviceType
                    }
                }
            }
        }

        DeviceSetupView {
            id: deviceSetupView
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            visible: supportedControllers.count > 0 ? 1 : 0
            anchors.rightMargin: -Kirigami.Units.smallSpacing
            width: Kirigami.Units.gridUnit * 15
        }

        Popup {
            id: keySetupPopUp
            x: ((parent.width - deviceSetupView.width) - width)  / 2
            y: (parent.height - height) / 2
            width: parent.width * 0.70
            height: parent.height * 0.10
            property var keyType

            function keyCodeRecieved(keyCode) {
                kcm.setCecKeyConfig(keyType[1], keyCode)
                keySetupPopUp.close()
            }

            onOpened: {
                var getCecKey = kcm.cecKeyFromRemotePress()
                keyCodeRecieved(getCecKey)
            }

            contentItem: Item {
                anchors.fill: parent

                PlasmaComponents.Label {
                    anchors.centerIn: parent
                    text: i18n("Select Key On Your TV Remote For %1", (keySetupPopUp.keyType ? keySetupPopUp.keyType[0] : ""))
                }
            }
        }

        Popup {
            id: keySetupGamepadPopUp
            x: (parent.width - width)  / 2
            y: (parent.height - height) / 2
            width: parent.width * 0.70
            height: parent.height * 0.10
            property var keyType

            Connections {
                target: kcm
                onGamepadKeyPressed: {
                    if(keySetupGamepadPopUp.opened) {
                        if(kcm.gamepadKeyConfig("ButtonEnter") == keyCode) {
                            deviceSetupView.ignoreEvent = true       
                        }
                        kcm.setGamepadKeyConfig(keySetupGamepadPopUp.keyType[1], keyCode)
                        keySetupGamepadPopUp.close()
                    }
                }
            }

            contentItem: Item {
                anchors.fill: parent

                PlasmaComponents.Label {
                    anchors.centerIn: parent
                    text: i18n("Select Key On Your Gamepad For %1", (keySetupGamepadPopUp.keyType ? keySetupGamepadPopUp.keyType[0] : ""))
                }
            }
        }
    }
}
