/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *   SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QDebug>
#include <QObject>

#include <libcec/cec.h>

#define LOOPTIME 50 * 1000

using namespace CEC;

class CECController : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma.remotecontrollers.CEC")

public:
    explicit CECController();
    ~CECController() override;

    void discoverDevices();

public Q_SLOTS:
    Q_SCRIPTABLE int sendNextKey();
    Q_SCRIPTABLE bool hdmiCecSupported();
    Q_SCRIPTABLE bool sendKey(uchar, CEC::cec_logical_address address = CECDEVICE_TV);
    Q_SCRIPTABLE bool powerOnDevices(CEC::cec_logical_address address = CECDEVICE_TV);
    Q_SCRIPTABLE bool powerOffDevices(CEC::cec_logical_address address = CECDEVICE_BROADCAST);
    Q_SCRIPTABLE bool makeActiveSource();
    Q_SCRIPTABLE bool setOSDName(const QString&);

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

Q_DECLARE_METATYPE(cec_logical_address);
