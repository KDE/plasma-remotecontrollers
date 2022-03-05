/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "controllermanager.h"
#include "notificationsmanager.h"

#include <QDebug>

#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>

static constexpr std::array<int, 38> s_supportedKeys = { KEY_PLAY
                , KEY_STOP
                , KEY_PAUSE
                , KEY_REWIND
                , KEY_FASTFORWARD
                , KEY_ENTER
                , KEY_CANCEL
                , KEY_SELECT
                , KEY_UP
                , KEY_DOWN
                , KEY_LEFT
                , KEY_RIGHT
                , KEY_0
                , KEY_1
                , KEY_2
                , KEY_3
                , KEY_4
                , KEY_5
                , KEY_6
                , KEY_7
                , KEY_8
                , KEY_9
                , KEY_BLUE
                , KEY_RED
                , KEY_GREEN
                , KEY_YELLOW
                , KEY_CHANNELUP
                , KEY_CHANNELDOWN
                , KEY_VOLUMEUP
                , KEY_VOLUMEDOWN
                , KEY_EXIT
                , KEY_BACK
                , KEY_HOME
                , KEY_MENU
                , KEY_SUBTITLE
                , KEY_MINUS
                , KEY_FORWARD
                , KEY_LEFTMETA
};

ControllerManager::ControllerManager(QObject *parent)
    : QObject(parent)
{
    // Setup notifications
    QObject::connect(this, &ControllerManager::deviceConnected,
                     &NotificationsManager::instance(), &NotificationsManager::notifyNewDevice);
    QObject::connect(this, &ControllerManager::deviceDisconnected,
                     &NotificationsManager::instance(), &NotificationsManager::notifyDisconnectedDevice);

    struct uinput_setup usetup;
    m_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    
    if (m_fd < 0) {
        qCritical() << "Couldn't open /dev/uinput, make sure the kernel module is loaded";
        exit(1);
    }
    
    // Register all keys we want to press with this application
    ioctl(m_fd, UI_SET_EVBIT, EV_KEY);

    for ( auto x : s_supportedKeys)
        ioctl(m_fd, UI_SET_KEYBIT, x);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; // TODO: change vendor
    usetup.id.product = 0x5678; // TODO: change product
    strcpy(usetup.name, "Wiimote");
    
    ioctl(m_fd, UI_DEV_SETUP, &usetup);
    ioctl(m_fd, UI_DEV_CREATE);
    sleep(1);
}

ControllerManager &ControllerManager::instance()
{
    static ControllerManager _instance;
    return _instance;
}

void ControllerManager::newDevice(Device *device)
{
    qInfo() << "New device connected:" << device->getName();

    device->setIndex(m_connectedDevices.size());

    connect(device, &Device::deviceDisconnected, this, &ControllerManager::removeDevice);

    m_connectedDevices.append(device);

    // Don't send notifications for CEC devices, since we expect them to always be available
    if (device->getDeviceType() != DeviceCEC)
        emit deviceConnected(device);
}

void ControllerManager::removeDevice(int deviceIndex)
{
    Device *removedDevice = m_connectedDevices.at(deviceIndex);
    m_connectedDevices.remove(deviceIndex);

    qInfo() << "Device disconnected:" << removedDevice->getName();

    emit deviceDisconnected(removedDevice);
    delete removedDevice;
    
    // Reset indexes
    for (int i = 0; i < m_connectedDevices.size(); i++)
        m_connectedDevices.at(i)->setIndex(i);
}

bool ControllerManager::isConnected(QString uniqueIdentifier)
{
    if (m_connectedDevices.size() < 1)
        return false;

    return std::find_if(m_connectedDevices.begin(), m_connectedDevices.end(), [&uniqueIdentifier](Device *other) {
        return other->getUniqueIdentifier() == uniqueIdentifier;
    }) != m_connectedDevices.end();
}

QVector<Device*> ControllerManager::getDevicesByType(DeviceType deviceType)
{
    QVector<Device*> devices;

    for (int i = 0; i < m_connectedDevices.size(); i++)
        if (m_connectedDevices.at(i)->getDeviceType() == deviceType)
            devices.append(m_connectedDevices.at(i));

    return devices;
}

void ControllerManager::emitKey(int key, bool pressed)
{
    if (std::find(s_supportedKeys.cbegin(), s_supportedKeys.cend(), key) == s_supportedKeys.cend()) {
        qWarning() << "reporting unannounced key" << key;
    }
    emitEvent(EV_KEY, key, pressed ? 1 : 0);
    emitEvent(EV_SYN, SYN_REPORT, 0);
}

void ControllerManager::emitEvent(int type, int code, int val)
{
    struct input_event ie;
    
    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.input_event_sec = 0;
    ie.input_event_usec = 0;
    
    write(m_fd, &ie, sizeof(ie));
}

ControllerManager::~ControllerManager()
{
    m_connectedDevices.clear();
}
