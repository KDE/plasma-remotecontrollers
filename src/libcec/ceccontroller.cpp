#include "ceccontroller.h"

#include <QDebug>

#include <iostream> // Workaround for libcec bug
#include <libcec/cec.h>
#include <libcec/cecloader.h>
#include <libcec/cectypes.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <unistd.h>

#include <KSharedConfig>
#include <KConfigGroup>

using namespace CEC;

QHash<int, int> CECController::m_keyCodeTranslation;

void CECController::handleCecKeypress(void* param, const cec_keypress* key)
{
    Q_UNUSED(param)

    int nativeKeyCode = m_keyCodeTranslation.value(key->keycode, -1);
    
    if (nativeKeyCode < 0) {
        qDebug() << "DEBUG: Received a keypress we do not handle!";
        return;
    }
    
    if (key->duration) {
        emit static_cast<CECController *>(param)->keyPress(nativeKeyCode, false);
    } else {
        emit static_cast<CECController *>(param)->keyPress(nativeKeyCode, true);
    }
}

CECController::CECController()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup generalGroup = config->group("General");
    
    m_keyCodeTranslation = {
        { generalGroup.readEntry("ButtonPlay", (int) CEC_USER_CONTROL_CODE_PLAY), KEY_PLAY},
        { generalGroup.readEntry("ButtonStop", (int) CEC_USER_CONTROL_CODE_STOP), KEY_STOP},
        { generalGroup.readEntry("ButtonPause", (int) CEC_USER_CONTROL_CODE_PAUSE), KEY_PAUSE},
        { generalGroup.readEntry("ButtonRewind", (int) CEC_USER_CONTROL_CODE_REWIND), KEY_REWIND},
        { generalGroup.readEntry("ButtonFastforward", (int) CEC_USER_CONTROL_CODE_FAST_FORWARD), KEY_FASTFORWARD},
        { generalGroup.readEntry("ButtonEnter", (int) CEC_USER_CONTROL_CODE_SELECT), KEY_SELECT},
        { generalGroup.readEntry("ButtonUp", (int) CEC_USER_CONTROL_CODE_UP), KEY_UP},
        { generalGroup.readEntry("ButtonDown", (int) CEC_USER_CONTROL_CODE_DOWN), KEY_DOWN},
        { generalGroup.readEntry("ButtonLeft", (int) CEC_USER_CONTROL_CODE_LEFT), KEY_LEFT},
        { generalGroup.readEntry("ButtonRight", (int) CEC_USER_CONTROL_CODE_RIGHT), KEY_RIGHT},
        { generalGroup.readEntry("ButtonNumber0", (int) CEC_USER_CONTROL_CODE_NUMBER0), KEY_0},
        { generalGroup.readEntry("ButtonNumber1", (int) CEC_USER_CONTROL_CODE_NUMBER1), KEY_1},
        { generalGroup.readEntry("ButtonNumber2", (int) CEC_USER_CONTROL_CODE_NUMBER2), KEY_2},
        { generalGroup.readEntry("ButtonNumber3", (int) CEC_USER_CONTROL_CODE_NUMBER3), KEY_3},
        { generalGroup.readEntry("ButtonNumber4", (int) CEC_USER_CONTROL_CODE_NUMBER4), KEY_4},
        { generalGroup.readEntry("ButtonNumber5", (int) CEC_USER_CONTROL_CODE_NUMBER5), KEY_5},
        { generalGroup.readEntry("ButtonNumber6", (int) CEC_USER_CONTROL_CODE_NUMBER6), KEY_6},
        { generalGroup.readEntry("ButtonNumber7", (int) CEC_USER_CONTROL_CODE_NUMBER7), KEY_7},
        { generalGroup.readEntry("ButtonNumber8", (int) CEC_USER_CONTROL_CODE_NUMBER8), KEY_8},
        { generalGroup.readEntry("ButtonNumber9", (int) CEC_USER_CONTROL_CODE_NUMBER9), KEY_9},
        { generalGroup.readEntry("ButtonBlue", (int) CEC_USER_CONTROL_CODE_F1_BLUE), KEY_BLUE},
        { generalGroup.readEntry("ButtonRed", (int) CEC_USER_CONTROL_CODE_F2_RED), KEY_RED},
        { generalGroup.readEntry("ButtonGreen", (int) CEC_USER_CONTROL_CODE_F3_GREEN), KEY_GREEN},
        { generalGroup.readEntry("ButtonYellow", (int) CEC_USER_CONTROL_CODE_F4_YELLOW), KEY_YELLOW},
        { generalGroup.readEntry("ButtonChannelUp", (int) CEC_USER_CONTROL_CODE_CHANNEL_UP), KEY_CHANNELUP},
        { generalGroup.readEntry("ButtonChannelDown", (int) CEC_USER_CONTROL_CODE_CHANNEL_DOWN), KEY_CHANNELDOWN},
        { generalGroup.readEntry("ButtonExit", (int) CEC_USER_CONTROL_CODE_EXIT), KEY_EXIT},
        { generalGroup.readEntry("ButtonBack", (int) CEC_USER_CONTROL_CODE_AN_RETURN), KEY_BACK},
        { generalGroup.readEntry("ButtonHome", (int) CEC_USER_CONTROL_CODE_ROOT_MENU), KEY_HOME},
        { generalGroup.readEntry("ButtonSubtitle", (int) CEC_USER_CONTROL_CODE_SUB_PICTURE), KEY_SUBTITLE},
        { generalGroup.readEntry("ButtonInfo", (int) CEC_USER_CONTROL_CODE_DISPLAY_INFORMATION), KEY_INFO},
    };

    m_cecCallbacks.Clear();
    m_cecCallbacks.keyPress = &CECController::handleCecKeypress;
    
    libcec_configuration cecConfig;
    cecConfig.Clear();
    cecConfig.bActivateSource = 0;
    snprintf(cecConfig.strDeviceName, LIBCEC_OSD_NAME_SIZE, "joyclick");
    cecConfig.clientVersion = LIBCEC_VERSION_CURRENT;
    cecConfig.deviceTypes.Add(CEC_DEVICE_TYPE_RECORDING_DEVICE);
    cecConfig.callbacks = &m_cecCallbacks;
    cecConfig.callbackParam = this;
    
    m_cecAdapter = LibCecInitialise(&cecConfig);
    
    if (!m_cecAdapter) {
        qCritical() << "Could not create CEC adaptor with current config";
        exit(1);
    }
    
    // Init video on targets that need this
    m_cecAdapter->InitVideoStandalone();
}

void CECController::run()
{
    // TODO: keep trying to detect a device till we find one
    cec_adapter_descriptor devices[10];
    int devices_found = 0;
    
    while (devices_found == 0) {
        devices_found = m_cecAdapter->DetectAdapters(devices, 10, NULL);
        
        if (devices_found == 0)
            sleep(1);
    }
    
    if (!m_cecAdapter->Open(devices[0].strComName)) {
        qWarning() << "Could not open CEC device " << devices[0].strComPath << " " << devices[0].strComName;
        return;
    }
    
    qDebug() << "Succesfully opened CEC device";
    
    // Just live forever
    while (true) {}
}

CECController::~CECController() = default;


