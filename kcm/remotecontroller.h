/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RemoteControllers_H
#define RemoteControllers_H

#include <KQuickAddons/ConfigModule>
#include <KConfigGroup>
#include <QDBusVariant>
#include "kcmdbusinterface.h"
#include "keymapmodel.h"

class KeyMapModel;
class KcmDbusInterface;
class RemoteController : public KQuickAddons::ConfigModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma.remotecontrollers.KCM")
    Q_PROPERTY(KeyMapModel *keyMapModel READ keyMapModel CONSTANT)

public:
    explicit RemoteController(QObject *parent = nullptr, const QVariantList &list = QVariantList());
    ~RemoteController() override;
    KcmDbusInterface kcmDbusInterface;
    KeyMapModel *keyMapModel();

public Q_SLOTS:
    QString cecKeyConfig(const QString &key);
    QString gamepadKeyConfig(const QString &key);
    void setCecKeyConfig(const QString &button, const QString &key);
    void setGamepadKeyConfig(const QString &button, const QString &key);
    int cecKeyFromRemotePress();

Q_SIGNALS:
    void cecConfigChanged(const QString button);
    void gamepadConfigChanged(const QString button);
    void gamepadKeyPressed(int keyCode);

private:
    KeyMapModel *m_keyMapModel;
};

#endif
