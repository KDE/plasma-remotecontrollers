/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DEVICESMODEL_H
#define DEVICESMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDBusInterface>

class ControllerManagerDBusInterface : public QDBusInterface
{
    Q_OBJECT

public:
    ControllerManagerDBusInterface(const QString &service, const QString &path, const char *interface, const QDBusConnection &connection, QObject *parent = nullptr)
        : QDBusInterface(service, path, interface, connection, parent)
    {
    }

signals:
    void deviceConnected(const QString &deviceName);
    void deviceDisconnected(const QString &deviceName);
};

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

    explicit DevicesModel(QObject *parent = nullptr);
    ~DevicesModel();
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex indexOf(const QString &uniqueIdentifier) const;
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE void load();

    // Dbus interface implementation
    QStringList connectedDevices();
    QString deviceName(const QString &uniqueIdentifier);
    int deviceType(const QString &uniqueIdentifier);
    QString deviceIconName(const QString &uniqueIdentifier);

public Q_SLOTS:
    void deviceConnected(const QString &uniqueIdentifier);
    void deviceDisconnected(const QString &uniqueIdentifier);

Q_SIGNALS:
    void devicesChanged();
    void countChanged();

private:
    QHash<int, QByteArray> m_roleNames;
    QList<QVariantMap> m_devices;
};


#endif // DEVICESMODEL_H
