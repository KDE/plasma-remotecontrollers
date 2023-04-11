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

enum WiimoteDevtypes {
    WIIMOTE_DEVTYPE_UNKNOWN,
    WIIMOTE_DEVTYPE_GENERIC,
    WIIMOTE_DEVTYPE_GEN10,
    WIIMOTE_DEVTYPE_GEN15,
    WIIMOTE_DEVTYPE_GEN20,
    WIIMOTE_DEVTYPE_BALANCEBOARD,
    WIIMOTE_DEVTYPE_PROCONTROLLER
};

enum WiimoteExtensions {
    WIIMOTE_EXTENSION_NONE,
    WIIMOTE_EXTENSION_UNKNOWN,
    WIIMOTE_EXTENSION_NUNCHUK,
    WIIMOTE_EXTENSION_CLASSIC,
    WIIMOTE_EXTENSION_BALANCEBOARD,
    WIIMOTE_EXTENSION_PROCONTROLLER
};

class Wiimote : public Device
{
    Q_OBJECT

public:
    explicit Wiimote(struct xwii_iface *iface, const QString &sysPath);
    ~Wiimote();

    WiimoteDevtypes getDevType();

public Q_SLOTS:
    void watchEvents() override;

Q_SIGNALS:
    void keyPress(int keyCode, bool pressed);

private:
    struct xwii_iface *m_iface;
    struct pollfd m_fds[2];
    int m_fdsNum;
    WiimoteDevtypes m_devType;
    WiimoteExtensions m_extensionType;
    int m_previousNunchukAxisTime = 0;

    void handleWatch();
    void handleKeypress(struct xwii_event *event);
    void handleNunchuk(struct xwii_event *event);

    void getExtensionType();
};
