/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include "wiimote.h"
#include "../device.h"

#include <QThread>
#include <xwiimote.h>

#define LOOPTIME 50 * 1000
#define LOOPTIME_WII 7.5 * 1000

class WiimoteController : public QObject
{
    Q_OBJECT

public:
    explicit WiimoteController();
    ~WiimoteController();

public Q_SLOTS:
    void deviceDetection();

Q_SIGNALS:
    void newDevice(Device *device);

private:
    QThread *m_detectionThread;
    QThread *m_eventsThread;
};

class WiimoteEventWatcher : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void watchEvents();
};
