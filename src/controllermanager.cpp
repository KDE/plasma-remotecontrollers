/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "controllermanager.h"
#include "notificationsmanager.h"
#include "uinputsystem.h"
#include "kwinfakeinputsystem.h"
#include "devicesmodel.h"

#include <KWindowSystem>
#include <KSharedConfig>
#include <KConfigGroup>
#include <QDebug>

class NoOpInputSystem : public AbstractSystem
{
public:
    bool init() override { return true; }
    void emitKey(int key, bool pressed) override {
        Q_UNUSED(key)
        Q_UNUSED(pressed)
    }

};

ControllerManager::ControllerManager(QObject *parent)
    : QObject(parent)
{
    // Setup notifications
    QObject::connect(this, &ControllerManager::deviceConnected,
                     &NotificationsManager::instance(), &NotificationsManager::notifyNewDevice);
    QObject::connect(this, &ControllerManager::deviceDisconnected,
                     &NotificationsManager::instance(), &NotificationsManager::notifyDisconnectedDevice);

    m_inputSystem.reset(new UInputSystem);
    if (!m_inputSystem->init()) {
        m_inputSystem.reset(new KWinFakeInputSystem);
        if (!m_inputSystem->init()) {
            m_inputSystem.reset(new NoOpInputSystem);
        }
    }

    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-remotecontrollersrc"));
    static KConfigGroup grp(config, QLatin1String("Blacklist"));
    m_applicationBlacklist = grp.readEntry("applications", QStringList());
}

ControllerManager &ControllerManager::instance()
{
    static ControllerManager _instance;
    return _instance;
}

void ControllerManager::newDevice(Device *device)
{
    qInfo() << "New device connected:" << device->getName();

    device->setIndex(m_connectedDevices.size());

    connect(device, &Device::deviceDisconnected, this, &ControllerManager::removeDevice);

    m_connectedDevices.append(device);

    // Don't send notifications for CEC devices, since we expect them to always be available
    if (device->getDeviceType() != DeviceCEC)
        emit deviceConnected(device);
}

void ControllerManager::deviceRemoved(Device *device)
{
    qInfo() << "Device disconnected:" << device->getName();
    emit deviceDisconnected(device);
    m_connectedDevices.removeOne(device);
    for (int i = 0; i < m_connectedDevices.size(); i++) {
        m_connectedDevices[i]->setIndex(i);
    }
}

void ControllerManager::removeDevice(int deviceIndex)
{
    Device *removedDevice = m_connectedDevices.at(deviceIndex);
    m_connectedDevices.remove(deviceIndex);

    qInfo() << "Device disconnected:" << removedDevice->getName();

    emit deviceDisconnected(removedDevice);
    delete removedDevice;
    
    // Reset indexes
    for (int i = 0; i < m_connectedDevices.size(); i++)
        m_connectedDevices.at(i)->setIndex(i);
}

bool ControllerManager::isConnected(QString uniqueIdentifier)
{
    if (m_connectedDevices.size() < 1)
        return false;

    return std::find_if(m_connectedDevices.begin(), m_connectedDevices.end(), [&uniqueIdentifier](Device *other) {
        return other->getUniqueIdentifier() == uniqueIdentifier;
    }) != m_connectedDevices.end();
}

QVector<Device*> ControllerManager::getDevicesByType(DeviceType deviceType)
{
    QVector<Device*> devices;

    for (int i = 0; i < m_connectedDevices.size(); i++)
        if (m_connectedDevices.at(i)->getDeviceType() == deviceType)
            devices.append(m_connectedDevices.at(i));

    return devices;
}

QVector<Device*> ControllerManager::connectedDevices()
{
    return m_connectedDevices;
}

void ControllerManager::emitKey(int key, bool pressed) const
{
    int focusedWindowID = KWindowSystem::activeWindow();
    QString focusedWindowName = KWindowInfo(focusedWindowID, NET::WMName).name();
    for (const QString &blacklistedApplication : m_applicationBlacklist) {
        if (focusedWindowName.split(QLatin1Char(' ')).contains(blacklistedApplication, Qt::CaseInsensitive)) {
            return;
        }
    }
    
    m_inputSystem->emitKey(key, pressed);
}

ControllerManager::~ControllerManager()
{
    m_connectedDevices.clear();
}

void ControllerManager::noopInput()
{
    m_inputSystem.reset(new NoOpInputSystem);
}

