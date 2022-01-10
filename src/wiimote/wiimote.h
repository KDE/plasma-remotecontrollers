/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include "../device.h"

#include <QObject>

#include <xwiimote.h>
#include <poll.h>

enum WiimoteExtensions {
    EXTENSION_NONE,
    EXTENSION_UNKNOWN,
    EXTENSION_NUNCHUK,
    EXTENSION_CLASSIC,
    EXTENSION_BALANCEBOARD,
    EXTENSION_PROCONTROLLER
};

class Wiimote : public Device
{
    Q_OBJECT

public:
    explicit Wiimote(char *sysPath);
    ~Wiimote();

public slots:
    void watchEvents() override;

Q_SIGNALS:
    void keyPress(int keyCode, bool pressed);

private:
    char *m_sysPath;
    struct xwii_iface *m_iface;
    struct pollfd m_fds[2];
    int m_fdsNum;
    WiimoteExtensions m_extensionType;
    int m_previousNunchukAxisTime = 0;

    void handleWatch();
    void handleKeypress(struct xwii_event *event);
    void handleNunchuk(struct xwii_event *event);

    void getExtensionType();
};
