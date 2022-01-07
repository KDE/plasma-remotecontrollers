/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "device.h"

#include <QDebug>

Device::Device(DeviceType deviceType, QString name, QString uniqueIdentifier)
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

QString Device::getUniqueIdentifier()
{
    return m_uniqueIdentifier;
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
