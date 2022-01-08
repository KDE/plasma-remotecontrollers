#pragma once

#include <QObject>

enum DeviceType {
    DeviceCEC,
    DeviceWiimote,
    DeviceGamepad
};

class Device : public QObject
{
    Q_OBJECT

public:
    Device() = default;
    Device(DeviceType deviceType, QString name, char* uniqueIdentifier);
    ~Device();

    void setIndex(int index);
    QString getUniqueIdentifier();

    QString getName();
    DeviceType getDeviceType();

public slots:
    virtual void watchEvents() {
        while (true) { };
    };

Q_SIGNALS:
    void deviceDisconnected(int index);

protected:
    int m_index = -1;
    QString m_uniqueIdentifier;
    QString m_name;
    DeviceType m_deviceType;
};
