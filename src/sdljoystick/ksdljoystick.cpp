/*
    SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
    SPDX-FileCopyrightText: 2023 Jeremy Whiting <jpwhiting@kde.org>
    SPDX-FileCopyrightText: 2023 Niccolò Venerandi <niccolo@venerandi.com>
    SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ksdljoystick.h"

#include <QTimer>
#include <SDL3/SDL.h>

KSDLJoyStick::KSDLJoyStick(SDL_Joystick *joystick, QObject *parent)
    : QObject(parent)
    , m_joystick(joystick)
{
    m_name = QString::fromLocal8Bit(SDL_GetJoystickName(m_joystick));
    m_path = QString::fromLocal8Bit(SDL_GetJoystickPath(m_joystick));
}

QString KSDLJoyStick::name() const
{
    return m_name;
}

QString KSDLJoyStick::path() const
{
    return m_path;
}

void KSDLJoyStick::onButtonEvent(const SDL_Event sdlEvent)
{
    Q_EMIT buttonStateChanged(sdlEvent.jbutton);
}

void KSDLJoyStick::onAxisEvent(const SDL_Event sdlEvent)
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

SDL_Joystick *KSDLJoyStick::joystick() const
{
    return m_joystick;
}

QVector2D KSDLJoyStick::axisValue() const
{
    return QVector2D(m_axis);
}

#include "moc_ksdljoystick.cpp"
