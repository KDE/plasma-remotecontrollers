/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "controllermanagerdbusinterface.h"
#include "device.h"
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusArgument>

ControllerManagerDBusInterface::ControllerManagerDBusInterface(QObject *parent)
    : QObject(parent)
{
    QDBusConnection::sessionBus().registerObject("/ControllerManager", this, QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllSlots);
}

ControllerManagerDBusInterface::~ControllerManagerDBusInterface()
{
}

Q_SCRIPTABLE QStringList ControllerManagerDBusInterface::getConnectedDevices()
{
    auto connected_devices = ControllerManager::instance().connectedDevices();
    QStringList deviceList;
    for (auto device : connected_devices) {
        deviceList << device->getUniqueIdentifier();
    }

    return deviceList;
}

Q_SCRIPTABLE QString ControllerManagerDBusInterface::getDeviceName(const QString &uniqueIdentifier)
{
    QVector<Device*> devices = ControllerManager::instance().connectedDevices();
    for (auto device : devices) {
        if (device->getUniqueIdentifier() == uniqueIdentifier) {
            return device->getName();
        }
    }
    return QString();
}

Q_SCRIPTABLE int ControllerManagerDBusInterface::getDeviceType(const QString &uniqueIdentifier)
{
    QVector<Device*> devices = ControllerManager::instance().connectedDevices();
    for (auto device : devices) {
        if (device->getUniqueIdentifier() == uniqueIdentifier) {
            return device->getDeviceType();
        }
    }
   
    return -1;
}

Q_SCRIPTABLE QString ControllerManagerDBusInterface::getDeviceIconName(const QString &uniqueIdentifier)
{
    QVector<Device*> devices = ControllerManager::instance().connectedDevices();
    for (auto device : devices) {
        if (device->getUniqueIdentifier() == uniqueIdentifier) {
            return device->iconName();
        }
    }
    return QString();
}

Q_SCRIPTABLE void ControllerManagerDBusInterface::acquireNoOp()
{
    ControllerManager::instance().noopInput();
}

Q_SCRIPTABLE void ControllerManagerDBusInterface::releaseNoOp()
{
    ControllerManager::instance().releaseNoop();
}