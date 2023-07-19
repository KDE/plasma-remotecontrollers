/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "wiimotecontroller.h"
#include "../controllermanager.h"
#include "../notificationsmanager.h"

#include <QDebug>

WiimoteController::WiimoteController()
{
    m_detectionThread = new QThread();
    m_eventsThread = new QThread();
    WiimoteEventWatcher *eventWatcher = new WiimoteEventWatcher();
    
    QObject::connect(m_detectionThread, &QThread::started, this, &WiimoteController::deviceDetection);
    QObject::connect(m_eventsThread, &QThread::started, eventWatcher, &WiimoteEventWatcher::watchEvents);
    
    this->moveToThread(m_detectionThread);
    eventWatcher->moveToThread(m_eventsThread);

    m_detectionThread->start();
    m_eventsThread->start();
}

void WiimoteController::deviceDetection()
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

            struct xwii_iface *iface;
            int ret = xwii_iface_new(&iface, ent);

            if (ret) {
                qCritical() << "wiimote: ERROR: Failed to create a new Wiimote device, error:" << ret;
                continue;
            }

            Wiimote *wiimote = new Wiimote(iface, ent);
            if (wiimote->getDevType() == WIIMOTE_DEVTYPE_UNKNOWN) {
                qDebug() << "wiimote: DEBUG: Unable to determine device type, skipping";
                continue;
            }

            ControllerManager::instance().newDevice(wiimote);
            free(ent);
        }

        xwii_monitor_unref(mon);

        usleep(LOOPTIME); // Don't hug the CPU
    }
}

WiimoteController::~WiimoteController()
{
    m_detectionThread->quit();
    m_eventsThread->quit();
    m_detectionThread->wait();
    m_eventsThread->wait();
}

void WiimoteEventWatcher::watchEvents()
{
    while (true) {
        QVector<Device*> devices = ControllerManager::instance().getDevicesByType(DeviceWiimote);
        for (int i = 0; i < devices.size(); i++)
            devices.at(i)->watchEvents();
        
        usleep(LOOPTIME_WII); // Don't hug the CPU
    }
}

#include "moc_wiimotecontroller.cpp"
