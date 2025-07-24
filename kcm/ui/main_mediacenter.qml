/*
    SPDX-FileCopyrightText: 2020 Aditya Mehra <aix.m@outlook.com>
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents
import org.kde.kcmutils as KCM
import org.kde.bigscreen as Bigscreen
import org.kde.private.kcm.remotecontrollers 1.0

import './mediacenter'

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

    contentItem: ColumnLayout {
        spacing: 0

        QQC2.Label {
            text: i18n('Controllers')
            font.pixelSize: 22
            font.weight: Font.Normal
            Layout.topMargin: Kirigami.Units.gridUnit
            Layout.bottomMargin: Kirigami.Units.gridUnit
        }

        ListView {
            id: connectionView
            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true
            model: supportedControllers

            delegate: Bigscreen.ButtonDelegate {
                property QtObject device: model
                property int deviceType: model.deviceType

                raisedBackground: false
                width: parent.width
                text: model.deviceName
                icon.name: model.deviceIconName

                onClicked: {
                    deviceSetupSidebar.currentDevice = device;
                    deviceSetupSidebar.deviceType = deviceType;
                    deviceSetupSidebar.open();
                }
            }

            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent
                text: i18n("No remote controllers available.")
                visible: connectionView.count == 0 ? 1 : 0
                width: parent.width - (Kirigami.Units.largeSpacing * 8)
            }
        }

        DeviceSetupSidebar {
            id: deviceSetupSidebar
            onClosed: connectionView.forceActiveFocus()

            // Close sidebar if device is null
            onCurrentDeviceChanged: {
                if (currentDevice === null) {
                    close();
                }
            }
        }
    }
}
