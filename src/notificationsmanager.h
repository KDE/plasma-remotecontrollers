/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include "device.h"

#include <QObject>

class NotificationsManager : public QObject
{
    Q_OBJECT

public:
    static NotificationsManager &instance();

public slots:
    void notifyNewDevice(Device* device);
    void notifyDisconnectedDevice(Device* device);

private:
    NotificationsManager();
};

