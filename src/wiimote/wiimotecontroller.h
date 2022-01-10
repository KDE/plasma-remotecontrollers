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

class WiimoteController : public QThread
{
    Q_OBJECT

public:
    explicit WiimoteController();
    ~WiimoteController() override;
    
    void run() override;

Q_SIGNALS:
    void newDevice(Device *device);
};
