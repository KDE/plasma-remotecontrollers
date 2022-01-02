#include "libcec/ceccontroller.h"
#include "wiimote/wiimotecontroller.h"
#include "notifier.h"
#include "devicetypes.h"

#include <QGuiApplication>
#include <QDebug>

#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    Notifier* notifier = new Notifier();
    Uinput* uinput = new Uinput();

    CECController* ct = new CECController();
    WiimoteController* wt = new WiimoteController(uinput);
    
    ct->start();
    wt->start();

    // Connect notification signals
    qRegisterMetaType<DeviceType>("DeviceType");
    QObject::connect(wt, SIGNAL(deviceConnected(DeviceType)),
                     notifier, SLOT(notifyNewDevice(DeviceType)));
    QObject::connect(wt, SIGNAL(deviceDisconnected(DeviceType)),
                     notifier, SLOT(notifyDisconnectedDevice(DeviceType)));

    // Connect uinput signals
    QObject::connect(ct, SIGNAL(keyPress(int, bool)),
                     uinput, SLOT(emitKey(int, bool)));

    return app.exec();
}
