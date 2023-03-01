/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *   SPDX-FileCopyrightText: 2022 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "uinputsystem.h"

#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>

#include <QDebug>

UInputSystem::UInputSystem()
{
}

bool UInputSystem::init()
{
    setSupportedKeys({});
    return m_fd > 0;
}

void UInputSystem::emitKey(int key, bool pressed)
{
    if (!m_supportedKeys.contains(key)) {
        qWarning() << "reporting unannounced key" << key;
    }
    qWarning() << "key" << key << pressed;
    emitEvent(EV_KEY, key, pressed ? 1 : 0);
    emitEvent(EV_SYN, SYN_REPORT, 0);
}

void UInputSystem::emitEvent(int type, int code, int val)
{
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.input_event_sec = 0;
    ie.input_event_usec = 0;

    write(m_fd, &ie, sizeof(ie));
}

void UInputSystem::setSupportedKeys(const QSet<int>& keys)
{
    if (m_fd > 0 && keys == m_supportedKeys) {
        return;
    }
    if (m_fd > 0) {
        close(m_fd);
    }

    m_supportedKeys = keys;

    struct uinput_setup usetup;
    m_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    if (m_fd < 0) {
        qCritical() << "Couldn't open /dev/uinput, make sure the kernel module is loaded";
        return;
    }

    // Register all keys we want to press with this application
    ioctl(m_fd, UI_SET_EVBIT, EV_KEY);

    for ( auto x : std::as_const(m_supportedKeys))
        ioctl(m_fd, UI_SET_KEYBIT, x);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; // TODO: change vendor
    usetup.id.product = 0x5678; // TODO: change product
    strcpy(usetup.name, "PlasmaRemoteControllers");

    ioctl(m_fd, UI_DEV_SETUP, &usetup);
    ioctl(m_fd, UI_DEV_CREATE);
}
