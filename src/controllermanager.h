#pragma once

#include "device.h"

#include <QObject>
#include <QThread>

struct DeviceEntry {
    Device *device;
    QThread *thread;
};

class ControllerManager : public QObject
{
    Q_OBJECT

public:
    explicit ControllerManager(QObject *parent = nullptr);
    ~ControllerManager();
    static ControllerManager &instance();
    
    void newDevice(Device *device);
    bool isConnected(QString uniqueIdentifier);

public slots:
    void emitKey(int key, bool pressed);
    void removeDevice(int deviceIndex);

Q_SIGNALS:
    void deviceConnected(Device*);
    void deviceDisconnected(Device*);

private:
    int m_fd;
    QVector<DeviceEntry*> m_connectedDevices;

    void emitEvent(int type, int code, int val);
};
