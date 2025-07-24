/*
    SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2020 Aditya Mehra <aix.m@outlook.com>
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.extras as PlasmaExtras
import org.kde.bigscreen as Bigscreen
import org.kde.kitemmodels as KItemModels

import org.kde.private.kcm.remotecontrollers 1.0

Bigscreen.SidebarOverlay {
    id: root

    openFocusItem: buttonMapListView

    property QtObject currentDevice
    property int deviceType

    // Refresh model when controller selected
    onAboutToShow: buttonMapListView.model.invalidateFilter()

    header: Bigscreen.SidebarOverlayHeader {
        iconSource: currentDevice ? currentDevice.deviceIconName : ''
        title: currentDevice ? currentDevice.deviceName : ''
        subtitle: {
            switch (root.deviceType) {
                case 0:
                    return i18n("Bind TV Remote Keys To Buttons");
                case 1:
                    return i18n("Bind Wii Remote Keys To Buttons");
                case 2:
                    return i18n("Bind Gamepad Keys To Buttons");
            }
            return '';
        }
    }

    content: ListView {
        id: buttonMapListView
        spacing: 0
        clip: true

        Keys.onLeftPressed: root.close();

        model: KItemModels.KSortFilterProxyModel {
            sourceModel: kcm.keyMapModel
            filterRoleName: "DeviceTypeRole"
            filterRowCallback: function(source_row, source_parent) {
                var filter_device = root.deviceType == 0 ? "CEC" : "GAMEPAD";
                return sourceModel.data(sourceModel.index(source_row, 0, source_parent), KeyMapModel.DeviceTypeRole).indexOf(filter_device) !== -1;
            }
        }

        delegate: Bigscreen.ButtonDelegate {
            text: i18n('Action: %1', model.buttonDisplay)
            description: i18n('Input: %1', model.assignedKeyDisplay)
            icon.name: model.assignedKeyIcon

            width: buttonMapListView.width

            onClicked: {
                if (model.deviceType == "GAMEPAD") {
                    keySetupGamepadPopUp.keyType = [model.buttonDisplay, model.buttonType]
                    keySetupGamepadPopUp.open()
                }
                if (model.deviceType == "CEC") {
                    keySetupPopUp.keyType = [model.buttonDisplay, model.buttonType]
                    keySetupPopUp.open()
                }
            }
        }

        Bigscreen.Dialog {
            id: keySetupPopUp

            property var keyType

            title: i18n("Select button mapping for %1", (keySetupPopUp.keyType ? keySetupPopUp.keyType[0] : ""))

            onClosed: buttonMapListView.forceActiveFocus();

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

                QQC2.Label {
                    text: i18n('Waiting for key press from your remote…')
                    anchors.centerIn: parent
                    font.pixelSize: Bigscreen.Units.defaultFontPixelSize
                }
            }
        }

        Bigscreen.Dialog {
            id: keySetupGamepadPopUp
            standardButtons: selectedKeyCode === -1 ? Bigscreen.Dialog.Cancel : (Bigscreen.Dialog.Ok | Bigscreen.Dialog.Cancel)

            property var keyType
            property bool firstPress: true

            property int selectedKeyCode: -1

            title: i18n("Select button mapping for %1", (keySetupGamepadPopUp.keyType ? keySetupGamepadPopUp.keyType[0] : ""))

            onAboutToShow: {
                // Reset dialog
                firstPress = true;
                selectedKeyCode = -1;
            }

            onClosed: buttonMapListView.forceActiveFocus();
            onAccepted: {
                kcm.setGamepadKeyConfig(keySetupGamepadPopUp.keyType[1], selectedKeyCode);
                close();
            }

            Connections {
                target: kcm

                // Listen for gamepad key press to bind control
                onGamepadKeyPressed: (keyCode) => {
                    if (keySetupGamepadPopUp.opened) {
                        // Ignore first event from the controller if it initiated the press, or if key is already selected
                        if (keySetupGamepadPopUp.firstPress || keySetupGamepadPopUp.selectedKeyCode !== -1) {
                            keySetupGamepadPopUp.firstPress = false;
                            return;
                        }

                        keySetupGamepadPopUp.selectedKeyCode = keyCode;
                    }
                }
            }

            contentItem: Item {
                anchors.fill: parent

                QQC2.Label {
                    text: ((keySetupGamepadPopUp.selectedKeyCode === -1) ?
                        i18n('Waiting for key press from your gamepad…') :
                        i18n("Selected key: %1", kcm.keyMapModel.keyDisplayProvider.keyDisplay(keySetupGamepadPopUp.selectedKeyCode)))
                    anchors.centerIn: parent
                    font.pixelSize: Bigscreen.Units.defaultFontPixelSize
                }
            }
        }
    }
}
