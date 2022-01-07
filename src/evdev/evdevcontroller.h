/*
 *   SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QDebug>
#include <QThread>

#include "../device.h"
#include <libevdev/libevdev.h>

class EvdevDeviceThread;
class EvdevController : public QObject
{
public:
    explicit EvdevController();
    bool addDevice(const QString &path);
};

class EvdevDevice : public Device
{
    Q_OBJECT

public:
    explicit EvdevDevice(const QString& path, libevdev *device);
    ~EvdevDevice() override;

    libevdev *device() const {
        return m_device;
    }

signals:
    void keyPress(int keyCode, bool pressed);

private:
    EvdevDeviceThread *m_thread;
    libevdev *const m_device;
};


