/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DEVICESMODEL_H
#define DEVICESMODEL_H

#include <QAbstractListModel>
#include <QObject>

class DevicesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum DeviceRoles {
        DeviceTypeRole = Qt::UserRole + 1,
        DeviceNameRole,
        DeviceUniqueIdentifierRole,
        DeviceIconNameRole
    };

    Q_ENUM(DeviceRoles)

    explicit DevicesModel(QObject *parent = 0);
    ~DevicesModel();
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex indexOf(const QString &uniqueIdentifier) const;
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE void load();
    void deviceConnected(QString uniqueIdentifier);
    void deviceDisconnected(QString uniqueIdentifier);

    // Dbus interface implementation
    QStringList getConnectedDevices();
    QString getDeviceName(const QString &uniqueIdentifier);
    int getDeviceType(const QString &uniqueIdentifier);
    QString getDeviceIconName(const QString &uniqueIdentifier);
    void setNoop();
    void releaseNoop();

Q_SIGNALS:
    void devicesChanged();
    void countChanged();

private:
    QHash<int, QByteArray> m_roleNames;
    QList<QVariantMap> m_devices;
};


#endif // DEVICESMODEL_H
