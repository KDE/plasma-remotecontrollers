#include "device.h"

#include <QDebug>

Device::Device(DeviceType deviceType, QString name, char *uniqueIdentifier)
{
    m_deviceType = deviceType;
    m_name = name;
    m_uniqueIdentifier = uniqueIdentifier;
}

void Device::setIndex(int index)
{
    if (m_index < 0) {
        m_index = index;
    }
}

char& Device::getUniqueIdentifier()
{
    return *m_uniqueIdentifier;
}

QString Device::getName()
{
    return m_name;
}

DeviceType Device::getDeviceType()
{
    return m_deviceType;
}

Device::~Device() = default;
