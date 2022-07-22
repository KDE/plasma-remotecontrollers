/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "devicesmodel.h"
#include "controllermanager.h"

DevicesModel *DevicesModel::instance()
{
    static DevicesModel* s_self = nullptr;
    if (!s_self) {
        s_self = new DevicesModel();
    }
    return s_self;
}

DevicesModel::DevicesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[DeviceTypeRole] = "deviceType";
    m_roleNames[DeviceNameRole] = "deviceName";
    m_roleNames[DeviceUniqueIdentifierRole] = "deviceUniqueIdentifier";
    m_roleNames[DeviceIconNameRole] = "deviceIconName";
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
    if (index >= m_devices.size())
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
        if (m_devices[i]->getUniqueIdentifier() == uniqueIdentifier) {
            return createIndex(i, 0);
        }
    }
    return QModelIndex();
}

void DevicesModel::deviceConnected(Device *device)
{
    if (device->getUniqueIdentifier().isEmpty()) {
        return;
    }

    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i]->getUniqueIdentifier() == device->getUniqueIdentifier()) {
            return;
        }
    }
    beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
    m_devices.append(device);
    endInsertRows();
    emit devicesChanged();
    emit dataChanged(createIndex(0, 0), createIndex(m_devices.size(), 0));
}

void DevicesModel::deviceDisconnected(Device *device)
{
    if (device->getUniqueIdentifier().isEmpty()) {
        return;
    }

    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i]->getUniqueIdentifier() == device->getUniqueIdentifier()) {
            beginRemoveRows(QModelIndex(), i, i);
            m_devices.removeAt(i);
            endRemoveRows();
            emit devicesChanged();
            return;
        }
    }
    emit dataChanged(createIndex(0, 0), createIndex(m_devices.size(), 0));
}

void DevicesModel::load()
{
    connect(&ControllerManager::instance(), &ControllerManager::deviceConnected, this, &DevicesModel::deviceConnected);
    connect(&ControllerManager::instance(), &ControllerManager::deviceDisconnected, this, &DevicesModel::deviceDisconnected);

    beginResetModel();
    m_devices.clear();

    auto device_model = ControllerManager::instance().connectedDevices();
    for (int i = 0; i < device_model.size(); ++i) {
        m_devices.append(device_model[i]);
    }

    endResetModel();
}

#include "moc_devicesmodel.cpp"
