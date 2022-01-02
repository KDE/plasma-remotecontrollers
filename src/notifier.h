#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QObject>
#include "devicetypes.h"

class Notifier : public QObject
{
    Q_OBJECT

public:
    explicit Notifier();
    ~Notifier() override;

public slots:
    void notifyNewDevice(DeviceType deviceType);
    void notifyDisconnectedDevice(DeviceType deviceType);
};

#endif // NOTIFIER_H

