/*
 *   SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "kcmdbusinterface.h"
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDebug>

KcmDbusInterface::KcmDbusInterface(QObject *parent)
    : QObject(parent)
{
    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.plasma.remotecontrollers")) {
        QDBusConnection::sessionBus().registerService("org.kde.plasma.remotecontrollers");
    }
    QDBusConnection::sessionBus().registerObject("/KCM", this, QDBusConnection::ExportAllSlots);
    QDBusConnectionInterface *iface = QDBusConnection::sessionBus().interface();
    QDBusConnection dbus = QDBusConnection::sessionBus();

    if (iface) {
        if (iface->isServiceRegistered("org.kde.plasma.remotecontrollers")) {
            if (dbus.isConnected()) {
                dbus.connect("org.kde.plasma.remotecontrollers", "/EVDEV", "org.kde.plasma.remotecontrollers.EVDEV", "keyPress", this, SLOT(evdevKeyPress(const QDBusMessage&)));
            } else {
                qWarning() << "DBus connection is not connected.";                    
            }
        }
    }
}

KcmDbusInterface::~KcmDbusInterface()
{
}

void KcmDbusInterface::evdevKeyPress(const QDBusMessage& msg)
{
    QList<QVariant> arguments = msg.arguments();
    int keyCode = arguments.at(0).toInt();
    emit evdevKeyPressed(keyCode);
}
