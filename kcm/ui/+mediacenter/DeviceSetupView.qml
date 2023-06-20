/*
    SPDX-FileCopyrightText: 2020 Aditya Mehra <aix.m@outlook.com>
    SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.components 2.0 as PlasmaComponents2
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.mycroft.bigscreen 1.0 as BigScreen
import Qt5Compat.GraphicalEffects

Rectangle {
    id: deviceView
    color: Kirigami.Theme.backgroundColor
    property QtObject currentDevice
    property var deviceType
    property bool ignoreEvent: false

    onIgnoreEventChanged: {
        if (ignoreEvent) {
            ignoreEventResetTimer.start()
        }
    }

    Timer {
        id: ignoreEventResetTimer
        interval: 200
        onTriggered: {
            deviceView.ignoreEvent = false
        }
    }
    
    onActiveFocusChanged: {
        if(activeFocus){
            deviceMapView.forceActiveFocus()
        }
    }
    
    ColumnLayout {
        id: colLayoutSettingsItem
        clip: true

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: headerAreaSettingsItem.top
            margins: Kirigami.Units.largeSpacing * 2
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height * 0.10
        }
        
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: dIcon.height + (label2.height + Kirigami.Units.largeSpacing * 2)
            Layout.alignment: Qt.AlignTop

            Rectangle {
                id: dIcon
                anchors.top: headrSept.bottom
                anchors.topMargin: Kirigami.Units.largeSpacing
                anchors.horizontalCenter: parent.horizontalCenter
                width: Kirigami.Units.iconSizes.huge
                height: width
                radius: 100
                color: Kirigami.Theme.backgroundColor
                
                PlasmaCore.IconItem {
                    id: deviceIconStatus
                    anchors.centerIn: parent
                    width: Kirigami.Units.iconSizes.large
                    height: width
                    source: currentDevice.deviceIconName
                }
            }
            
            Kirigami.Heading {
                id: label2
                width: parent.width
                anchors.top: dIcon.bottom
                anchors.topMargin: Kirigami.Units.largeSpacing
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                level: 2
                maximumLineCount: 2
                elide: Text.ElideRight
                color: PlasmaCore.ColorScope.textColor
                text: i18n(currentDevice.deviceName)
            }

            Kirigami.Separator {
                id: lblSept2
                anchors.top: label2.bottom
                anchors.topMargin: Kirigami.Units.smallSpacing
                height: 1
                width: parent.width
            }
        }

        DeviceMap {
            id: deviceMapView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            deviceType: deviceView.deviceType
        }
    }

    RowLayout {
        id: headerAreaSettingsItem
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Kirigami.Units.largeSpacing * 2
        height: Kirigami.Units.gridUnit * 2

        PlasmaComponents2.Button {
            id: backBtnSettingsItem
            iconSource: "arrow-left"
            Layout.alignment: Qt.AlignLeft

            KeyNavigation.up: deviceMapView
            KeyNavigation.down: deviceMapView

            PlasmaComponents2.Highlight {
                z: -2
                anchors.fill: parent
                anchors.margins: -Kirigami.Units.gridUnit / 4
                visible: backBtnSettingsItem.activeFocus ? 1 : 0
            }

            Keys.onReturnPressed: {
                clicked()
            }

            onClicked: {
                connectionView.forceActiveFocus()
            }
        }

        Label {
            id: backbtnlabelHeading
            text: i18n("Go back [←] to return to device selection")
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
        }
    }
}
