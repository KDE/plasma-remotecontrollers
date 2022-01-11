/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include "../device.h"

#include <QObject>

#include <poll.h>
#include <wiiuse.h>

class Wiimote : public Device
{
    Q_OBJECT

public:
    explicit Wiimote(wiimote* wiiptr);
    ~Wiimote() = default;

public slots:
    void watchEvents() override;

Q_SIGNALS:
    void keyPress(int keyCode, bool pressed);

private:
    wiimote *m_wiiptr;

    void handleEvent();
};
