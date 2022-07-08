/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "devicesmodel.h"
#include "controllermanager.h"

DevicesModel::DevicesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[DeviceTypeRole] = "deviceType";
    m_roleNames[DeviceNameRole] = "deviceName";
    m_roleNames[DeviceUniqueIdentifierRole] = "deviceUniqueIdentifier";
    m_roleNames[DeviceIconNameRole] = "deviceIconName";
}

DevicesModel::~DevicesModel()
{
    clear();
}

QHash<int, QByteArray> DevicesModel::roleNames() const
{
    return m_roleNames;
}

void DevicesModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_devices.size() - 1);
    m_devices.clear();
    endRemoveRows();
}

void DevicesModel::refresh()
{
    beginRemoveRows(QModelIndex(), 0, m_devices.size() - 1);
    m_devices.clear();
    endRemoveRows();

    auto connected_devices = ControllerManager::instance().getConnectedDevices();
    beginInsertRows(QModelIndex(), 0, connected_devices.size() - 1);
    m_devices = connected_devices;
    endInsertRows();

    beginResetModel();
    endResetModel();
}

int DevicesModel::rowCount(const QModelIndex &) const
{
    return m_devices.size();
}

QVariant DevicesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_devices.size())
        return QVariant();

    // Can't use const device here, because we need to call the getter functions
    // to get the data.

    Device *device = m_devices[index.row()];
    switch (role) {
    case DeviceTypeRole:
        return device->getDeviceType();
    case DeviceNameRole:
        return device->getName();
    case DeviceUniqueIdentifierRole:
        return device->getUniqueIdentifier();
    case DeviceIconNameRole:
        return device->iconName();
    default:
        return QVariant();
    }

    return QVariant();
}

QVariantMap DevicesModel::get(int index) const
{
    if (index < 0 || index >= m_devices.size())
        return QVariantMap();

    Device *device = m_devices[index];
    QVariantMap map;
    map["deviceType"] = device->getDeviceType();
    map["deviceName"] = device->getName();
    map["deviceUniqueIdentifier"] = device->getUniqueIdentifier();
    map["deviceIconName"] = device->iconName();
    return map;
}

QModelIndex DevicesModel::indexOf(const QString &uniqueIdentifier) const
{
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i]->getUniqueIdentifier() == uniqueIdentifier)
            return index(i, 0);
    }

    return QModelIndex();
}

#include "moc_devicesmodel.cpp"