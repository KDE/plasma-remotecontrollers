/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "controllermanager.h"
#include "libcec/ceccontroller.h"

#ifdef HAS_XWIIMOTE
# include "wiimote/wiimotecontroller.h"
#endif // HAS_XWIIMOTE

#include <QGuiApplication>
#include <QDebug>

#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    CECController* ct = new CECController();
    ct->start();

#ifdef HAS_XWIIMOTE
    WiimoteController* wt = new WiimoteController();
    wt->start();
#endif // HAS_XWIIMOTE

    return app.exec();
}
