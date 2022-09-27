/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "controllermanager.h"
#include "evdev/evdevcontroller.h"

#ifdef HAS_LIBCEC
#include "libcec/ceccontroller.h"
#endif
#ifdef HAS_XWIIMOTE
# include "wiimote/wiimotecontroller.h"
#endif // HAS_XWIIMOTE

#include <QApplication>
#include <QDebug>
#include <QDBusConnection>
#include <KLocalizedString>

#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // KStatusNotifierItem needs a QApplication
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("plasma-remotecontrollers");

    new EvdevController();

#ifdef HAS_LIBCEC
    CECController* ct = new CECController();
    ct->start();
#endif
    
#ifdef HAS_XWIIMOTE
    new WiimoteController();
#endif // HAS_XWIIMOTE

    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning() << "Cannot connect to the D-Bus session bus.\nPlease check your system settings and try again.";
        return 1;
    }

    return app.exec();
}
