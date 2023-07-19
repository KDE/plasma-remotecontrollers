/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keydisplayprovider.h"
#include <QVariantMap>
#include <linux/input-event-codes.h>
#include <KLocalizedString>

KeyDisplayProvider::KeyDisplayProvider(QObject *parent)
    : QObject(parent)
{
    m_displayMap.append(QVariantMap({{ "key", KEY_PLAY }, { "text", i18n("Play") }, { "genericIcon", "media-playback-start" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_STOP }, { "text", i18n("Stop") }, { "genericIcon", "media-playback-stop" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_PAUSE }, { "text", i18n("Pause") }, { "genericIcon", "media-playback-pause" }, { "gamePadIcon", "media-playback-pause" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_REWIND }, { "text", i18n("Rewind") }, { "genericIcon", "media-seek-backward" }, { "gamePadIcon", "media-seek-backward" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_FASTFORWARD }, { "text", i18n("Fast Forward") }, { "genericIcon", "media-seek-forward" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_ENTER }, { "text", i18n("Enter") }, { "genericIcon", "keyboard-enter" }, { "gamePadIcon", "input-gamepad-a" }, { "psIcon", "input-gamepad-playstation-cross" }, { "cecIcon", "input-tvremote-ok" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_UP }, { "text", i18n("Up") }, { "genericIcon", "arrow-up" }, { "gamePadIcon", "input-gamepad-dpad-up" }, { "psIcon", "input-gamepad-playstation-dpad-up" }, { "cecIcon", "input-tvremote-dpad-up" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_DOWN }, { "text", i18n("Down") }, { "genericIcon", "arrow-down" }, { "gamePadIcon", "input-gamepad-dpad-down" }, { "psIcon", "input-gamepad-playstation-dpad-down" }, { "cecIcon", "input-tvremote-dpad-down" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_RIGHT }, { "text", i18n("Right") }, { "genericIcon", "arrow-right" }, { "gamePadIcon", "input-gamepad-dpad-right" }, { "psIcon", "input-gamepad-playstation-dpad-right" }, { "cecIcon", "input-tvremote-dpad-right" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_LEFT }, { "text", i18n("Left") }, { "genericIcon", "arrow-left" }, { "gamePadIcon", "input-gamepad-dpad-left" }, { "psIcon", "input-gamepad-playstation-dpad-left" }, { "cecIcon", "input-tvremote-dpad-left" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_0 }, { "text", i18n("Button 0") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_1 }, { "text", i18n("Button 1") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_2 }, { "text", i18n("Button 2") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_3 }, { "text", i18n("Button 3") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_4 }, { "text", i18n("Button 4") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_5 }, { "text", i18n("Button 5") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_6 }, { "text", i18n("Button 6") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_7 }, { "text", i18n("Button 7") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_8 }, { "text", i18n("Button 8") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_9 }, { "text", i18n("Button 9") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_BLUE }, { "text", i18n("Button Blue") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_GREEN }, { "text", i18n("Button Green") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_YELLOW }, { "text", i18n("Button Yellow") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_RED }, { "text", i18n("Button Red") }, { "genericIcon", "" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_CHANNELUP}, { "text", i18n("Channel Up") }, { "genericIcon", "arrow-up-double" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_CHANNELDOWN}, { "text", i18n("Channel Down") }, { "genericIcon", "arrow-down-double" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_EXIT }, { "text", i18n("Exit") }, { "genericIcon", "application-exit" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_BACK }, { "text", i18n("Back") }, { "genericIcon", "go-previous" }, { "gamePadIcon", "input-gamepad-rt" }, { "psIcon", "input-gamepad-playstation-r2" }, { "cecIcon", "input-tvremote-back" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_HOME }, { "text", i18n("Home") }, { "genericIcon", "go-home" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "input-tvremote-home" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_HOMEPAGE }, { "text", i18n("Home") }, { "genericIcon", "go-home" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "input-tvremote-home" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_MENU }, { "text", i18n("Menu") }, { "genericIcon", "menu" }, { "gamePadIcon", "input-gamepad-xbox-series-menu" }, { "psIcon", "input-gamepad-playstation-dualsense-options" }, { "cecIcon", "input-tvremote-menu" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_INFO }, { "text", i18n("Info") }, { "genericIcon", "dialog-information" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_SUBTITLE }, { "text", i18n("Subtitle") }, { "genericIcon", "media-standard-track" }, { "gamePadIcon", "" }, { "psIcon", "" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_LEFTMETA }, { "text", i18n("Left Meta") }, { "genericIcon", "code-function" }, { "gamePadIcon", "input-gamepad-xbox-series-view" }, { "psIcon", "input-gamepad-playstation-dualsense-create" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_GAMES }, { "text", i18n("Games") }, { "genericIcon", "applications-games" }, { "gamePadIcon", "input-gamepad-xbox-360-start" }, { "psIcon", "input-gamepad-playstation-dualshock-3-start" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", KEY_CANCEL }, { "text", i18n("Cancel") }, { "genericIcon", "dialog-cancel" }, { "gamePadIcon", "input-gamepad-b" }, { "psIcon", "input-gamepad-circle" }, { "cecIcon", "" } }));

    m_displayMap.append(QVariantMap({{ "key", BTN_MODE }, { "text", i18n("Mode") }, { "genericIcon", "input-mouse" }, { "gamePadIcon", "input-gamepad-xbox-series-view" }, { "psIcon", "input-gamepad-playstation-dualsense-create" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_START }, { "text", i18n("Start") }, { "genericIcon", "media-playback-start" }, { "gamePadIcon", "input-gamepad-xbox-360-start" }, { "psIcon", "input-gamepad-playstation-dualshock-3-start" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_NORTH }, { "text", i18n("Gamepad X / Gamepad Square") }, { "genericIcon", "input-gamepad-x" }, { "gamePadIcon", "input-gamepad-x" }, { "psIcon", "input-gamepad-playstation-square" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_SOUTH }, { "text", i18n("Gamepad A / Gamepad Cross") }, { "genericIcon", "input-gamepad-a" }, { "gamePadIcon", "input-gamepad-a" }, { "psIcon", "input-gamepad-playstation-cross" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_EAST }, { "text", i18n("Gamepad B / Gamepad Circle") }, { "genericIcon", "input-gamepad-b" }, { "gamePadIcon", "input-gamepad-b" }, { "psIcon", "input-gamepad-playstation-circle" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_WEST }, { "text", i18n("Gamepad Y / Gamepad Triangle") }, { "genericIcon", "input-gamepad-y" }, { "gamePadIcon", "input-gamepad-y" }, { "psIcon", "input-gamepad-playstation-triangle" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_TL }, { "text", i18n("Trigger Left-1") }, { "genericIcon", "input-gamepad-lb" }, { "gamePadIcon", "input-gamepad-lb" }, { "psIcon", "input-gamepad-playstation-l1" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_TR }, { "text", i18n("Trigger Right-1") }, { "genericIcon", "input-gamepad-rb" }, { "gamePadIcon", "input-gamepad-rb" }, { "psIcon", "input-gamepad-playstation-r1" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_TL2 }, { "text", i18n("Trigger Left-2") }, { "genericIcon", "input-gamepad-lt" }, { "gamePadIcon", "input-gamepad-lt" }, { "psIcon", "input-gamepad-playstation-l2" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_TR2 }, { "text", i18n("Trigger Right-2") }, { "genericIcon", "input-gamepad-rt" }, { "gamePadIcon", "input-gamepad-rt" }, { "psIcon", "input-gamepad-playstation-r2" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_DPAD_UP }, { "text", i18n("DPad Up") }, { "genericIcon", "input-gamepad-dpad-up" }, { "gamePadIcon", "input-gamepad-dpad-up" }, { "psIcon", "input-gamepad-playstation-dpad-up" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_DPAD_DOWN }, { "text", i18n("DPad Down") }, { "genericIcon", "input-gamepad-dpad-down" }, { "gamePadIcon", "input-gamepad-dpad-down" }, { "psIcon", "input-gamepad-playstation-dpad-down" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_DPAD_LEFT }, { "text", i18n("DPad Left") }, { "genericIcon", "input-gamepad-dpad-left" }, { "gamePadIcon", "input-gamepad-dpad-left" }, { "psIcon", "input-gamepad-playstation-dpad-left" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_DPAD_RIGHT }, { "text", i18n("DPad Right") }, { "genericIcon", "input-gamepad-dpad-right" }, { "gamePadIcon", "input-gamepad-dpad-right" }, { "psIcon", "input-gamepad-playstation-dpad-right" }, { "cecIcon", "" } }));
    m_displayMap.append(QVariantMap({{ "key", BTN_SELECT }, { "text", i18n("Select") }, { "genericIcon", "input-gamepad-xbox-series-menu" }, { "gamePadIcon", "input-gamepad-xbox-series-menu" }, { "psIcon", "input-gamepad-playstation-dualsense-options" }, { "cecIcon", "" } }));
}

KeyDisplayProvider::~KeyDisplayProvider()
{

}

QString KeyDisplayProvider::keyDisplay(const int &key) const
{
    for (const QVariantMap &map : m_displayMap) {
        if (map.value("key") == key) {
            return map.value("text").toString();
        }
    }
    return QString();
}

QString KeyDisplayProvider::keyIcon(const int &key, const QString &deviceType) const
{
    for (const QVariantMap &map : m_displayMap) {
        if (map.value("key") == key) {
            if(deviceType == "Gamepad" || deviceType == "Xbox" || deviceType == "Wii") {
                if(!map.value("gamePadIcon").toString().isEmpty()) {
                    return map.value("gamePadIcon").toString();
                } else {
                    return map.value("genericIcon").toString();
                }
            } else if(deviceType == "playstation") {
                if(!map.value("psIcon").toString().isEmpty()) {
                    return map.value("psIcon").toString();
                } else {
                    return map.value("genericIcon").toString();
                }
            } else if(deviceType == "CEC") {
                if(!map.value("cecIcon").toString().isEmpty()) {
                    return map.value("cecIcon").toString();
                } else {
                    return map.value("genericIcon").toString();
                }
            } else {
                return map.value("genericIcon").toString();
            }
        }
    }
    return QString();
}

#include "moc_keydisplayprovider.cpp"
