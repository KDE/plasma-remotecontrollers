/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QQmlEngine>
#include "remotecontrollersplugin.h"
#include "devicesmodel.h"

RemoteControllersPlugin::RemoteControllersPlugin(QObject *parent)
    : QQmlExtensionPlugin(parent)
{
}

void RemoteControllersPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.remotecontrollers"));
    qmlRegisterType<DevicesModel>(uri, 1, 0, "DevicesModel");
}