/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QObject>

#include <unistd.h>

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
    Device(DeviceType deviceType, QString name, QString uniqueIdentifier);
    ~Device();

    void setIndex(int index);
    QString getUniqueIdentifier();

    QString getName();
    DeviceType getDeviceType();
    QString iconName() const;

public slots:
    virtual void watchEvents() { return; };

Q_SIGNALS:
    void deviceDisconnected(int index);

protected:
    int m_index = -1;
    QString m_uniqueIdentifier;
    QString m_name;
    DeviceType m_deviceType;
};
