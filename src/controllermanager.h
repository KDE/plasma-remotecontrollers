#pragma once

#include "device.h"

#include <QObject>
#include <QThread>

class ControllerManager : public QObject
{
    Q_OBJECT

public:
    explicit ControllerManager(QObject *parent = nullptr);
    ~ControllerManager();
    static ControllerManager &instance();
    
    void newDevice(Device *device);
    bool isConnected(QString uniqueIdentifier);
    QVector<Device*> getDevicesByType(DeviceType deviceType);

public slots:
    void emitKey(int key, bool pressed);
    void removeDevice(int deviceIndex);

Q_SIGNALS:
    void deviceConnected(Device*);
    void deviceDisconnected(Device*);

private:
    int m_fd;
    QVector<Device*> m_connectedDevices;

    void emitEvent(int type, int code, int val);
};
