/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include "device.h"

#include <QObject>

class ControllerManager : public QObject
{
    Q_OBJECT

public:
    explicit ControllerManager(QObject *parent = nullptr);
    ~ControllerManager();
    static ControllerManager &instance();
    
    void newDevice(Device *device);
    bool isConnected(QString uniqueIdentifier);
    int getConnectedDevicesCount();
    QVector<Device*> getDevicesByType(DeviceType deviceType);

public slots:
    void emitKey(int key, bool pressed);
    void removeDevice(int deviceIndex);

Q_SIGNALS:
    void deviceConnected(Device*);
    void deviceDisconnected(Device*);

private:
    int m_fd;
    QVector<Device*> m_connectedDevices;

    void emitEvent(int type, int code, int val);
};
