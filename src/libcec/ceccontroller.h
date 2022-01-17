/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
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

public Q_SLOTS:
    Q_SCRIPTABLE int sendNextKey();

Q_SIGNALS:
    void keyPress(int keyCode, bool pressed);
    void pressedKey(int keycode);

private:
    ICECAdapter* m_cecAdapter = nullptr;
    ICECCallbacks m_cecCallbacks;
    static QHash<int, int> m_keyCodeTranslation;
    
    static bool m_catchNextInput;
    static int m_caughtInput;

    static void handleCecKeypress(void* param, const cec_keypress* key);
};

