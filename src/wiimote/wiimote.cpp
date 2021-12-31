#include "wiimote.h"
#include <QLoggingCategory>
#include <QDebug>

#include <xwiimote.h>
#include <poll.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <unistd.h>

wiimote::wiimote(int fd, struct xwii_iface *iface)
{
    // TODO: Something here SIGSEGV's
    m_fd = fd;
    
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

void wiimote::emit_key(int key)
{
    emit_event(EV_KEY, key, 1);
    emit_event(EV_SYN, SYN_REPORT, 0);
    emit_event(EV_KEY, key, 0);
    emit_event(EV_SYN, SYN_REPORT, 0);
}

void wiimote::emit_event(int type, int code, int val)
{
    struct input_event ie;
    
    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;
    
    write(m_fd, &ie, sizeof(ie));
}

void wiimote::handle_keypress(struct xwii_event *event)
    {
    unsigned int code = event->v.key.code;
    bool pressed = event->v.key.state;
    
    if (pressed) {     
        switch (code) {
            case XWII_KEY_LEFT:
                emit_key(KEY_LEFT);
                break;
            case XWII_KEY_RIGHT:
                emit_key(KEY_RIGHT);
                break;
            case XWII_KEY_UP:
                emit_key(KEY_UP);
                break;
            case XWII_KEY_DOWN:
                emit_key(KEY_DOWN);
                break;
            case XWII_KEY_A:
                emit_key(KEY_ENTER);
                break;
            case XWII_KEY_B:
                emit_key(KEY_BACK);
                break;
            case XWII_KEY_HOME:
                emit_key(KEY_HOMEPAGE);
                break;
            case XWII_KEY_ONE:
                emit_key(KEY_1);
                break;
            case XWII_KEY_TWO:
                emit_key(KEY_2);
                break;
            case XWII_KEY_MINUS:
                emit_key(KEY_MINUS);
                break;
            case XWII_KEY_PLUS:
            case XWII_KEY_X:
            case XWII_KEY_Y:
            case XWII_KEY_TL:
            case XWII_KEY_TR:
            case XWII_KEY_ZL:
            case XWII_KEY_ZR:
                break;
        }
    }
}

void wiimote::handle_nunchuk(struct xwii_event *event)
{
    double val;
    
    if (event->type == XWII_EVENT_NUNCHUK_MOVE) {
        int time_since_previous_event =
        event->time.tv_sec - m_previous_nunchuk_axis;
        
        if (time_since_previous_event > 0) {
            // pow(val, 1/4) for smoother interpolation around the origin
            val = event->v.abs[0].x * 12;
            if (val > 1000) {
                emit_key(KEY_RIGHT);
                m_previous_nunchuk_axis = event->time.tv_sec;
            } else if (val < -1000) {
                emit_key(KEY_LEFT);
                m_previous_nunchuk_axis = event->time.tv_sec;
            }
            
            val = event->v.abs[0].y * 12;
            if (val > 1000) {
                emit_key(KEY_UP);
                m_previous_nunchuk_axis = event->time.tv_sec;
            } else if (val < -1000) {
                emit_key(KEY_DOWN);
                m_previous_nunchuk_axis = event->time.tv_sec;
            }
        }
    }
}

wiimote::~wiimote() = default;
