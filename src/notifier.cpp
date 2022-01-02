#include "notifier.h"

#include <QDebug>
#include <KNotification>

Notifier::Notifier()
{
}

void Notifier::notifyNewDevice(DeviceType deviceType)
{
    KNotification* notification = new KNotification("controllerConnected");

    notification->setTitle("New controller connected");
    if (deviceType == DeviceWiimote)
        notification->setText("A Wiimote has been connected");
    notification->sendEvent();
}

void Notifier::notifyDisconnectedDevice(DeviceType deviceType)
{
    KNotification* notification = new KNotification("controllerDisconnected");

    notification->setTitle("A controller has been disconnected");
    if (deviceType == DeviceWiimote)
        notification->setText("A wiimote has been disconnected");
    notification->sendEvent();
}

Notifier::~Notifier() = default;
