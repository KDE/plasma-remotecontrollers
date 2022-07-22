/*
 *   SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef KCMDBUSINTERFACE_H
#define KCMDBUSINTERFACE_H

#include <QObject>
#include <QDBusMessage>

class KcmDbusInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma-remotecontrollers.KCM")

public:
    explicit KcmDbusInterface(QObject *parent = nullptr);
    ~KcmDbusInterface() override;

public Q_SLOTS:
    Q_SCRIPTABLE void evdevKeyPress(const QDBusMessage&);

Q_SIGNALS:
    void evdevKeyPressed(int keyCode);
};

#endif // KCMDBUSINTERFACE_H