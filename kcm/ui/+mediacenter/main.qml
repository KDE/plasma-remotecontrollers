/*
    SPDX-FileCopyrightText: 2020 Aditya Mehra <aix.m@outlook.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents
import org.kde.kcmutils as KCM
import org.kde.bigscreen as BigScreen
import org.kde.private.kcm.remotecontrollers 1.0

import "+mediacenter/delegates" as Delegates

KCM.SimpleKCM {
    id: root

    title: i18n("Remote Controllers")
    background: null

    leftPadding: Kirigami.Units.smallSpacing
    topPadding: Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.smallSpacing

    property var supportedControllers: kcm.devicesModel

    onActiveFocusChanged: {
        if (activeFocus) {
            connectionView.forceActiveFocus();
        }
    }

    Connections {
        target: kcm.devicesModel
        onDevicesChanged: {
            if (connectionView.count > 0) {
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
            clip: true
            anchors.left: parent.left
            anchors.top: headerAreaTop.bottom
            anchors.bottom: parent.bottom
            width: parent.width - deviceSetupView.width

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: Kirigami.Units.largeSpacing
                anchors.topMargin: Kirigami.Units.largeSpacing * 2

                BigScreen.TileView {
                    id: connectionView
                    focus: true
                    model: supportedControllers
                    Layout.alignment: Qt.AlignTop
                    cellWidth: (Kirigami.Units.iconSizes.huge + Kirigami.Units.largeSpacing*6)
                    title: supportedControllers.count > 0 ? i18n("Found Devices") : i18n("No Devices Found")
                    currentIndex: 0
                    delegate: Delegates.DeviceDelegate {}
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
                    if (keySetupGamepadPopUp.opened) {
                        if (kcm.gamepadKeyConfig("ButtonEnter") == keyCode) {
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
