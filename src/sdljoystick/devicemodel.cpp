/*
    SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
    SPDX-FileCopyrightText: 2023 Jeremy Whiting <jpwhiting@kde.org>
    SPDX-FileCopyrightText: 2023 Niccolò Venerandi <niccolo@venerandi.com>
    SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "devicemodel.h"

#include <QTimer>

#include <SDL3/SDL.h>
#include <ios>
#include <iostream>
#include <vector>

#include "ksdljoystick.h"

#include "../controllermanager.h"

// 100 ms while we have devices
const int kShortPollTime = 100;
// 2 seconds while we don't have any devices
const int kLongPollTime = 2000;

static bool initialized = false;

DeviceModel::DeviceModel()
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DeviceModel::poll);
    m_timer->start(kLongPollTime);

    init();

    // Only poll once per 2 seconds until we have a device

    // Also call it once after we have initialized in case
    // there are already controllers.
    QTimer::singleShot(kShortPollTime, this, &DeviceModel::poll);
}

DeviceModel::~DeviceModel()
{
    if (initialized) {
        SDL_Quit();
        initialized = false;
    }
}

void DeviceModel::init()
{
    if (!initialized) {
        if (!SDL_Init(SDL_INIT_JOYSTICK)) {
            qWarning() << "Failed to initialize SDL joystick subsystem: " << SDL_GetError();
        } else {
            initialized = true;
        }
    }

    if (initialized) {
        int numJoysticks = 0;
        SDL_JoystickID* joystickIDs = SDL_GetJoysticks(&numJoysticks);
        if (!joystickIDs) {
            qWarning() << "SDL_GetJoysticks failed: " << SDL_GetError() << "\n";
            SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
            return;
        }

        qDebug() << "Number of joysticks connected: " << numJoysticks;

        for (int i = 0; i < numJoysticks; ++i) {
            SDL_JoystickID instID = joystickIDs[i];
            const char* name = SDL_GetJoystickNameForID(instID);
            const char* path = SDL_GetJoystickPathForID(instID);

            qDebug() << "Joystick #" << i
                    << " — Instance ID: " << instID
                    << "\n\tName: " << (name ? name : "Unknown")
                    << "\n\tPath: " << (path ? path : "Unknown");
            addDevice(instID);
        }
    }
}

KSDLJoyStick *DeviceModel::device(int index) const
{
    if (index < 0 || index >= m_devices.count()) {
        return nullptr;
    }

    const int sdlIndex = m_devices.keys().at(index);
    return m_devices.value(sdlIndex);
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_devices.count();
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index)) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        const int sdlIndex = m_devices.keys().at(index.row());
        return QString("%1 (%2)").arg(m_devices.value(sdlIndex)->name(), m_devices.value(sdlIndex)->path());
    }

    return {};
}

void DeviceModel::poll()
{
    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_JOYSTICK_ADDED:
            addDevice(event.cdevice.which);
            break;
        case SDL_EVENT_JOYSTICK_REMOVED:
            removeDevice(event.cdevice.which);
            break;
        case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
        case SDL_EVENT_JOYSTICK_BUTTON_UP:
            m_devices.value(event.jbutton.which)->onButtonEvent(event);
            break;
        case SDL_EVENT_JOYSTICK_AXIS_MOTION:
            m_devices.value(event.jaxis.which)->onAxisEvent(event);
            break;
        }
    }
}

void DeviceModel::addDevice(const int deviceIndex)
{
    const auto joystick = SDL_OpenJoystick(deviceIndex);

    if (!joystick) {
        qWarning() << "Failed to open joystick: " << SDL_GetError();
        return;
    }
    const auto id = SDL_GetJoystickID(joystick);

    if (m_devices.contains(id)) {
        qWarning() << "Got a duplicate add event, ignoring. Index: " << deviceIndex;
        return;
    }

    beginInsertRows(QModelIndex(), m_devices.count(), m_devices.count());
    m_devices.insert(id, new KSDLJoyStick(joystick, this));
    endInsertRows();

    // Now that we have a device poll every short poll time
    m_timer->setInterval(kShortPollTime);
    Q_EMIT devicesChanged();

    auto qjoystick = new PRJoyStick(m_devices[id]);
    ControllerManager::instance().newDevice(qjoystick);
}

void DeviceModel::removeDevice(const int deviceIndex)
{
    if (!m_devices.contains(deviceIndex)) {
        qWarning() << "Invalid device index from removal event, ignoring";
        return;
    }

    const int index = m_devices.keys().indexOf(deviceIndex);

    beginRemoveRows(QModelIndex(), index, index);
    m_devices.value(deviceIndex)->deleteLater();
    m_devices.remove(deviceIndex);
    endRemoveRows();

    if (m_devices.count() == 0) {
        // Go back to long timeout polling now that we don't have a device
        m_timer->setInterval(kLongPollTime);
    }
    Q_EMIT devicesChanged();
}

int DeviceModel::count() const
{
    return m_devices.size();
}

#include "moc_devicemodel.cpp"
