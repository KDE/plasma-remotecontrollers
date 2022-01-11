/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "wiimotecontroller.h"
#include "../controllermanager.h"
#include "../notificationsmanager.h"

#include <QDebug>

#define LOOPTIME_WII 7.5 * 1000

WiimoteController::WiimoteController()
{
    m_detectionThread = new QThread();
    m_eventsThread = new QThread();
    WiimoteEventWatcher *eventWatcher = new WiimoteEventWatcher();
    
    QObject::connect(m_detectionThread, &QThread::started, this, &WiimoteController::deviceDetection);
    QObject::connect(m_eventsThread, &QThread::started, eventWatcher, &WiimoteEventWatcher::watchEvents);
    
    this->moveToThread(m_detectionThread);
    eventWatcher->moveToThread(m_eventsThread);
    
    m_wiimotesptr = wiiuse_init(MAX_WIIMOTES);
    
    // Silence the library, it should never print anything
    // It'll still print a copyright banner but we can live with that
    wiiuse_set_output(LOGLEVEL_INFO, NULL);

    m_detectionThread->start();
    m_eventsThread->start();
}

void WiimoteController::deviceDetection()
{
    while (true) {
        int found = wiiuse_find(m_wiimotesptr, MAX_WIIMOTES, SEARCH_TIMEOUT);
        
        if (!found)
            continue;
        
        int connected = wiiuse_connect(m_wiimotesptr, MAX_WIIMOTES);

        if (connected) {
            for (int i = 0; i < connected; i++) {
                if (ControllerManager::instance().isConnected(m_wiimotesptr[i]->bdaddr_str))
                    continue;
                
                Wiimote *wiimote = new Wiimote(m_wiimotesptr[i]);
                ControllerManager::instance().newDevice(wiimote);
            }
        }
    }
}

WiimoteController::~WiimoteController()
{
    m_detectionThread->quit();
    m_eventsThread->quit();
    m_detectionThread->wait();
    m_eventsThread->wait();

    wiiuse_cleanup(m_wiimotesptr, MAX_WIIMOTES);
}

void WiimoteEventWatcher::watchEvents()
{
    while (true) {
        QVector<Device*> devices = ControllerManager::instance().getDevicesByType(DeviceWiimote);
        for (int i = 0; i < devices.size(); i++)
            devices.at(i)->watchEvents();
        
        usleep(LOOPTIME_WII);
    }
}

