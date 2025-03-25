#ifndef QJOYSTICK_H
#define QJOYSTICK_H

#include "gamepad.h"
#include <QObject>

class QJoyStick : public QObject {
    Q_OBJECT
    Q_ENUMS(Direction)

    Q_PROPERTY(Gamepad *device MEMBER m_device READ device WRITE setDevice NOTIFY deviceChanged REQUIRED)


    Q_PROPERTY(Direction directionX READ directionX WRITE setDirectionX NOTIFY directionXChanged)
    Q_PROPERTY(Direction directionY READ directionY WRITE setDirectionY NOTIFY directionYChanged)

    Q_PROPERTY(bool button0Pressed READ button0Pressed WRITE setButton0Pressed NOTIFY button0PressedChanged)
    Q_PROPERTY(bool button1Pressed READ button1Pressed WRITE setButton1Pressed NOTIFY button1PressedChanged)
    Q_PROPERTY(bool button2Pressed READ button2Pressed WRITE setButton2Pressed NOTIFY button2PressedChanged)
    Q_PROPERTY(bool button3Pressed READ button3Pressed WRITE setButton3Pressed NOTIFY button3PressedChanged)
    //QML_VALUE_TYPE(qjoystick)

public:
    enum Direction {
        None,
        Up,
        Down,
        Left,
        Right
    };
    Q_ENUM(Direction)

    explicit QJoyStick(QObject* parent = nullptr);

    Gamepad* device() const;
    void setDevice(Gamepad* gamepad);

    Direction directionX() const;
    void setDirectionX(Direction newDirection);

    Direction directionY() const;
    void setDirectionY(Direction newDirection);

    bool button0Pressed() const;
    void setButton0Pressed(bool pressed);

    bool button1Pressed() const;
    void setButton1Pressed(bool pressed);

    bool button2Pressed() const;
    void setButton2Pressed(bool pressed);

    bool button3Pressed() const;
    void setButton3Pressed(bool pressed);


Q_SIGNALS:
    void deviceChanged();

    void directionXChanged(Direction newDirection);
    void directionYChanged(Direction newDirection);

    void button0PressedChanged(bool pressed);
    void button1PressedChanged(bool pressed);
    void button2PressedChanged(bool pressed);
    void button3PressedChanged(bool pressed);

private:
    void initButtons();
    void initStick();

    Direction m_directionX { None };
    Direction m_directionY { None };

    qint16 m_axisX { 0 };
    qint16 m_axisY { 0 };

    bool m_button1Pressed { false };
    bool m_button2Pressed { false };
    bool m_button3Pressed { false };
    bool m_button0Pressed { false };

    Gamepad* m_device { nullptr };
    QList<SDL_GameControllerButton> m_buttons;
};

#endif // QJOYSTICK_H
