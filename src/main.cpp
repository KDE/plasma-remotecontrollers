#include "libcec/ceccontroller.h"
#include "notifier.h"
#include "devicetypes.h"

#ifdef HAS_XWIIMOTE
# include "wiimote/wiimotecontroller.h"
#endif // HAS_XWIIMOTE

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
    ct->start();

    QObject::connect(ct, SIGNAL(keyPress(int, bool)),
                     uinput, SLOT(emitKey(int, bool)));

#ifdef HAS_XWIIMOTE
    WiimoteController* wt = new WiimoteController(uinput);
    wt->start();
    QObject::connect(wt, SIGNAL(deviceConnected(DeviceType)),
                     notifier, SLOT(notifyNewDevice(DeviceType)));
    QObject::connect(wt, SIGNAL(deviceDisconnected(DeviceType)),
                     notifier, SLOT(notifyDisconnectedDevice(DeviceType)));
#endif // HAS_XWIIMOTE

    qRegisterMetaType<DeviceType>("DeviceType");

    return app.exec();
}
