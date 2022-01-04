#pragma once

#include "devicetypes.h"

#include <QObject>

class ControllerManager : public QObject
{
    Q_OBJECT

public:
    explicit ControllerManager(QObject *parent = nullptr);
    virtual ~ControllerManager();
    static ControllerManager &instance();
    
    int newDevice(DeviceType deviceType);
    void removeDevice(int deviceIndex);

public slots:
    void emitKey(int key, bool pressed);

Q_SIGNALS:
    void deviceConnected(DeviceType);
    void deviceDisconnected(DeviceType);

private:
    int m_fd;
    QList<DeviceType> m_connectedDevices;

    void emitEvent(int type, int code, int val);
};
