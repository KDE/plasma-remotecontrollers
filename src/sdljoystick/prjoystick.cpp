/*
    SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QDebug>
#include "prjoystick.h"

#include "../controllermanager.h"

#include <linux/input-event-codes.h>

// The middle position of the joystick, calibration thing basically
const int kJoyStickNone = 1024;


PRJoyStick::PRJoyStick(KSDLJoyStick* gamepad)
{
    qDebug() << "Initializing joystick with None position between" << -kJoyStickNone << "and" << kJoyStickNone;
    setDevice(gamepad);
    m_uniqueIdentifier = gamepad->path();
    m_name = gamepad->name();
    m_deviceType = DeviceJoystick;

    QObject::connect(this, &PRJoyStick::keyPress,
                     &ControllerManager::instance(), &ControllerManager::emitKey);
}

KSDLJoyStick* PRJoyStick::device() const
{
    return m_device;
}

void PRJoyStick::setDevice(KSDLJoyStick* gamepad)
{
    if (m_device == gamepad) {
        return;
    }
    if (m_device) {
        disconnect(m_device);
    }
    m_device = gamepad;
    Q_EMIT deviceChanged();

    initButtons();
    initStick();
}

void PRJoyStick::initButtons()
{
    if (!m_device) {
        return;
    }

    connect(m_device, &KSDLJoyStick::buttonStateChanged, this, [this](SDL_JoyButtonEvent jbutton) {
        const int button = jbutton.button;
        if (button >= 0) {
            const bool pressed = jbutton.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN;

            if (button == 0) {
                setButton0Pressed(pressed);
            } else if (button == 1) {
                setButton1Pressed(pressed);
            } else if (button == 2) {
                setButton2Pressed(pressed);
            } else if (button == 3) {
                setButton3Pressed(pressed);
            } else {
                qWarning() << "Unknown button" << button << "pressed:" << pressed;
            }
        }
    });
}

void PRJoyStick::initStick()
{
    m_directionX = None;
    m_directionY = None;

    if (!m_device) {
        return;
    }

    connect(m_device, &KSDLJoyStick::axisStateChanged, this, [this](int axis) {
        auto val = SDL_GetJoystickAxis(m_device->joystick(), axis);
        if (axis == SDL_GAMEPAD_AXIS_LEFTX) {
            if (val > kJoyStickNone) {
                setDirectionX(Right);
            } else if (val < -kJoyStickNone) {
                setDirectionX(Left);
            } else {
                setDirectionX(None);
            }
        } else if (axis == SDL_GAMEPAD_AXIS_LEFTY)  {
            if (val > kJoyStickNone) {
                setDirectionY(Down);
            } else if (val < -kJoyStickNone) {
                setDirectionY(Up);
            } else {
                setDirectionY(None);
            }
        } else {
            qWarning() << "Unknown axis" << axis;
        }
    });
}

PRJoyStick::Direction PRJoyStick::directionX() const {
    return m_directionX;
}

void PRJoyStick::setDirectionX(Direction newDirection) {
    if (m_directionX != newDirection) {

        // X axis, press
        if (m_directionX == None && newDirection == Left) {
            Q_EMIT keyPress(KEY_LEFT, true);
        }
        if (m_directionX == None && newDirection == Right) {
            Q_EMIT keyPress(KEY_RIGHT, true);
        }

        // X axis, release
        if (m_directionX == Left && newDirection == None) {
            Q_EMIT keyPress(KEY_LEFT, false);
        }
        if (m_directionX == Right && newDirection == None) {
            Q_EMIT keyPress(KEY_RIGHT, false);
        }

        m_directionX = newDirection;
        Q_EMIT directionXChanged(m_directionX);
    }
}

PRJoyStick::Direction PRJoyStick::directionY() const {
    return m_directionY;
}

void PRJoyStick::setDirectionY(Direction newDirection) {
    if (m_directionY != newDirection) {

        // X axis, press
        if (m_directionY == None && newDirection == Up) {
            Q_EMIT keyPress(KEY_UP, true);
        }
        if (m_directionY == None && newDirection == Down) {
            Q_EMIT keyPress(KEY_DOWN, true);
        }

        // X axis, release
        if (m_directionY == Up && newDirection == None) {
            Q_EMIT keyPress(KEY_UP, false);
        }
        if (m_directionY == Down && newDirection == None) {
            Q_EMIT keyPress(KEY_DOWN, false);
        }
        m_directionY = newDirection;
        Q_EMIT directionYChanged(m_directionY);
    }
}

bool PRJoyStick::button0Pressed() const {
    return m_button0Pressed;
}

void PRJoyStick::setButton0Pressed(bool pressed) {
    if (m_button0Pressed != pressed) {
        m_button0Pressed = pressed;
        Q_EMIT keyPress(KEY_ENTER, pressed);
        Q_EMIT button0PressedChanged(m_button0Pressed);
    }
}

bool PRJoyStick::button1Pressed() const {
    return m_button1Pressed;
}

void PRJoyStick::setButton1Pressed(bool pressed) {
    if (m_button1Pressed != pressed) {
        m_button1Pressed = pressed;
        Q_EMIT keyPress(KEY_LEFTCTRL, pressed);
        Q_EMIT button1PressedChanged(m_button1Pressed);
    }
}

bool PRJoyStick::button2Pressed() const {
    return m_button2Pressed;
}

void PRJoyStick::setButton2Pressed(bool pressed) {
    if (m_button2Pressed != pressed) {
        m_button2Pressed = pressed;
        Q_EMIT keyPress(KEY_ENTER, pressed);
        Q_EMIT button2PressedChanged(m_button2Pressed);
    }
}

bool PRJoyStick::button3Pressed() const {
    return m_button3Pressed;
}

void PRJoyStick::setButton3Pressed(bool pressed) {
    if (m_button3Pressed != pressed) {
        m_button3Pressed = pressed;
        Q_EMIT keyPress(KEY_LEFTSHIFT, pressed);
        Q_EMIT button3PressedChanged(m_button3Pressed);
    }
}


#include "moc_prjoystick.cpp"
