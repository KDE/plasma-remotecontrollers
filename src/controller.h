#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QThread>

#include "devicetypes.h"

class Controller : public QThread
{
    Q_OBJECT

signals:
    void deviceConnected(DeviceType deviceType);
    void deviceDisconnected(DeviceType deviceType);
};

#endif // CONTROLLER_H
