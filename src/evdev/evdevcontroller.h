/*
 *   SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QDebug>
#include <QThread>
#include <QSet>
#include <Solid/Device>

#include "../device.h"
#include <libevdev/libevdev.h>
#include "evdevdbusinterface.h"

class EvdevDeviceThread;
class EvdevDbusInterface;
class EvdevController : public QObject
{
public:
    explicit EvdevController();
    bool addDevice(const Solid::Device &device);

    EvdevDbusInterface m_dbusInterface;
};

class EvdevDevice : public Device
{
    Q_OBJECT

public:
    explicit EvdevDevice(const QString &udi, libevdev *device, EvdevController* controller);
    ~EvdevDevice() override;

    libevdev *device() const {
        return m_device;
    }
    void readNow();

Q_SIGNALS:
    void keyPress(int keyCode, bool pressed);

private:
    void processEvent(struct input_event &ev);
    void setKey(int key, bool pressed);

    EvdevController *const m_controller;
    QSet<int> m_pressedKeys;
    void deviceRemoved(const QString &udi);
    libevdev *const m_device;
    const QString m_udi;
    const QMap<int, QList<int>> m_buttons;
};


