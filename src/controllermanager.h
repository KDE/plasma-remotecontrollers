/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include "device.h"
#include <QHash>
#include <QObject>
#include <QDateTime>
#include <QSet>
#include <QTimer>
#include "controllermanagerdbusinterface.h"

class AbstractSystem;
class KStatusNotifierItem;
class QAction;
class RemoteControllersSettings;

namespace TaskManager
{
    class TasksModel;
}

class ControllerManagerDBusInterface;
class ControllerManager : public QObject
{
    Q_OBJECT

public:
    explicit ControllerManager(QObject *parent = nullptr);
    ~ControllerManager();
    static ControllerManager &instance();
    
    void newDevice(Device *device);
    void deviceRemoved(Device *device);
    bool isConnected(QString uniqueIdentifier);
    QVector<Device*> getDevicesByType(DeviceType deviceType);

    /** Have input not forward events to the OS */
    void noopInput();

    /** Have input forward events to the OS */
    void resetInputSystem();

public Q_SLOTS:
    void emitKey(int key, bool pressed);
    void removeDevice(int deviceIndex);
    QVector<Device*> connectedDevices();
    void refreshApps();

Q_SIGNALS:
    void deviceConnected(Device*);
    void deviceDisconnected(Device*);

private:
    void simulateUserActivity();
    bool appInhibited(const QString &appId) const;
    ControllerManagerDBusInterface *m_dbusInterface = nullptr;

    bool m_enabled = true;
    QVector<Device*> m_connectedDevices;
    QScopedPointer<AbstractSystem> m_inputSystem;
    KStatusNotifierItem *m_sni = nullptr;
    TaskManager::TasksModel *m_appsModel = nullptr;
    QHash<QString, QAction *> m_appActions;
    QScopedPointer<RemoteControllersSettings> m_settings;
    QTimer m_lastUsed;

    QDateTime m_lastSimulated;
    QSet<int> m_usedKeys;
};
