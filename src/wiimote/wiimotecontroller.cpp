#include "wiimotecontroller.h"
#include "../controllermanager.h"
#include "../notificationsmanager.h"

#include <QDebug>

WiimoteController::WiimoteController()
{
}

void WiimoteController::run()
{
    // Keep trying to find a Wiiremote indefinitely
    struct xwii_monitor *mon;
    char *ent;

    while(true) {
        mon = xwii_monitor_new(false, false);
        if (!mon) {
            qCritical() << "Cannot create monitor";
            return;
        }

        while ((ent = xwii_monitor_poll(mon))) {
            if (ControllerManager::instance().isConnected(ent))
                continue;

            ControllerManager::instance().newDevice(new Wiimote(ent));
            free(ent);
        }

        xwii_monitor_unref(mon);
        
        // Let's not hug the CPU
        sleep(1);
    }
}

WiimoteController::~WiimoteController() = default;
