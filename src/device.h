#pragma once

#include <QThread>

enum DeviceType {
    DeviceCEC,
    DeviceWiimote,
    DeviceGamepad
};

class Device : public QThread
{
    Q_OBJECT

public:
    Device() = default;
    Device(DeviceType deviceType, QString name, char* uniqueIdentifier);
    ~Device();

    void setIndex(int index);
    char& getUniqueIdentifier();

    QString getName();
    DeviceType getDeviceType();

protected:
    int m_index = -1;
    char* m_uniqueIdentifier;
    QString m_name;
    DeviceType m_deviceType;
};
