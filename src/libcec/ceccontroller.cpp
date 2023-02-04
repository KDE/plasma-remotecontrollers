/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *   SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "ceccontroller.h"
#include "../controllermanager.h"
#include "../device.h"

#include <QDebug>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>

#include <Solid/DeviceNotifier>

#include <iostream> // Workaround for libcec bug
#include <libcec/cec.h>
#include <libcec/cecloader.h>
#include <libcec/cectypes.h>
#include <linux/input-event-codes.h>
#include <unistd.h>

using namespace CEC;

QHash<int, int> CECController::m_keyCodeTranslation;
bool CECController::m_catchNextInput = false;
bool CECController::m_nativeNavMode = true;
int CECController::m_caughtInput = -1;
int CECController::m_hitcommand;


QDBusArgument& operator<<(QDBusArgument& arg, const cec_logical_address& address) {
    arg.beginStructure();
    arg << static_cast<uchar>(address);
    arg.endStructure();
    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, cec_logical_address& address) {
    uchar value;
    arg.beginStructure();
    arg >> value;
    arg.endStructure();
    address = static_cast<cec_logical_address>(value);
    return arg;
}

void CECController::handleCecKeypress(void* param, const cec_keypress* key)
{
    Q_UNUSED(param);
    // only handle complete event when we get the keycode, opcode for press event is always sent before keycode
    handleCompleteEvent(key->keycode, key->duration, m_hitcommand);
}

void CECController::handleCommandReceived(void* param, const cec_command* command)
{
    Q_UNUSED(param);
    m_hitcommand = command->opcode;
}

void CECController::handleCompleteEvent(const int keycode, const int keyduration, const int opcode)
{
    Q_UNUSED(keyduration);

    if (m_catchNextInput) {
        m_caughtInput = keycode;

        // check if m_caughtInput has changed
        if (m_caughtInput != -1) {
            m_catchNextInput = false;
        }
    } else if(m_nativeNavMode) {
        int nativeKeyCode = m_keyCodeTranslation.value(keycode, -1);

        if (nativeKeyCode < 0) {
            qDebug() << "DEBUG: Received a keypress we do not handle!";
            return;
        }

        if(opcode == CEC_OPCODE_USER_CONTROL_PRESSED) {
            // send key press and key release events before cec key release event
            // otherwise the key release event will take 500 ms to be sent and cause the key to be stuck
            // in the pressed state
            ControllerManager::instance().emitKey(nativeKeyCode, 1);
            ControllerManager::instance().emitKey(nativeKeyCode, 0);
        } else if (opcode == CEC_OPCODE_USER_CONTROL_RELEASE) {

            // not sure if this is actually needed, cec will send key pressed events even when it hasn't produced a key release event
            ControllerManager::instance().emitKey(nativeKeyCode, 0);
        }
    } else {
        return;
    }
}

CECController::CECController()
{
    qDBusRegisterMetaType<cec_logical_address>();
    QDBusConnection::sessionBus().registerService("org.kde.plasma.remotecontrollers");
    QDBusConnection::sessionBus().registerObject("/CEC", this, QDBusConnection::ExportScriptableSlots);

    QObject::connect(this, &CECController::keyPress,
                     &ControllerManager::instance(), &ControllerManager::emitKey);

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup generalGroup = config->group("General");

    auto configPair = [&generalGroup](const char* name, int cecKey, int evKey) {
        return std::make_pair<int, int>(generalGroup.readEntry(QString("Button") + name, cecKey), generalGroup.readEntry(QString("Key") + name, evKey));
    };

    m_keyCodeTranslation = {
        configPair("Play", CEC_USER_CONTROL_CODE_PLAY, KEY_PLAY),
        configPair("Stop", CEC_USER_CONTROL_CODE_STOP, KEY_STOP),
        configPair("Pause", CEC_USER_CONTROL_CODE_PAUSE, KEY_PAUSE),
        configPair("Rewind", CEC_USER_CONTROL_CODE_REWIND, KEY_REWIND),
        configPair("Fastforward", CEC_USER_CONTROL_CODE_FAST_FORWARD, KEY_FASTFORWARD),
        configPair("Enter", CEC_USER_CONTROL_CODE_SELECT, KEY_ENTER),
        configPair("Up", CEC_USER_CONTROL_CODE_UP, KEY_UP),
        configPair("Down", CEC_USER_CONTROL_CODE_DOWN, KEY_DOWN),
        configPair("Left", CEC_USER_CONTROL_CODE_LEFT, KEY_LEFT),
        configPair("Right", CEC_USER_CONTROL_CODE_RIGHT, KEY_RIGHT),
        configPair("Number0", CEC_USER_CONTROL_CODE_NUMBER0, KEY_0),
        configPair("Number1", CEC_USER_CONTROL_CODE_NUMBER1, KEY_1),
        configPair("Number2", CEC_USER_CONTROL_CODE_NUMBER2, KEY_2),
        configPair("Number3", CEC_USER_CONTROL_CODE_NUMBER3, KEY_3),
        configPair("Number4", CEC_USER_CONTROL_CODE_NUMBER4, KEY_4),
        configPair("Number5", CEC_USER_CONTROL_CODE_NUMBER5, KEY_5),
        configPair("Number6", CEC_USER_CONTROL_CODE_NUMBER6, KEY_6),
        configPair("Number7", CEC_USER_CONTROL_CODE_NUMBER7, KEY_7),
        configPair("Number8", CEC_USER_CONTROL_CODE_NUMBER8, KEY_8),
        configPair("Number9", CEC_USER_CONTROL_CODE_NUMBER9, KEY_9),
        configPair("Blue", CEC_USER_CONTROL_CODE_F1_BLUE, KEY_BLUE),
        configPair("Red", CEC_USER_CONTROL_CODE_F2_RED, KEY_RED),
        configPair("Green", CEC_USER_CONTROL_CODE_F3_GREEN, KEY_GREEN),
        configPair("Yellow", CEC_USER_CONTROL_CODE_F4_YELLOW, KEY_YELLOW),
        configPair("ChannelUp", CEC_USER_CONTROL_CODE_CHANNEL_UP, KEY_CHANNELUP),
        configPair("ChannelDown", CEC_USER_CONTROL_CODE_CHANNEL_DOWN, KEY_CHANNELDOWN),
        configPair("Exit", CEC_USER_CONTROL_CODE_EXIT, KEY_EXIT),
        configPair("Back", CEC_USER_CONTROL_CODE_AN_RETURN, KEY_BACK),
        configPair("Home", CEC_USER_CONTROL_CODE_ROOT_MENU, KEY_HOMEPAGE),
        configPair("Subtitle", CEC_USER_CONTROL_CODE_SUB_PICTURE, KEY_SUBTITLE),
        configPair("Info", CEC_USER_CONTROL_CODE_DISPLAY_INFORMATION, KEY_INFO),
    };

    m_cecCallbacks.Clear();
    m_cecCallbacks.keyPress = &CECController::handleCecKeypress;
    m_cecCallbacks.commandReceived = &CECController::handleCommandReceived;


    libcec_configuration cecConfig;
    cecConfig.Clear();
    cecConfig.bActivateSource = 0;
    snprintf(cecConfig.strDeviceName, LIBCEC_OSD_NAME_SIZE, "%s", qPrintable(generalGroup.readEntry("OSDName", i18n("KDE Plasma"))));
    cecConfig.clientVersion = LIBCEC_VERSION_CURRENT;
    cecConfig.deviceTypes.Add(CEC_DEVICE_TYPE_RECORDING_DEVICE);
    cecConfig.callbacks = &m_cecCallbacks;

    m_cecAdapter = LibCecInitialise(&cecConfig);

    if (!m_cecAdapter) {
        qCritical() << "Could not create CEC adaptor with current config";
        exit(1);
    }

    // Init video on targets that need this
    m_cecAdapter->InitVideoStandalone();

    auto notifier = Solid::DeviceNotifier::instance();
    connect(notifier, &Solid::DeviceNotifier::deviceAdded, this, &CECController::discoverDevices);
    discoverDevices();
}

void CECController::discoverDevices() {
    cec_adapter_descriptor devices[10];
    int8_t deviceCount = m_cecAdapter->DetectAdapters(devices, 10, nullptr, true);

    if (deviceCount <= 0) {
        qWarning() << "No CEC devices found";
        return;
    }

    for (int8_t i = 0; i < deviceCount; i++) {
        QString uniqueIdentifier = devices[i].strComName;
        if (ControllerManager::instance().isConnected(uniqueIdentifier))
            continue;

        if (!m_cecAdapter->Open(devices[i].strComName)) {
            qWarning() << "Could not open CEC device " << devices[i].strComPath << " " << devices[i].strComName;
            return;
        }

        // TODO: detect and handle disconnects
        Device* device = new Device(DeviceCEC, "CEC Controller", devices[i].strComName);
        ControllerManager::instance().newDevice(device);
    }
}

CECController::~CECController() = default;

int CECController::sendNextKey()
{
    m_catchNextInput = true;
    m_nativeNavMode = false;

    // don't depend on caught input being set
    // enter key sends keycode 0 which is == false, keeps the loop running indefinitely

    while (m_catchNextInput) {
        sleep(1);
    }

    m_nativeNavMode = true;

    return m_caughtInput;
}

bool CECController::hdmiCecSupported()
{
    cec_logical_addresses iFaceAddresses = m_cecAdapter->GetLogicalAddresses();
    cec_power_status currentStatus;
    for (uint8_t i = 0; i < CECDEVICE_BROADCAST; i++) {
        if (static_cast<cec_logical_address>(iFaceAddresses[i]) != CECDEVICE_UNKNOWN) {
            currentStatus = m_cecAdapter->GetDevicePowerStatus(static_cast<cec_logical_address>(iFaceAddresses[i]));
            if (currentStatus == CEC_POWER_STATUS_ON) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}
