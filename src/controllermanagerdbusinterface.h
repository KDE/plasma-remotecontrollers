/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef CONTROLLERMANAGERDBUSINTERFACE_H
#define CONTROLLERMANAGERDBUSINTERFACE_H

#include <QObject>
#include "controllermanager.h"

class ControllerManagerDBusInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma.remotecontrollers.ControllerManager")
    
public:
    explicit ControllerManagerDBusInterface(QObject *parent = nullptr);
    ~ControllerManagerDBusInterface() override;

public Q_SLOTS:
    Q_SCRIPTABLE QStringList getConnectedDevices();
    Q_SCRIPTABLE QString getDeviceName(const QString &uniqueIdentifier);
    Q_SCRIPTABLE int getDeviceType(const QString &uniqueIdentifier);
    Q_SCRIPTABLE QString getDeviceIconName(const QString &uniqueIdentifier);
    Q_SCRIPTABLE void setNoop();
    Q_SCRIPTABLE void releaseNoop();

Q_SIGNALS:
    void deviceConnected(const QString &uniqueIdentifier);
    void deviceDisconnected(const QString &uniqueIdentifier);
};

#endif // CONTROLLERMANAGERDBUSINTERFACE_H