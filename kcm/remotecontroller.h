/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RemoteControllers_H
#define RemoteControllers_H

#include "devicesmodel.h"
#include "kcmdbusinterface.h"
#include "keymapmodel.h"
#include <KConfigGroup>
#include <KQuickConfigModule>
#include <QDBusVariant>

class KeyMapModel;
class KcmDbusInterface;
class RemoteController : public KQuickConfigModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma.remotecontrollers.KCM")
    Q_PROPERTY(KeyMapModel *keyMapModel READ keyMapModel CONSTANT)
    Q_PROPERTY(DevicesModel *devicesModel READ devicesModel CONSTANT)

public:
    explicit RemoteController(QObject *parent, const KPluginMetaData &metaData);
    ~RemoteController() override;
    KcmDbusInterface kcmDbusInterface;
    DevicesModel *devicesModel();
    KeyMapModel *keyMapModel();

public Q_SLOTS:
    QString cecKeyConfig(const QString &key);
    QString gamepadKeyConfig(const QString &key);
    void setCecKeyConfig(const QString &button, const QString &key);
    void setGamepadKeyConfig(const QString &button, const QString &key);
    void acquireNoOp();
    void releaseNoOp();
    int cecKeyFromRemotePress();

Q_SIGNALS:
    void cecConfigChanged(const QString &button);
    void gamepadConfigChanged(const QString &button);
    void gamepadKeyPressed(int keyCode);

private:
    DevicesModel *m_devicesModel;
    KeyMapModel *m_keyMapModel;
};

#endif
