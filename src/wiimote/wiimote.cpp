#include "wiimote.h"
#include "../controllermanager.h"

#include <QLoggingCategory>
#include <QDebug>

#include <xwiimote.h>
#include <linux/input-event-codes.h>
#include <unistd.h>

QHash<int, int> Wiimote::m_keyCodeTranslation;

Wiimote::Wiimote(char *sysPath)
{
    m_uniqueIdentifier = sysPath;
    m_name = "Wiimote";
    m_deviceType = DeviceWiimote;

    QObject::connect(this, &Wiimote::keyPress,
                     &ControllerManager::instance(), &ControllerManager::emitKey);

    int ret = xwii_iface_new(&m_iface, sysPath);

    if (ret) {
        qCritical() << "Error: Cannot create xwii_iface " << ret;
        return;
    }

    ret = xwii_iface_open(m_iface, xwii_iface_available(m_iface) | XWII_IFACE_WRITABLE);

    if (ret) {
        qCritical() << "Error: Cannot open interface " << ret;
        return;
    }

    m_keyCodeTranslation = {
        { XWII_KEY_A, KEY_SELECT},
        { XWII_KEY_B, KEY_BACK},
        { XWII_KEY_UP, KEY_UP},
        { XWII_KEY_DOWN, KEY_DOWN},
        { XWII_KEY_LEFT, KEY_LEFT},
        { XWII_KEY_RIGHT, KEY_RIGHT},
        { XWII_KEY_ONE, KEY_1},
        { XWII_KEY_TWO, KEY_2},
        { XWII_KEY_PLUS, KEY_VOLUMEUP},
        { XWII_KEY_MINUS, KEY_VOLUMEDOWN},
        { XWII_KEY_HOME, KEY_HOME},
    };
    
    ret = xwii_iface_watch(m_iface, true);
    
    if (ret) {
        qCritical() << "Error: Cannot initialize hotplug watch descriptor";
        return;
    }

    memset(m_fds, 0, sizeof(m_fds));
    m_fds[0].fd = 0;
    m_fds[0].events = POLLIN;
    m_fds[1].fd = xwii_iface_get_fd(m_iface);
    m_fds[1].events = POLLIN;
    m_fdsNum = 2;
    
    // Let the user know the device is being used by rumbling
    xwii_iface_rumble(m_iface, true);
    sleep(1);
    xwii_iface_rumble(m_iface, false);
}

void Wiimote::run()
{
    struct xwii_event event;
    int ret;
    bool deviceGone = false;
    while (!deviceGone) {
        ret = poll(m_fds, m_fdsNum, -1);
        if (ret < 0) {
            qDebug() << "Error: Cannot poll fds: " << ret;
            break;
        }
        
        ret = xwii_iface_dispatch(m_iface, &event, sizeof(event));
        if (ret && ret != -EAGAIN) {
            qCritical() << "Error: Read failed with err: " << ret;
            break;
        }

        switch (event.type) {
            case XWII_EVENT_GONE:
                m_fds[1].fd = -1;
                m_fds[1].events = 0;
                m_fdsNum = 1;
                deviceGone = true;
                ControllerManager::instance().removeDevice(m_index);
                return;
            case XWII_EVENT_KEY:
                handleKeypress(&event);
                break;
            case XWII_EVENT_ACCEL:
            case XWII_EVENT_IR:
            case XWII_EVENT_BALANCE_BOARD:
            case XWII_EVENT_MOTION_PLUS:
            case XWII_EVENT_NUNCHUK_KEY:
            case XWII_EVENT_NUNCHUK_MOVE:
                handleNunchuk(&event);
                break;
            case XWII_EVENT_CLASSIC_CONTROLLER_KEY:
            case XWII_EVENT_CLASSIC_CONTROLLER_MOVE:
            case XWII_EVENT_PRO_CONTROLLER_KEY:
            case XWII_EVENT_PRO_CONTROLLER_MOVE:
            case XWII_EVENT_WATCH:
            case XWII_EVENT_NUM:
            case XWII_EVENT_GUITAR_KEY:
            case XWII_EVENT_GUITAR_MOVE:
            case XWII_EVENT_DRUMS_KEY:
            case XWII_EVENT_DRUMS_MOVE:
                break;
        }
    }
}

void Wiimote::handleKeypress(struct xwii_event *event)
    {
    bool pressed = event->v.key.state;
    int nativeKeyCode = m_keyCodeTranslation.value(event->v.key.code, -1);
    
    if (nativeKeyCode < 0) {
        qDebug() << "DEBUG: Received a keypress we do not handle!";
        return;
    }

    emit keyPress(nativeKeyCode, pressed);
}

void Wiimote::handleNunchuk(struct xwii_event *event)
{
    double val;

    if (event->type == XWII_EVENT_NUNCHUK_MOVE) {
        int time_since_previous_event =
        event->time.tv_sec - m_previousNunchukAxisTime;
        
        if (time_since_previous_event > 0) {
            // pow(val, 1/4) for smoother interpolation around the origin
            val = event->v.abs[0].x * 12;
            if (val > 1000) {
                emit keyPress(KEY_RIGHT, true);
                emit keyPress(KEY_RIGHT, false);
                m_previousNunchukAxisTime = event->time.tv_sec;
            } else if (val < -1000) {
                emit keyPress(KEY_LEFT, true);
                emit keyPress(KEY_LEFT, false);
                m_previousNunchukAxisTime = event->time.tv_sec;
            }
            
            val = event->v.abs[0].y * 12;
            if (val > 1000) {
                emit keyPress(KEY_UP, true);
                emit keyPress(KEY_UP, false);
                m_previousNunchukAxisTime = event->time.tv_sec;
            } else if (val < -1000) {
                emit keyPress(KEY_DOWN, true);
                emit keyPress(KEY_DOWN, false);
                m_previousNunchukAxisTime = event->time.tv_sec;
            }
        }
    }
}

Wiimote::~Wiimote()
{
    // TODO: this seems to never be called
    xwii_iface_unref(m_iface);
}
