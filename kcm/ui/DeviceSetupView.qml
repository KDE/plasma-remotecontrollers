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
import org.kde.plasma.components 3.0 as PlasmaComponents
import Qt5Compat.GraphicalEffects

Item {
    id: deviceView
    property var deviceName
    property var deviceIcon
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
        anchors.fill: parent            
        anchors.margins: Kirigami.Units.largeSpacing
        
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4
            Layout.alignment: Qt.AlignTop

            RowLayout {
                id: headerViewRowLayout
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - Kirigami.Units.largeSpacing

                Rectangle {
                    id: dIcon
                    Layout.preferredWidth: Kirigami.Units.iconSizes.huge
                    Layout.preferredHeight: width
                    radius: 100
                    color: Kirigami.Theme.backgroundColor
                    
                    Kirigami.Icon {
                        id: deviceIconStatus
                        anchors.centerIn: parent
                        width: Kirigami.Units.iconSizes.large - Kirigami.Units.largeSpacing
                        height: width
                        source: deviceView.deviceIcon
                    }
                }
                
                Kirigami.Heading {
                    id: label2
                    Layout.fillWidth: true
                    Layout.fillHeight: true                 
                    horizontalAlignment: Text.AlignLeft
                    wrapMode: Text.WordWrap
                    level: 2
                    maximumLineCount: 2
                    elide: Text.ElideRight
                    color: PlasmaCore.ColorScope.textColor
                    text: i18n(deviceView.deviceName)
                }
            }

            Kirigami.Separator {
                id: lblSept2
                anchors.top: headerViewRowLayout.bottom
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
}
