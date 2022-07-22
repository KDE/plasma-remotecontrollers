/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef REMOTECONTROLLERSPLUGIN_H
#define REMOTECONTROLLERSPLUGIN_H

#include <QQmlExtensionPlugin>
#ifdef HAS_LIBCEC
#include "libcec/ceccontroller.h"
#endif
#include "evdev/evdevcontroller.h"

#ifdef HAS_XWIIMOTE
# include "wiimote/wiimotecontroller.h"
#endif // HAS_XWIIMOTE

class RemoteControllersPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    explicit RemoteControllersPlugin(QObject *parent = nullptr);
    void registerTypes(const char *uri) override;

private:
    EvdevController *m_evdevController;
#ifdef HAS_LIBCEC
    CECController *m_cecController;
#endif

#ifdef HAS_XWIIMOTE
    WiimoteController *m_wiimoteController;
#endif // HAS_XWIIMOTE

};

#endif // REMOTECONTROLLERSPLUGIN_H
