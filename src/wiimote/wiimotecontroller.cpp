#include "wiimotecontroller.h"

#include <QDebug>

WiimoteController::WiimoteController(int fd) { m_fd = fd; }

void WiimoteController::run()
{
    // Keep trying to find a Wiiremote indefinitely
    static struct xwii_iface *iface;
    while(true) {
        struct xwii_monitor *mon;
        char *ent;

        mon = xwii_monitor_new(false, false);
        if (!mon) {
            qCritical() << "Cannot create monitor";
            exit(1);
        }

        char* found_wiimote = NULL;
        while ((ent = xwii_monitor_poll(mon))) {
            found_wiimote = ent;
            free(ent);
        }
        
        if (!found_wiimote) {
            sleep(1);
            continue;
        }
        
        qInfo() << "  Found wiimote device: " << found_wiimote;

        xwii_monitor_unref(mon);
        
        int ret = 0;
        
        ret = xwii_iface_new(&iface, found_wiimote);
        
        if (ret) {
            qCritical() << "Cannot create xwii_iface " << found_wiimote << " " << ret;
            exit(1);
        }
        
        ret = xwii_iface_open(iface, xwii_iface_available(iface) | XWII_IFACE_WRITABLE);
        
        if (ret) {
            qCritical() << "Error: Cannot open interface " << ret;
            exit(1);
        }
        
        new wiimote(m_fd, iface);
        xwii_iface_unref(iface);
    }
}

WiimoteController::~WiimoteController() = default;
