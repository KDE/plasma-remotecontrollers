#include "wiimotecontroller.h"
#include "../controllermanager.h"
#include "../notificationsmanager.h"
#include "../constants.h"

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
        // First detect any new controllers
        mon = xwii_monitor_new(false, false);
        if (!mon) {
            qCritical() << "Cannot create monitor";
            return;
        }

        while ((ent = xwii_monitor_poll(mon))) {
            QString uniqueIdentifier = ent;
            if (ControllerManager::instance().isConnected(uniqueIdentifier))
                continue;

            ControllerManager::instance().newDevice(new Wiimote(ent));
            free(ent);
        }

        xwii_monitor_unref(mon);
        
        // Then check for a event on all connected controllers
        QVector<Device*> devices = ControllerManager::instance().getDevicesByType(DeviceWiimote);
        for (int i = 0; i < devices.size(); i++)
            devices.at(i)->watchEvents();

        usleep(LOOPTIME_WII); // Don't hug the CPU
    }
}

WiimoteController::~WiimoteController() = default;
