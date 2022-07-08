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
import org.kde.plasma.remotecontrollers 1.0 as PlasmaRemoteControllers
import "delegates" as Delegates

KCM.ScrollViewKCM {
    id: root

    title: i18n("Remote Controllers")
    KCM.ConfigModule.quickHelp: i18n("This module lets you configure remote controllers.")

    Component.onCompleted: {
        connectionView.forceActiveFocus();
    }

    // ListModel {
    //     id: supportedControllers
    //     ListElement { display: "HDMI-CEC"; iconName: "input-dialpad"; toolTip: "TV Remote Controller"}
    // }

    PlasmaRemoteControllers.DevicesModel {
        id: devicesModel
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
                model:  devicesModel
                Layout.alignment: Qt.AlignTop
                Layout.preferredHeight: Kirigami.Units.gridUnit * 4
                Kirigami.FormData.label: i18n("Device:")
                textRole: "display"
                delegate: Delegates.DeviceDelegate{}
                Behavior on x {
                    NumberAnimation {
                        duration: Kirigami.Units.longDuration * 2
                        easing.type: Easing.InOutQuad
                    }
                }

                Component.onCompleted: {
                    currentIndex = 0
                }

                onCurrentIndexChanged: {
                    deviceSetupView.currentDevice = {
                        display: devicesModel.get(currentIndex).deviceName,
                        iconName: devicesModel.get(currentIndex).deviceIconName,
                        toolTip: devicesModel.get(currentIndex).deviceType
                    }
                }
            }
        }

        DeviceSetupView {
            id: deviceSetupView
            visible: true //supportedControllers.count > 0 ? 1 : 0
            Layout.fillWidth: true                
            Layout.fillHeight: true
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
            //keySetupPopUp.close()
        }

        onOpened: {
            var getCecKey = kcm.getCecKeyFromRemotePress()
            keyCodeRecieved(getCecKey)
        }

        contentItem: Item {
            anchors.fill: parent

            PlasmaComponents.Label {
                anchors.centerIn: parent
                text: "Select Key On Your TV Remote For " + keySetupPopUp.keyType[0]
            }
        }
    }
}
