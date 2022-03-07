/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "notificationsmanager.h"

#include <QDebug>
#include <KNotification>
#include <KLocalizedString>

NotificationsManager &NotificationsManager::instance()
{
    static NotificationsManager _instance;
    return _instance;
}

NotificationsManager::NotificationsManager()
{
}

void NotificationsManager::notifyNewDevice(Device* device)
{
    KNotification* notification = new KNotification("controllerConnected");

    notification->setTitle(i18n("Controller connected"));
    notification->setText(device->getName());
    notification->setIconName(device->iconName());
    notification->sendEvent();
}

void NotificationsManager::notifyDisconnectedDevice(Device* device)
{
    KNotification* notification = new KNotification("controllerDisconnected");

    notification->setTitle(i18n("Controller Disconnected"));
    notification->setText(device->getName());
    notification->setIconName(device->iconName());
    notification->sendEvent();
}
