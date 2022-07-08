/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DEVICESMODEL_H
#define DEVICESMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include "device.h"

class DevicesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum DeviceRoles {
        DeviceTypeRole = Qt::UserRole + 1,
        DeviceNameRole,
        DeviceUniqueIdentifierRole,
        DeviceIconNameRole
    };

    Q_ENUM(DeviceRoles)

    explicit DevicesModel(QObject *parent = nullptr);
    ~DevicesModel() override;

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex indexOf(const QString &uniqueIdentifier) const;    
    Q_INVOKABLE QVariantMap get(int index) const;
    void refresh();
    void clear();
    
private:
    QHash<int, QByteArray> m_roleNames;
    QVector<Device*> m_devices;
};


#endif // DEVICESMODEL_H