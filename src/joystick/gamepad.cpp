/*
    SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
    SPDX-FileCopyrightText: 2023 Jeremy Whiting <jpwhiting@kde.org>
    SPDX-FileCopyrightText: 2023 Niccolò Venerandi <niccolo@venerandi.com>
    SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "gamepad.h"

#include <QTimer>
#include <SDL3/SDL.h>

Gamepad::Gamepad(SDL_Joystick *joystick, QObject *parent)
    : QObject(parent)
    , m_joystick(joystick)
{
    m_name = QString::fromLocal8Bit(SDL_GetJoystickName(m_joystick));
    m_path = QString::fromLocal8Bit(SDL_GetJoystickPath(m_joystick));
}

QString Gamepad::name() const
{
    return m_name;
}

QString Gamepad::path() const
{
    return m_path;
}

void Gamepad::onButtonEvent(const SDL_Event sdlEvent)
{
    Q_EMIT buttonStateChanged(sdlEvent.jbutton);
}

void Gamepad::onAxisEvent(const SDL_Event sdlEvent)
{
    const float value = sdlEvent.jaxis.value;
    if (sdlEvent.jaxis.axis == SDL_GAMEPAD_AXIS_LEFTX) {
        m_axis.setX(value);
        Q_EMIT axisValueChanged();
    } else if (sdlEvent.jaxis.axis == SDL_GAMEPAD_AXIS_LEFTY) {
        m_axis.setY(value);
        Q_EMIT axisValueChanged();
    }

    Q_EMIT axisStateChanged(sdlEvent.jaxis.axis);
}

SDL_Joystick *Gamepad::joystick() const
{
    return m_joystick;
}

QVector2D Gamepad::axisValue() const
{
    return QVector2D(m_axis);
}

#include "moc_gamepad.cpp"
