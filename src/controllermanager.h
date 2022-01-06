#pragma once

#include "device.h"

#include <QObject>

class ControllerManager : public QObject
{
    Q_OBJECT

public:
    explicit ControllerManager(QObject *parent = nullptr);
    ~ControllerManager() = default;
    static ControllerManager &instance();
    
    int newDevice(Device *device);
    void removeDevice(int deviceIndex);
    bool isConnected(char *uniqueIdentifier);

public slots:
    void emitKey(int key, bool pressed);

Q_SIGNALS:
    void deviceConnected(Device*);
    void deviceDisconnected(Device*);

private:
    int m_fd;
    QVector<Device*> m_connectedDevices;

    void emitEvent(int type, int code, int val);
};
