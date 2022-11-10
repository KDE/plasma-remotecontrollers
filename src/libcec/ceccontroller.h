/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *   SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QDebug>
#include <QThread>

#include <libcec/cec.h>

#define LOOPTIME 50 * 1000

using namespace CEC;

class CECController : public QThread
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma-remotecontrollers.CEC")

public:
    explicit CECController();
    ~CECController() override;

    void run() override;
    void discoverDevices();

public Q_SLOTS:
    Q_SCRIPTABLE int sendNextKey();
    Q_SCRIPTABLE bool hdmiCecSupported();

Q_SIGNALS:
    void keyPress(int keyCode, bool pressed);
    void pressedKey(int keycode);

private:
    ICECAdapter* m_cecAdapter = nullptr;
    ICECCallbacks m_cecCallbacks;
    static QHash<int, int> m_keyCodeTranslation;

    static bool m_catchNextInput;
    static int m_caughtInput;
    static bool m_nativeNavMode;

    static void handleCecKeypress(void* param, const cec_keypress* key);
    static void handleCommandReceived(void* param, const cec_command* command);

    static int m_hitcommand;

    static void handleCompleteEvent(const int keycode, const int keyduration, const int opcode);
};
