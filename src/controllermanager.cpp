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

#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KStatusNotifierItem>
#include <QAction>
#include <QDebug>
#include <QMenu>

#include <taskmanager/tasksmodel.h>
#include <taskmanager/abstracttasksmodel.h>

#include "remotecontrollers.h"

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
    , m_settings(new RemoteControllersSettings)
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

    m_sni = new KStatusNotifierItem(QStringLiteral("org.kde.plasma.remotecontrollers"), this);
    m_sni->setIconByName("input-gamepad");
    m_sni->setStandardActionsEnabled(false);
    m_sni->setContextMenu(new QMenu);
    m_sni->setToolTipTitle(i18n("Plasma Remote Controllers"));
    m_sni->setToolTipSubTitle(i18n("Configure Inhibited Apps"));

    auto section = new QAction(i18n("Inhibitions"));
    section->setEnabled(false);
    m_sni->contextMenu()->addAction(section);

    m_appsModel = new TaskManager::TasksModel(this);
    connect(m_appsModel, &TaskManager::TasksModel::activeTaskChanged, this, [this] {
        const QString appId = m_appsModel->activeTask().data(TaskManager::AbstractTasksModel::AppId).toString();
        m_enabled = !m_settings->applications().contains(appId);
    });
    connect(m_appsModel, &TaskManager::TasksModel::rowsAboutToBeRemoved, this, &ControllerManager::refreshApps);
    connect(m_appsModel, &TaskManager::TasksModel::rowsInserted, this, &ControllerManager::refreshApps);
    connect(m_appsModel, &TaskManager::TasksModel::modelReset, this, &ControllerManager::refreshApps);
    refreshApps();
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

    m_sni->setStatus(KStatusNotifierItem::Active);
}

void ControllerManager::deviceRemoved(Device *device)
{
    qInfo() << "Device disconnected:" << device->getName();
    emit deviceDisconnected(device);
    m_connectedDevices.removeOne(device);
    for (int i = 0; i < m_connectedDevices.size(); i++) {
        m_connectedDevices[i]->setIndex(i);
    }

    m_sni->setStatus(m_connectedDevices.count() > 0 ? KStatusNotifierItem::Active : KStatusNotifierItem::Passive);
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
    if (!m_enabled)
        return;

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

void ControllerManager::refreshApps()
{
    auto menu = m_sni->contextMenu();
    auto actions = menu->actions();
    for (int i = 0, c = m_appsModel->rowCount(); i < c; ++i) {
        const auto task = m_appsModel->index(i, 0);
        auto appId = task.data(TaskManager::AbstractTasksModel::AppId).toString();
        auto &action = m_appActions[appId];
        if (action) {
            actions.removeAll(action);
        } else {
            QString appName = task.data(TaskManager::AbstractTasksModel::AppName).toString();
            if (appName.isNull()) {
                appName = task.data(Qt::DisplayRole).toString();
            }
            action = new QAction(appName);
            action->setCheckable(true);
            action->setChecked(m_settings->applications().contains(appId));
            action->setProperty("appId", appId);
            connect(action, &QAction::toggled, this, [this, action] (bool checked) {
                if (checked) {
                    auto apps = m_settings->applications();
                    apps += action->property("appId").toString();
                    m_settings->setApplications(apps);
                } else {
                    auto apps = m_settings->applications();
                    apps.removeAll(action->property("appId").toString());
                    m_settings->setApplications(apps);
                }
                m_settings->save();
            });
            m_appActions[appId] = action;
            menu->addAction(action);
        }
    }
    for (auto action : actions) {
        menu->removeAction(action);
        delete m_appActions.take(action->property("appId").toString());
    }
}
