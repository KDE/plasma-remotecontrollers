#include "wiimote.h"
#include <QLoggingCategory>
#include <QDebug>

#include <xwiimote.h>
#include <poll.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <unistd.h>

int Wiimote::m_fd;
QHash<int, int> Wiimote::m_keyCodeTranslation;

Wiimote::Wiimote(int fd, struct xwii_iface *iface)
{
    m_fd = fd;
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
    
    int ret = 0;
    
    ret = xwii_iface_watch(iface, true);
    
    if (ret) {
        qCritical() << "Error: Cannot initialize hotplug watch descriptor";
        exit(1);
    }

    int fds_num;
    struct pollfd fds[2];
    
    memset(fds, 0, sizeof(fds));
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[1].fd = xwii_iface_get_fd(iface);
    fds[1].events = POLLIN;
    fds_num = 2;
    
    struct xwii_event event;
    bool device_gone = false;
    while (!device_gone) {
        int ret = poll(fds, fds_num, -1);
        if (ret < 0) {
            qDebug() << "Error: Cannot poll fds: " << ret;
            break;
        }
        
        ret = xwii_iface_dispatch(iface, &event, sizeof(event));
        if (ret) {
            if (ret != -EAGAIN) {
                qCritical() << "Error: Read failed with err: " << ret;
                break;
            }
        }
        switch (event.type){
            case XWII_EVENT_GONE:
                fds[1].fd = -1;
                fds[1].events = 0;
                fds_num = 1;
                device_gone = true;
                break;
            case XWII_EVENT_KEY:
                handle_keypress(&event);
                break;
            case XWII_EVENT_ACCEL:
            case XWII_EVENT_IR:
            case XWII_EVENT_BALANCE_BOARD:
            case XWII_EVENT_MOTION_PLUS:
            case XWII_EVENT_NUNCHUK_KEY:
            case XWII_EVENT_NUNCHUK_MOVE:
                handle_nunchuk(&event);
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

void Wiimote::emit_key(int key, int pressed)
{
    emit_event(EV_KEY, key, pressed);
    emit_event(EV_SYN, SYN_REPORT, 0);
}

void Wiimote::emit_event(int type, int code, int val)
{
    struct input_event ie;
    
    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;
    
    write(m_fd, &ie, sizeof(ie));
}

void Wiimote::handle_keypress(struct xwii_event *event)
    {
    bool pressed = event->v.key.state;
    int nativeKeyCode = m_keyCodeTranslation.value(event->v.key.code, -1);
    
    if (nativeKeyCode < 0) {
        qDebug() << "DEBUG: Received a keypress we do not handle!";
        return;
    }

    emit_key(nativeKeyCode, pressed);
}

void Wiimote::handle_nunchuk(struct xwii_event *event)
{
    double val;

    if (event->type == XWII_EVENT_NUNCHUK_MOVE) {
        int time_since_previous_event =
        event->time.tv_sec - m_previous_nunchuk_axis;
        
        if (time_since_previous_event > 0) {
            // pow(val, 1/4) for smoother interpolation around the origin
            val = event->v.abs[0].x * 12;
            if (val > 1000) {
                emit_key(KEY_RIGHT, 1);
                emit_key(KEY_RIGHT, 0);
                m_previous_nunchuk_axis = event->time.tv_sec;
            } else if (val < -1000) {
                emit_key(KEY_LEFT, 1);
                emit_key(KEY_LEFT, 0);
                m_previous_nunchuk_axis = event->time.tv_sec;
            }
            
            val = event->v.abs[0].y * 12;
            if (val > 1000) {
                emit_key(KEY_UP, 1);
                emit_key(KEY_UP, 0);
                m_previous_nunchuk_axis = event->time.tv_sec;
            } else if (val < -1000) {
                emit_key(KEY_DOWN, 1);
                emit_key(KEY_DOWN, 0);
                m_previous_nunchuk_axis = event->time.tv_sec;
            }
        }
    }
}

Wiimote::~Wiimote() = default;
