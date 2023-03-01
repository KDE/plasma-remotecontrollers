/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "controllermanager.h"
#include "notificationsmanager.h"
#include "uinputsystem.h"
#include "kwinfakeinputsystem.h"

#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KStatusNotifierItem>
#include <KWindowSystem>
#include <QAction>
#include <QMenu>

#include <taskmanager/tasksmodel.h>
#include <taskmanager/abstracttasksmodel.h>

#include "remotecontrollers.h"
#include "screensaver_interface.h"
#include "plasmarc-debug.h"

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
    , m_dbusInterface(new ControllerManagerDBusInterface(this))
    , m_settings(new RemoteControllersSettings)
{
    // Setup notifications
    QObject::connect(this, &ControllerManager::deviceConnected,
                     &NotificationsManager::instance(), &NotificationsManager::notifyNewDevice);
    QObject::connect(this, &ControllerManager::deviceDisconnected,
                     &NotificationsManager::instance(), &NotificationsManager::notifyDisconnectedDevice);

    resetInputSystem();

    m_sni = new KStatusNotifierItem(QStringLiteral("org.kde.plasma.remotecontrollers"), this);
    m_sni->setIconByName("input-gamepad");
    m_sni->setStandardActionsEnabled(false);
    m_sni->setContextMenu(new QMenu);
    m_sni->setTitle(i18n("Remote Controllers"));
    m_sni->setToolTipTitle(m_sni->title());
    m_sni->setToolTipSubTitle(i18n("Configure Inhibited Apps"));

    auto section = new QAction(i18n("Inhibitions"));
    section->setEnabled(false);
    m_sni->contextMenu()->addAction(section);

    m_appsModel = new TaskManager::TasksModel(this);
    connect(m_appsModel, &TaskManager::TasksModel::activeTaskChanged, this, [this] {
        const QString appId = m_appsModel->activeTask().data(TaskManager::AbstractTasksModel::AppId).toString();
        m_enabled = !appInhibited(appId);
    });
    connect(m_appsModel, &TaskManager::TasksModel::rowsAboutToBeRemoved, this, &ControllerManager::refreshApps);
    connect(m_appsModel, &TaskManager::TasksModel::rowsInserted, this, &ControllerManager::refreshApps);
    connect(m_appsModel, &TaskManager::TasksModel::modelReset, this, &ControllerManager::refreshApps);

    // Mark the SNI as passive after 10' without usage
    m_lastUsed.setInterval(10 * 60 * 1000);
    m_lastUsed.setSingleShot(true);
    connect(&m_lastUsed, &QTimer::timeout, this, [this] {
        m_sni->setStatus(KStatusNotifierItem::Passive);
    });
    
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

    m_usedKeys += device->usedKeys();
    m_inputSystem->setSupportedKeys(m_usedKeys);

    device->setIndex(m_connectedDevices.size());

    connect(device, &Device::deviceDisconnected, this, &ControllerManager::removeDevice);

    m_connectedDevices.append(device);

    // Don't send notifications for CEC devices, since we expect them to always be available
    if (device->getDeviceType() != DeviceCEC) {
        emit deviceConnected(device);
        emit m_dbusInterface->deviceConnected(device->getUniqueIdentifier());
    }
    m_lastUsed.start();
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
    emit m_dbusInterface->deviceDisconnected(device->getUniqueIdentifier());

    m_sni->setStatus(m_connectedDevices.count() > 0 ? KStatusNotifierItem::Active : KStatusNotifierItem::Passive);
    m_lastUsed.start();
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

void ControllerManager::emitKey(int key, bool pressed)
{
    m_sni->setStatus(KStatusNotifierItem::Active);
    m_lastUsed.start();
    if (!m_enabled) {
        // Make sure the system knows that it's being interacted with
        simulateUserActivity();
        return;
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

void ControllerManager::resetInputSystem()
{
    m_inputSystem.reset();
    std::unique_ptr<AbstractSystem> inputSystem(new UInputSystem);
    if (inputSystem->init()) {
        m_inputSystem.reset(inputSystem.release());
    }

    if (!m_inputSystem && KWindowSystem::isPlatformWayland()) {
        std::unique_ptr<AbstractSystem> inputSystem(new KWinFakeInputSystem);
        if (inputSystem->init()) {
            m_inputSystem.reset(inputSystem.release());
        }
    }
    if (!m_inputSystem) {
        m_inputSystem.reset(new NoOpInputSystem);
        qCWarning(PLASMARC) << "Could not find an input system, plasma-remotecontrollers will not be able to send events";
    }
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
            action->setChecked(appInhibited(appId));
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

bool ControllerManager::appInhibited(const QString &appId) const
{
    const auto inhibitedApps = m_settings->applications();
    for (const auto &inhibitedApp : inhibitedApps) {
        const QRegularExpression rx(QRegularExpression::wildcardToRegularExpression(inhibitedApp));
        const auto match = rx.match(appId);
        if (match.hasMatch()) {
            return true;
        }
    }
    return false;
}

void ControllerManager::simulateUserActivity()
{
    const auto current = QDateTime::currentDateTimeUtc();
    if (!m_lastSimulated.isNull() && m_lastSimulated.secsTo(current) < 60) {
        return;
    }

    m_lastSimulated = current;
    OrgFreedesktopScreenSaverInterface iface(QStringLiteral("org.freedesktop.ScreenSaver"), QStringLiteral("/ScreenSaver"), QDBusConnection::sessionBus());
    iface.SimulateUserActivity();
}
