/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "devicesmodel.h"
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
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

    ControllerManagerDBusInterface *iface = new ControllerManagerDBusInterface("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", QDBusConnection::sessionBus(), this);
    if(iface->isValid()) {
        connect(iface, &ControllerManagerDBusInterface::deviceConnected, this, &DevicesModel::deviceConnected);
        connect(iface, &ControllerManagerDBusInterface::deviceDisconnected, this, &DevicesModel::deviceDisconnected);
    }

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

    QStringList devices = connectedDevices();

    QVariantMap device;
    for (const QString &uniqueIdentifier : devices) {
        device["deviceName"] = deviceName(uniqueIdentifier);
        device["deviceType"] = deviceType(uniqueIdentifier);
        device["deviceUniqueIdentifier"] = uniqueIdentifier;
        device["deviceIconName"] = deviceIconName(uniqueIdentifier);
        m_devices.append(device);
    }

    endResetModel();
}

void DevicesModel::deviceConnected(const QString &uniqueIdentifier)
{
    QVariantMap device;
    device["deviceName"] = deviceName(uniqueIdentifier);
    device["deviceType"] = deviceType(uniqueIdentifier);
    device["deviceUniqueIdentifier"] = uniqueIdentifier;
    device["deviceIconName"] = deviceIconName(uniqueIdentifier);
    beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
    m_devices.append(device);
    endInsertRows();
}

void DevicesModel::deviceDisconnected(const QString &uniqueIdentifier)
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

QStringList DevicesModel::connectedDevices()
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "connectedDevices");
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        return QStringList();
    }
    return reply.arguments().at(0).toStringList();
}

QString DevicesModel::deviceName(const QString &uniqueIdentifier)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "deviceName");
    message << uniqueIdentifier;
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        return QString();
    }
    return reply.arguments().at(0).toString();
}

int DevicesModel::deviceType(const QString &uniqueIdentifier)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "deviceType");
    message << uniqueIdentifier;
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        return -1;
    }
    QVariant variant = reply.arguments().at(0);
    return variant.value<int>();
}

QString DevicesModel::deviceIconName(const QString &uniqueIdentifier)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "deviceIconName");
    message << uniqueIdentifier;
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        return QString();
    }
    QVariant variant = reply.arguments().at(0);
    return variant.value<QString>();
}

#include "moc_devicesmodel.cpp"
