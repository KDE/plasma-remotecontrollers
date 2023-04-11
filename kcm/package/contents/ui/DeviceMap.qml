/*
    SPDX-FileCopyrightText: 2020 Aditya Mehra <aix.m@outlook.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14
import org.kde.kirigami 2.12 as Kirigami
import org.kde.kitemmodels 1.0 as KItemModels
import org.kde.private.kcm.remotecontrollers 1.0
import "delegates" as Delegates

Item {
    id: deviceMap
    Layout.fillWidth: true
    Layout.fillHeight: true
    property var deviceType

    onFocusChanged: {
        if(focus) {
            buttonMapRepeater.forceActiveFocus()
        }
    }

    ScrollBar {
        id: scrollBarListView
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.gridUnit + Kirigami.Units.smallSpacing
        anchors.rightMargin: Kirigami.Units.gridUnit + Kirigami.Units.smallSpacing

        Kirigami.Heading {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: switch(deviceMap.deviceType) {
                case 0:
                    return i18n("Bind TV Remote Keys To Buttons");
                case 1:
                    return i18n("Bind Wii Remote Keys To Buttons");
                case 2:
                    return i18n("Bind Gamepad Keys To Buttons");
            }
        }

        Item {
            Layout.preferredHeight: Kirigami.Units.gridUnit
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: buttonMapRepeater
                anchors.fill: parent
                clip: true
                model: KItemModels.KSortFilterProxyModel { 
                    sourceModel: kcm.keyMapModel
                    filterRole: "DeviceTypeRole"
                    filterRowCallback: function(source_row, source_parent) {
                        var filter_device = deviceMap.deviceType == 0 ? "CEC" : "GAMEPAD"
                        return sourceModel.data(sourceModel.index(source_row, 0, source_parent), KeyMapModel.DeviceTypeRole).indexOf(filter_device) !== -1;
                    }
                }

                keyNavigationEnabled: true
                highlightFollowsCurrentItem: true
                spacing: Kirigami.Units.smallSpacing
                ScrollBar.vertical: scrollBarListView
                delegate: Delegates.MapButtonDelegate {
                    id: mapButtonType
                    objectName: model.objectName
                }
            }
        }
    }
}
