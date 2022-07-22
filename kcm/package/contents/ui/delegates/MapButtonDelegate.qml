/*
    SPDX-FileCopyrightText: 2020 Aditya Mehra <aix.m@outlook.com>
    
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.kirigami 2.12 as Kirigami

PlasmaComponents3.Button {
    id: btnMap
    implicitWidth: parent.width
    padding: Kirigami.Units.largeSpacing

    background: Rectangle {
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Button
        color: btnMap.activeFocus ? Kirigami.Theme.linkColor : Kirigami.Theme.backgroundColor
        border.width: 1
        border.color: Kirigami.Theme.disabledTextColor
    }

    contentItem: RowLayout {
        id: keyLayout

        PlasmaComponents.Label {
            id: keyButtonDisplayLabel
            Layout.preferredWidth: parent.width / 2
            horizontalAlignment: Text.AlignHCenter
            text: model.buttonDisplay
            elide: Text.ElideLeft
        }
  
        Item {
            Layout.preferredWidth: Kirigami.Units.iconSizes.large

            Kirigami.Icon {
                id: keyIcon
                source: model.assignedKeyIcon
                width: Kirigami.Units.iconSizes.large
                height: Kirigami.Units.iconSizes.large 
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter                                       
                visible: model.assignedKeyIcon != "" ? 1 : 0
                enabled: model.assignedKeyIcon != "" ? 1 : 0
                color: Kirigami.Theme.textColor
            }
        }

        Kirigami.Separator {
            id: decorationSeparator
            Layout.preferredWidth: 1                
            Layout.fillHeight: true
            visible: keyIcon.visible
            enabled: keyIcon.visible               
        }

        PlasmaComponents.Label {
            id: keyValue
            Layout.leftMargin: keyIcon.visible ? Kirigami.Units.largeSpacing : 0
            Layout.preferredWidth: parent.width * 0.25
            horizontalAlignment: decorationSeparator.visible ? Text.AlignLeft : Text.AlignHCenter
            fontSizeMode: Text.HorizontalFit
            minimumPixelSize: 8
            elide: Text.ElideRight
            font.pixelSize: keyButtonDisplayLabel.font.pixelSize
            text: model.assignedKeyDisplay
        }
    }

    Keys.onReturnPressed: clicked()

    onClicked: {
        if (model.deviceType == "GAMEPAD") {
            keySetupGamepadPopUp.keyType = [model.buttonDisplay, model.buttonType]
            
            if(!deviceView.ignoreEvent) {
                keySetupGamepadPopUp.open()
            }
        }
        if (model.deviceType == "CEC") {
            keySetupPopUp.keyType = [model.buttonDisplay, model.buttonType]
            keySetupPopUp.open()
        }
    }
}
