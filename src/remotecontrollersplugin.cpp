/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QQmlEngine>
#include <QQmlContext>
#include "remotecontrollersplugin.h"
#include "controllermanager.h"
#include "devicesmodel.h"

static DevicesModel *devicesModelSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);
    engine->setObjectOwnership(DevicesModel::instance(), QQmlEngine::CppOwnership);
    return DevicesModel::instance();
}

RemoteControllersPlugin::RemoteControllersPlugin(QObject *parent)
    : QQmlExtensionPlugin(parent)
{

    m_evdevController = new EvdevController();

#ifdef HAS_LIBCEC
    m_cecController = new CECController();
#endif

#ifdef HAS_XWIIMOTE
    m_wiimoteController = new WiimoteController();
#endif // HAS_XWIIMOTE

    ControllerManager::instance().noopInput();
}

void RemoteControllersPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.remotecontrollers"));
    qmlRegisterSingletonType<DevicesModel>(uri, 1, 0, "DevicesModel", devicesModelSingletonProvider);
}
