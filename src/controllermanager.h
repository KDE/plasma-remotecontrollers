/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include "device.h"

#include <QObject>

class AbstractSystem;

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

public slots:
    void emitKey(int key, bool pressed) const;
    void removeDevice(int deviceIndex);
    QVector<Device*> connectedDevices();

Q_SIGNALS:
    void deviceConnected(Device*);
    void deviceDisconnected(Device*);

private:
    QStringList m_applicationBlacklist;
    bool m_enabled = true;
    QVector<Device*> m_connectedDevices;
    QScopedPointer<AbstractSystem> m_inputSystem;
};
