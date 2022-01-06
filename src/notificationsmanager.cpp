#include "notificationsmanager.h"

#include <QDebug>
#include <KNotification>

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

    notification->setTitle("New controller connected");
    if (device->getDeviceType() == DeviceWiimote)
        notification->setText("A Wiimote has been connected");
    notification->sendEvent();
}

void NotificationsManager::notifyDisconnectedDevice(Device* device)
{
    KNotification* notification = new KNotification("controllerDisconnected");

    notification->setTitle("A controller has been disconnected");
    if (device->getDeviceType() == DeviceWiimote)
        notification->setText("A wiimote has been disconnected");
    notification->sendEvent();
}
