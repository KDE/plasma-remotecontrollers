/*
 *   SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef EVDEVDBUSINTERFACE_H
#define EVDEVDBUSINTERFACE_H

#include <QObject>

class EvdevDbusInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma-remotecontrollers.EVDEV")

public:
    explicit EvdevDbusInterface(QObject *parent = nullptr);
    ~EvdevDbusInterface() override;


public Q_SLOTS:
    Q_SCRIPTABLE void emitKeyPress(int keyCode);

Q_SIGNALS:
    void keyPress(int keyCode);
};

#endif // EVDEVDBUSINTERFACE_H