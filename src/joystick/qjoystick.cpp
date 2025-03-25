
#include <QDebug>
#include "qjoystick.h"

QJoyStick::QJoyStick(QObject* parent)
: QObject(parent) {}


Gamepad* QJoyStick::device() const
{
    return m_device;
}

void QJoyStick::setDevice(Gamepad* gamepad)
{
    if (m_device == gamepad) {
        return;
    }
    if (m_device) {
        disconnect(m_device);
    }
    m_device = gamepad;
    Q_EMIT deviceChanged();
    qDebug() << "Set gamepad!" << m_device;

    initButtons();
    initStick();
}

void QJoyStick::initButtons()
{
    m_buttons.clear();

    if (!m_device) {
        return;
    }

    const int numButtons = SDL_JoystickNumButtons(m_device->joystick());
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        const SDL_GameControllerButton button = static_cast<SDL_GameControllerButton>(i);
        if (SDL_GameControllerHasButton(m_device->gamecontroller(), button)) {
            m_buttons << button;
            if (m_buttons.count() == numButtons) {
                break;
            }
        }
    }

    connect(m_device, &Gamepad::buttonStateChanged, this, [this](SDL_GameControllerButton button) {
        const int row = m_buttons.indexOf(button);
        if (row >= 0) {
            const bool pressed = SDL_GameControllerGetButton(m_device->gamecontroller(), m_buttons.at(row));

            if (row == 0) {
                setButton0Pressed(pressed);
            } else if (row == 1) {
                setButton1Pressed(pressed);
            } else if (row == 2) {
                setButton2Pressed(pressed);
            } else if (row == 3) {
                setButton3Pressed(pressed);
            } else {
                qWarning() << "Unknown button" << row << "pressed:" << pressed;

            }
            //qDebug() << "Button" << row << "changed. pressed:" << pressed;
        }
    });
}

void QJoyStick::initStick()
{
    m_directionX = None;
    m_directionY = None;

    if (!m_device) {
        return;
    }

    connect(m_device, &Gamepad::axisStateChanged, this, [this](int axis) {
        auto val = SDL_JoystickGetAxis(m_device->joystick(), axis);
        qDebug() << "Axis" << axis << "changed:" << val;
        if (axis == 0) {
            if (val > 129) {
                setDirectionX(Right);
            } else if (val < 128) {
                setDirectionX(Left);
            } else {
                setDirectionX(None);
            }
        } else if (axis == 1)  {
            if (val > 129) {
                setDirectionY(Down);
            } else if (val < 128) {
                setDirectionY(Up);
            } else {
                setDirectionY(None);
            }
        } else {
            qWarning() << "Unknown axis" << axis;
        }
    });
}

QJoyStick::Direction QJoyStick::directionX() const {
    return m_directionX;
}

void QJoyStick::setDirectionX(Direction newDirection) {
    if (m_directionX != newDirection) {
        m_directionX = newDirection;
        Q_EMIT directionXChanged(m_directionX);
    }
}

QJoyStick::Direction QJoyStick::directionY() const {
    return m_directionY;
}

void QJoyStick::setDirectionY(Direction newDirection) {
    if (m_directionY != newDirection) {
        m_directionY = newDirection;
        Q_EMIT directionYChanged(m_directionY);
    }
}

bool QJoyStick::button0Pressed() const {
    return m_button0Pressed;
}

void QJoyStick::setButton0Pressed(bool pressed) {
    if (m_button0Pressed != pressed) {
        m_button0Pressed = pressed;
        qDebug() << "button 1" <<m_button0Pressed;
        Q_EMIT button0PressedChanged(m_button0Pressed);
    }
}


bool QJoyStick::button1Pressed() const {
    return m_button1Pressed;
}

void QJoyStick::setButton1Pressed(bool pressed) {
    if (m_button1Pressed != pressed) {
        m_button1Pressed = pressed;
        Q_EMIT button1PressedChanged(m_button1Pressed);
    }
}

bool QJoyStick::button2Pressed() const {
    return m_button2Pressed;
}

void QJoyStick::setButton2Pressed(bool pressed) {
    if (m_button2Pressed != pressed) {
        m_button2Pressed = pressed;
        Q_EMIT button2PressedChanged(m_button2Pressed);
    }
}

bool QJoyStick::button3Pressed() const {
    return m_button3Pressed;
}

void QJoyStick::setButton3Pressed(bool pressed) {
    if (m_button3Pressed != pressed) {
        m_button3Pressed = pressed;
        Q_EMIT button3PressedChanged(m_button3Pressed);
    }
}


#include "moc_qjoystick.cpp"
