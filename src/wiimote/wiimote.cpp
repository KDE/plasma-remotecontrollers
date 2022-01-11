/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "wiimote.h"
#include "../controllermanager.h"

#include <QLoggingCategory>
#include <QDebug>

#include <linux/input-event-codes.h>
#include <unistd.h>

Wiimote::Wiimote(wiimote* wiiptr)
{
    m_wiiptr = wiiptr;
    m_uniqueIdentifier = wiiptr->bdaddr_str;
    m_name = "Wiimote";
    m_deviceType = DeviceWiimote;
    
    switch (ControllerManager::instance().getDevicesByType(DeviceWiimote).size()) {
        case 0:
            wiiuse_set_leds(m_wiiptr, WIIMOTE_LED_1);
            break;
        case 1:
            wiiuse_set_leds(m_wiiptr, WIIMOTE_LED_2);
            break;
        case 2:
            wiiuse_set_leds(m_wiiptr, WIIMOTE_LED_3);
            break;
        case 3:
            wiiuse_set_leds(m_wiiptr, WIIMOTE_LED_4);
            break;
    }
    
    QObject::connect(this, &Wiimote::keyPress, &ControllerManager::instance(), &ControllerManager::emitKey);
    
    // Rumble shortly so the user knows the device is being used
    wiiuse_rumble(m_wiiptr, 1);
    usleep(200 * 1000);
    wiiuse_rumble(m_wiiptr, 0);
}

void Wiimote::watchEvents()
{
    if (wiiuse_poll(&m_wiiptr, 1)) {
        switch (m_wiiptr->event) {
            case WIIUSE_DISCONNECT:
            case WIIUSE_UNEXPECTED_DISCONNECT:
                ControllerManager::instance().removeDevice(this->m_index);
                break;
            case WIIUSE_EVENT:
                handleEvent();
                break;
            default:
                break;
        }
    }
}

void Wiimote::handleEvent()
{
    static QHash<int, int> keyCodeTranslationNoExpansion = {
        { WIIMOTE_BUTTON_UP, KEY_LEFT},
        { WIIMOTE_BUTTON_DOWN, KEY_RIGHT},
        { WIIMOTE_BUTTON_LEFT, KEY_DOWN},
        { WIIMOTE_BUTTON_RIGHT, KEY_UP},
        { WIIMOTE_BUTTON_ONE, KEY_BACK},
        { WIIMOTE_BUTTON_TWO, KEY_ENTER},
        { WIIMOTE_BUTTON_PLUS, KEY_VOLUMEUP},
        { WIIMOTE_BUTTON_MINUS, KEY_VOLUMEDOWN},
        { WIIMOTE_BUTTON_HOME, KEY_HOME},
    };
    static QHash<int, int> keyCodeTranslationNunchuk = {
        { WIIMOTE_BUTTON_A, KEY_ENTER},
        { WIIMOTE_BUTTON_B, KEY_BACK},
        { WIIMOTE_BUTTON_UP, KEY_UP},
        { WIIMOTE_BUTTON_DOWN, KEY_DOWN},
        { WIIMOTE_BUTTON_LEFT, KEY_LEFT},
        { WIIMOTE_BUTTON_RIGHT, KEY_RIGHT},
        { WIIMOTE_BUTTON_ONE, KEY_1},
        { WIIMOTE_BUTTON_TWO, KEY_2},
        { WIIMOTE_BUTTON_PLUS, KEY_VOLUMEUP},
        { WIIMOTE_BUTTON_MINUS, KEY_VOLUMEDOWN},
        { WIIMOTE_BUTTON_HOME, KEY_HOME},
    };
    
    QHash<int, int> keyCodeTranslation;
    switch (m_wiiptr->exp.type) {
        case EXP_NONE:
        case EXP_MOTION_PLUS:
            keyCodeTranslation = keyCodeTranslationNoExpansion;
            break;
        case EXP_NUNCHUK:
        case EXP_CLASSIC:
        case EXP_GUITAR_HERO_3:
        case EXP_WII_BOARD:
        case EXP_MOTION_PLUS_CLASSIC:
        case EXP_MOTION_PLUS_NUNCHUK:
            keyCodeTranslation = keyCodeTranslationNunchuk;
            break;
    }

    QHash<int, int>::const_iterator i;
    for (i = keyCodeTranslation.constBegin(); i != keyCodeTranslation.constEnd(); ++i) {
        if (IS_JUST_PRESSED(m_wiiptr, i.key()))
            emit keyPress(i.value(), true);
        else if (IS_RELEASED(m_wiiptr, i.key()))
            emit keyPress(i.value(), false);
    }
}
