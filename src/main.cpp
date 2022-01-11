/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "controllermanager.h"
#include "libcec/ceccontroller.h"

#ifdef HAS_WIIUSE
# include "wiimote/wiimotecontroller.h"
#endif // HAS_WIIUSE

#include <QGuiApplication>
#include <QDebug>

#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    CECController* ct = new CECController();
    ct->start();
    
#ifdef HAS_WIIUSE
    new WiimoteController();
#endif

    return app.exec();
}
