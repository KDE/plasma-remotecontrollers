/*
    SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
    SPDX-FileCopyrightText: 2023 Jeremy Whiting <jpwhiting@kde.org>
    SPDX-FileCopyrightText: 2023 Niccolò Venerandi <niccolo@venerandi.com>
    SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QPointF>
#include <QString>
#include <QVector2D>

#include <SDL3/SDL.h>

class KSDLJoyStick : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVector2D axisValue READ axisValue NOTIFY axisValueChanged)

public:
    KSDLJoyStick(SDL_Joystick *joystick, QObject *parent = nullptr);

    QString name() const;
    QString path() const;

    SDL_Joystick *joystick() const;

    QVector2D axisValue() const;

Q_SIGNALS:
    void buttonStateChanged(SDL_JoyButtonEvent jbutton);
    void axisStateChanged(int index);
    void axisValueChanged();

private:
    friend class DeviceModel;

    void onButtonEvent(SDL_Event sdlEvent);
    void onAxisEvent(SDL_Event sdlEvent);

    SDL_Joystick *m_joystick = nullptr;

    QString m_name;
    QString m_path;
    QPointF m_axis = QPointF(0.0, 0.0);
};
