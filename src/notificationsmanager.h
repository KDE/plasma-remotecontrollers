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

