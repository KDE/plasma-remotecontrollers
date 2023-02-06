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

    QDBusInterface *iface = new QDBusInterface("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", QDBusConnection::sessionBus(), this);
    if(iface->isValid()) {
        connect(iface, SIGNAL(deviceConnected(QString)), this, SLOT(deviceConnected(QString)));
        connect(iface, SIGNAL(deviceDisconnected(QString)), this, SLOT(deviceDisconnected(QString)));
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
        device["deviceName"] = getDeviceName(uniqueIdentifier);
        device["deviceType"] = getDeviceType(uniqueIdentifier);
        device["deviceUniqueIdentifier"] = uniqueIdentifier;
        device["deviceIconName"] = getDeviceIconName(uniqueIdentifier);
        m_devices.append(device);
    }

    endResetModel();
}

void DevicesModel::deviceConnected(const QString &uniqueIdentifier)
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
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "getConnectedDevices");
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        return QStringList();
    }
    QStringList uniqueIdentifiers;
    QVariant variant = reply.arguments().at(0);
    QStringList list = variant.value<QStringList>();
    return list;
}

QString DevicesModel::getDeviceName(const QString &uniqueIdentifier)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "getDeviceName");
    message << uniqueIdentifier;
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        return QString();
    }
    QString deviceName;
    QVariant variant = reply.arguments().at(0);
    return variant.value<QString>();
}

int DevicesModel::getDeviceType(const QString &uniqueIdentifier)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "getDeviceType");
    message << uniqueIdentifier;
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        return -1;
    }
    QVariant variant = reply.arguments().at(0);
    return variant.value<int>();
}

QString DevicesModel::getDeviceIconName(const QString &uniqueIdentifier)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasma.remotecontrollers", "/ControllerManager", "org.kde.plasma.remotecontrollers.ControllerManager", "getDeviceIconName");
    message << uniqueIdentifier;
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        return QString();
    }
    QVariant variant = reply.arguments().at(0);
    return variant.value<QString>();
}

#include "moc_devicesmodel.cpp"
