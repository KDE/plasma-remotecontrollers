/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "devicesmodel.h"
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusArgument>
#include <QDebug>

DevicesModel::DevicesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[DeviceTypeRole] = "deviceType";
    m_roleNames[DeviceNameRole] = "deviceName";
    m_roleNames[DeviceUniqueIdentifierRole] = "deviceUniqueIdentifier";
    m_roleNames[DeviceIconNameRole] = "deviceIconName";

    // We want to listen for signals from the dbus interface
    // QDBusConnection::sessionBus().connect(QString(), "/ControllerManager", "org.kde.plasma.remotecontrollers", "deviceConnected", this, &DevicesModel::deviceConnected);
    // QDBusConnection::sessionBus().connect(QString(), "/ControllerManager", "org.kde.plasma.remotecontrollers", "deviceDisconnected", this, &DevicesModel::deviceDisconnected);

    load();
}

DevicesModel::~DevicesModel()
{
}

QHash<int, QByteArray> DevicesModel::roleNames() const
{
    return m_roleNames;
}

int DevicesModel::count() const
{
    return m_devices.count();
}

int DevicesModel::rowCount(const QModelIndex &) const
{
    return m_devices.size();
}

QVariant DevicesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() >= m_devices.size())
        return QVariant();

    auto device = m_devices[index.row()];
    switch (role) {
    case DeviceTypeRole:
        return device["deviceType"];
    case DeviceNameRole:
        return device["deviceName"];
    case DeviceUniqueIdentifierRole:
        return device["deviceUniqueIdentifier"];
    case DeviceIconNameRole:
        return device["deviceIconName"];
    default:
        return QVariant();
    }

    return QVariant();
}

QVariantMap DevicesModel::get(int index) const
{
    if (index >= m_devices.size())
        return QVariantMap();

    auto device = m_devices[index];
    QVariantMap map;
    map["deviceType"] = device["deviceType"];
    map["deviceName"] = device["deviceName"];
    map["deviceUniqueIdentifier"] = device["deviceUniqueIdentifier"];
    map["deviceIconName"] = device["deviceIconName"];
    return map;
}

QModelIndex DevicesModel::indexOf(const QString &uniqueIdentifier) const
{
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i]["deviceUniqueIdentifier"] == uniqueIdentifier) {
            return index(i);
        }
    }
    return QModelIndex();
}

void DevicesModel::load()
{
    beginResetModel();
    m_devices.clear();
    qDebug() << "Loading devices...";

    // We will get a QStringList of connectedDevices from the getConnectedDevices method in the dbus interface
    QStringList connectedDevices = getConnectedDevices();
    // We need to build a list of devices from the connectedDevices QStringList
    QVariantMap device;
    for (const QString &uniqueIdentifier : connectedDevices) {
        qDebug() << "Found device:" << uniqueIdentifier;
        device["deviceName"] = getDeviceName(uniqueIdentifier);
        device["deviceType"] = getDeviceType(uniqueIdentifier);
        device["deviceUniqueIdentifier"] = uniqueIdentifier;
        device["deviceIconName"] = getDeviceIconName(uniqueIdentifier);
        m_devices.append(device);
    }

    for (const QVariantMap &device : m_devices) {
        qDebug() << "Device:" << device;
    }
    endResetModel();
}

void DevicesModel::deviceConnected(QString uniqueIdentifier)
{
    QVariantMap device;
    device["deviceName"] = getDeviceName(uniqueIdentifier);
    device["deviceType"] = getDeviceType(uniqueIdentifier);
    device["deviceUniqueIdentifier"] = uniqueIdentifier;
    device["deviceIconName"] = getDeviceIconName(uniqueIdentifier);
    beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
    m_devices.append(device);
    endInsertRows();
}

void DevicesModel::deviceDisconnected(QString uniqueIdentifier)
{
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i]["deviceUniqueIdentifier"] == uniqueIdentifier) {
            beginRemoveRows(QModelIndex(), i, i);
            m_devices.removeAt(i);
            endRemoveRows();
            break;
        }
    }
}

QStringList DevicesModel::getConnectedDevices()
{
    // we will get a QStringList of uniqueIdentifiers fro the getConnectedDevices method in the dbus interface
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "getConnectedDevices");
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Error getting connected devices:" << reply.errorMessage();
        return QStringList();
    }
    qDebug() << "Reply:" << reply.arguments();
    // Reply is like this: Reply: (QVariant(QStringList, ("/org/kde/solid/udev/sys/devices/pci0000:00/0000:00:14.0/usb1/1-3/1-3:1.0/0003:2563:0580.000E/input/input179/event256")))
    QStringList uniqueIdentifiers;
    QVariant variant = reply.arguments().at(0);
    QStringList list = variant.value<QStringList>();
    return list;
}

QString DevicesModel::getDeviceName(const QString &uniqueIdentifier)
{
    // we will get a QString of deviceName fro the getDeviceName method in the dbus interface
    qDebug() << "Getting device name for" << uniqueIdentifier;
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "getDeviceName");
    message << uniqueIdentifier;
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Error getting device name:" << reply.errorMessage();
        return QString();
    }
    QString deviceName;
    qDebug() << "Reply Name:" << reply.arguments();
    QVariant variant = reply.arguments().at(0);
    return variant.value<QString>();
}

int DevicesModel::getDeviceType(const QString &uniqueIdentifier)
{
    // we will get a int of deviceType fro the getDeviceType method in the dbus interface
    qDebug() << "Getting device type for" << uniqueIdentifier;
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "getDeviceType");
    message << uniqueIdentifier;
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Error getting device type:" << reply.errorMessage();
        return -1;
    }
    qDebug() << "Reply Device Type:" << reply.arguments();
    QVariant variant = reply.arguments().at(0);
    return variant.value<int>();
}

QString DevicesModel::getDeviceIconName(const QString &uniqueIdentifier)
{
    // we will get a QString of deviceIconName fro the getDeviceIconName method in the dbus interface
    qDebug() << "Getting device icon name for" << uniqueIdentifier;
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "getDeviceIconName");
    message << uniqueIdentifier;
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Error getting device icon name:" << reply.errorMessage();
        return QString();
    }
    qDebug() << "Reply Device Icon Name:" << reply.arguments();
    QVariant variant = reply.arguments().at(0);
    return variant.value<QString>();
}

#include "moc_devicesmodel.cpp"
