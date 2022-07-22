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

static constexpr std::array<int, 41> s_supportedKeys = {
                  KEY_PLAY
                , KEY_STOP
                , KEY_PAUSE
                , KEY_REWIND
                , KEY_FASTFORWARD
                , KEY_ENTER
                , KEY_CANCEL
                , KEY_SELECT
                , KEY_UP
                , KEY_DOWN
                , KEY_LEFT
                , KEY_RIGHT
                , KEY_0
                , KEY_1
                , KEY_2
                , KEY_3
                , KEY_4
                , KEY_5
                , KEY_6
                , KEY_7
                , KEY_8
                , KEY_9
                , KEY_BLUE
                , KEY_RED
                , KEY_GREEN
                , KEY_YELLOW
                , KEY_CHANNELUP
                , KEY_CHANNELDOWN
                , KEY_VOLUMEUP
                , KEY_VOLUMEDOWN
                , KEY_EXIT
                , KEY_BACK
                , KEY_HOME
                , KEY_MENU
                , KEY_SUBTITLE
                , KEY_MINUS
                , KEY_FORWARD
                , KEY_LEFTMETA
                , KEY_TAB
                , KEY_LEFTSHIFT
                , KEY_LEFTALT
};


UInputSystem::UInputSystem()
{
}

bool UInputSystem::init()
{
    struct uinput_setup usetup;
    m_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    if (m_fd < 0) {
        qCritical() << "Couldn't open /dev/uinput, make sure the kernel module is loaded";
        return false;
    }

    // Register all keys we want to press with this application
    ioctl(m_fd, UI_SET_EVBIT, EV_KEY);

    for ( auto x : s_supportedKeys)
        ioctl(m_fd, UI_SET_KEYBIT, x);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; // TODO: change vendor
    usetup.id.product = 0x5678; // TODO: change product
    strcpy(usetup.name, "PlasmaRemoteControllers");

    ioctl(m_fd, UI_DEV_SETUP, &usetup);
    ioctl(m_fd, UI_DEV_CREATE);
    sleep(1);
    return true;
}

void UInputSystem::emitKey(int key, bool pressed)
{
    if (std::find(s_supportedKeys.cbegin(), s_supportedKeys.cend(), key) == s_supportedKeys.cend()) {
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
