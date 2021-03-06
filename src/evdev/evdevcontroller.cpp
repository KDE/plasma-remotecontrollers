/*
 *   SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "evdevcontroller.h"
#include "plasmarc-evdev-debug.h"
#include "../controllermanager.h"
#include <Solid/Block>
#include <Solid/GenericInterface>
#include <Solid/Device>
#include <Solid/DeviceNotifier>

#include <QDirIterator>
#include <QFile>
#include <QSocketNotifier>

#include <linux/input-event-codes.h>
#include <fcntl.h>

EvdevController::EvdevController()
{
    auto notifier = Solid::DeviceNotifier::instance();
    auto refresh = [this] (const QString &udi) {
        Solid::Device device(udi);
        if (device.is<Solid::Block>()) {
            qCInfo(PLASMARC_EVDEV) << "Trying device on evdev:" << device.product() << device.as<Solid::Block>()->device();
            addDevice(device);
        }
    };
    connect(notifier, &Solid::DeviceNotifier::deviceAdded, this, refresh);

    const auto devices = Solid::Device::listFromType(Solid::DeviceInterface::Block);
    for (const Solid::Device &device : devices) {
        addDevice(device);
    }
}

bool EvdevController::addDevice(const Solid::Device &device)
{
    auto inputDevice = device.as<Solid::Block>();
    auto generic = device.as<Solid::GenericInterface>();
    if (generic && !generic->property("ID_INPUT_JOYSTICK").toBool()) {
        return false;
    }

    qDebug() << "trying" << device.udi() << device.displayName() << inputDevice->device();
    struct libevdev *dev = NULL;
    int fd = open(QFile::encodeName(inputDevice->device()), O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        qCDebug(PLASMARC_EVDEV) << "Failed to open" << inputDevice->device() << strerror(errno);
        libevdev_free(dev);
        return false;
    }

    int rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        qCWarning(PLASMARC_EVDEV) << "Failed to init libevdev" << inputDevice->device() << strerror(-rc) << fd;
        close(fd);
        libevdev_free(dev);
        return false;
    }
    if (!libevdev_has_event_type(dev, EV_ABS) ||
        !libevdev_has_event_code(dev, EV_KEY, BTN_MODE))
    {
        qCDebug(PLASMARC_EVDEV) << "This device does not look like a remote controller:" << libevdev_get_name(dev);
        close(fd);
        libevdev_free(dev);
        return false;
    }

    qCInfo(PLASMARC_EVDEV) << "Added evdev device:" << device.displayName();
    auto evdevDevice = new EvdevDevice(device.udi(), dev);
    ControllerManager::instance().newDevice(evdevDevice);

    auto notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, evdevDevice, &EvdevDevice::readNow);

    return true;
}

EvdevController &EvdevController::instance()
{
    static EvdevController instance;
    return instance;
}

EvdevDevice::EvdevDevice(const QString& path, libevdev *device)
    : Device(DeviceGamepad, QString::fromUtf8(libevdev_get_name(device)), path)
    , m_device(device)
    , m_udi(path)
{
    auto notifier = Solid::DeviceNotifier::instance();
    connect(notifier, &Solid::DeviceNotifier::deviceRemoved, this, &EvdevDevice::deviceRemoved);
}

EvdevDevice::~EvdevDevice()
{
    libevdev_free(m_device);
}

void EvdevDevice::deviceRemoved(const QString& udi)
{
    if (m_udi == udi) {
        deleteLater();
    }
}

void EvdevDevice::setKey(int key, bool pressed)
{
    if (pressed == m_pressedKeys.contains(key)) {
        return;
    }

    if (pressed) {
        m_pressedKeys.insert(key);
    } else {
        m_pressedKeys.remove(key);
    }
    emit EvdevController::instance().m_dbusInterface.emitKeyPress(key);
    ControllerManager::instance().emitKey(key, pressed);
}

void EvdevDevice::readNow()
{
    const int fd = libevdev_get_fd(m_device);
    struct input_event ev;
    int ret = read(fd, &ev, sizeof(ev));
    if (ret == 0) {
        qDebug() << "nothing to read";
    } else if (ret < 0) {
        qWarning() << "Error while reading" << strerror(errno);
        if(errno == 19) {
            ControllerManager::instance().deviceRemoved(this);
            deleteLater();
        }
    } else {
        processEvent(ev);
    }

    uint bytes;
    ret = ::ioctl(fd, FIONREAD, &bytes);
    if (ret == 0 && bytes >= sizeof(ev))
        readNow();
}

void EvdevDevice::processEvent(struct input_event& ev)
{
    if (ev.type == EV_KEY) {
        static const QMap<int, QVector<int>> s_buttons = {
            { BTN_MODE, { KEY_LEFTMETA } },
            { BTN_START, { KEY_GAMES } },
            { BTN_SOUTH, { KEY_ENTER } },
            { BTN_EAST, { KEY_CANCEL } },
            { BTN_WEST, { KEY_MENU } },
            { BTN_TL, { KEY_LEFTSHIFT, KEY_TAB } },
            { BTN_TR, { KEY_TAB } },
            { BTN_TL2, { KEY_BACK } },
            { BTN_TR2, { KEY_FORWARD } },
            { BTN_DPAD_UP, { KEY_UP } },
            { BTN_DPAD_DOWN, { KEY_DOWN } },
            { BTN_DPAD_LEFT, { KEY_LEFT } },
            { BTN_DPAD_RIGHT, { KEY_RIGHT } }
        };
        const auto nativeKeyCodes = s_buttons.value(ev.code);

        if (!nativeKeyCodes.isEmpty()) {
            for (auto code : nativeKeyCodes) {
                emit EvdevController::instance().m_dbusInterface.emitKeyPress(ev.code);
                emit ControllerManager::instance().emitKey(code, ev.value);
            }
            return;
        }
        qCDebug(PLASMARC_EVDEV) << "Ignoring Key:" << libevdev_event_type_get_name(ev.type) << libevdev_event_code_get_name(ev.type, ev.code) << ev.value;
    } else if (ev.type == EV_ABS) {
        switch (ev.code) {
            case ABS_HAT0Y:
                emit setKey(KEY_DOWN, ev.value > 0);
                emit setKey(KEY_UP, ev.value < 0);
                return;
            case ABS_HAT0X:
                emit setKey(KEY_RIGHT, ev.value > 0);
                emit setKey(KEY_LEFT, ev.value < 0);
                return;
        }
    }
}
