#pragma once

#include <QObject>
#include "devicetypes.h"

class NotificationsManager : public QObject
{
    Q_OBJECT

public:
    static NotificationsManager &instance();

public slots:
    void notifyNewDevice(DeviceType deviceType);
    void notifyDisconnectedDevice(DeviceType deviceType);

private:
    NotificationsManager();
};

