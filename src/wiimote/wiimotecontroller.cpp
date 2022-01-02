#include "wiimotecontroller.h"

#include <QDebug>

#include "../devicetypes.h"

WiimoteController::WiimoteController(Uinput* uinput) { m_uinput = uinput; }

void WiimoteController::run()
{
    // Keep trying to find a Wiiremote indefinitely
    struct xwii_iface* iface;
    while(true) {
        struct xwii_monitor* mon;
        char* ent;

        mon = xwii_monitor_new(false, false);
        if (!mon) {
            qCritical() << "Cannot create monitor";
            exit(1);
        }

        char* foundWiimote = NULL;
        while ((ent = xwii_monitor_poll(mon))) {
            foundWiimote = ent;
            free(ent);
        }
        
        if (!foundWiimote) {
            sleep(1);
            continue;
        }
        
        qInfo() << "  Found wiimote device: " << foundWiimote;

        xwii_monitor_unref(mon);

        int ret = 0;
        
        ret = xwii_iface_new(&iface, foundWiimote);

        if (ret) {
            qCritical() << "Cannot create xwii_iface " << ret;
            return;
        }
        
        ret = xwii_iface_open(iface, xwii_iface_available(iface) | XWII_IFACE_WRITABLE);

        if (ret) {
            qCritical() << "Error: Cannot open interface " << ret;
            return;
        }
        
        emit deviceConnected(DeviceWiimote);
        new Wiimote(m_uinput, iface);

        emit deviceDisconnected(DeviceWiimote);
        xwii_iface_unref(iface);
    }
}

WiimoteController::~WiimoteController() = default;
