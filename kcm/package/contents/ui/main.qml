/*
    SPDX-FileCopyrightText: 2020 Aditya Mehra <aix.m@outlook.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

*/

import QtQuick.Layouts 1.14
import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kirigami 2.12 as Kirigami
import org.kde.kcm 1.2 as KCM
import org.kde.private.kcm.remotecontrollers 1.0
import "delegates" as Delegates

KCM.ScrollViewKCM {
    id: root
    title: i18n("Remote Controllers")
    KCM.ConfigModule.quickHelp: i18n("This module lets you configure remote controllers.")

    Component.onCompleted: {
        connectionView.forceActiveFocus();
    }

    function listProperty(item)
    {
        for (var p in item)
        console.log(p + ": " + item[p]);
    }

    
    Connections {
        target: kcm.devicesModel

        onDevicesChanged: {
            if(connectionView.count > 0) {
                deviceSetupView.deviceName = connectionView.model.get(connectionView.currentIndex).deviceName
                deviceSetupView.deviceIcon = connectionView.model.get(connectionView.currentIndex).deviceIconName
                deviceSetupView.deviceType = connectionView.model.get(connectionView.currentIndex).deviceType
            }
        }
    }

    ColumnLayout {
        width: parent.width
        height: parent.height

        Kirigami.FormLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: connectionView.implicitHeight
            
            ComboBox {
                id: connectionView
                focus: true
                model:  kcm.devicesModel
                Layout.alignment: Qt.AlignTop
                Layout.preferredHeight: Kirigami.Units.gridUnit * 4
                Kirigami.FormData.label: i18n("Device:")
                textRole: "deviceName"
                visible: connectionView.count > 0 ? 1 : 0
                delegate: Delegates.DeviceDelegate{}

                Keys.onDownPressed: {
                    if(!connectionView.popup.opened) {
                        deviceSetupView.forceActiveFocus();
                    }
                }

                Behavior on x {
                    NumberAnimation {
                        duration: Kirigami.Units.longDuration * 2
                        easing.type: Easing.InOutQuad
                    }
                }

                Component.onCompleted: connectionView.currentIndex = 0
                onCountChanged: connectionView.currentIndex = 0
                onCurrentIndexChanged: {
                    if(connectionView.currentIndex > -1) {
                        deviceSetupView.deviceName = connectionView.model.get(connectionView.currentIndex).deviceName
                        deviceSetupView.deviceIcon = connectionView.model.get(connectionView.currentIndex).deviceIconName
                        deviceSetupView.deviceType = connectionView.model.get(connectionView.currentIndex).deviceType
                    }
                }
            }
        }

        DeviceSetupView {
            id: deviceSetupView
            visible: connectionView.count > 0 ? 1 : 0
            Layout.fillWidth: true                
            Layout.fillHeight: true
        }

        Item {
            id: noViewAvailable
            visible: connectionView.count == 0 ? 1 : 0

            Kirigami.Heading {
                text: i18n("No remote controllers available.")
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                level: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    Popup {
        id: keySetupPopUp
        x: (parent.width - width)  / 2
        y: (parent.height - height) / 2
        width: parent.width * 0.70
        height: parent.height * 0.10
        property var keyType

        function keyCodeRecieved(keyCode) {
            kcm.setCecKeyConfig(keyType[1], keyCode)
            keySetupPopUp.close()
        }

        onOpened: {
            kcm.setNoop()
            var getCecKey = kcm.cecKeyFromRemotePress()
            keyCodeRecieved(getCecKey)
        }

        onClosed: {
            kcm.releaseNoop()
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

        onOpened: {
            kcm.setNoop()
        }

        onClosed: {
            kcm.releaseNoop()
        }

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
