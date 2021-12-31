#include "libcec/ceccontroller.h"
#include "wiimote/wiimotecontroller.h"

#include <QGuiApplication>
#include <QDebug>

#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Setup uinput
    struct uinput_setup usetup;
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    
    if (fd < 0) {
        qCritical() << "Couldn't open /dev/uinput, make sure the kernel module is loaded";
        exit(1);
    }
    
    // Register all keys we want to press with this application
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, KEY_PLAY);
    ioctl(fd, UI_SET_KEYBIT, KEY_STOP);
    ioctl(fd, UI_SET_KEYBIT, KEY_PAUSE);
    ioctl(fd, UI_SET_KEYBIT, KEY_REWIND);
    ioctl(fd, UI_SET_KEYBIT, KEY_FASTFORWARD);
    ioctl(fd, UI_SET_KEYBIT, KEY_ENTER);
    ioctl(fd, UI_SET_KEYBIT, KEY_SELECT);
    ioctl(fd, UI_SET_KEYBIT, KEY_UP);
    ioctl(fd, UI_SET_KEYBIT, KEY_DOWN);
    ioctl(fd, UI_SET_KEYBIT, KEY_LEFT);
    ioctl(fd, UI_SET_KEYBIT, KEY_RIGHT);
    ioctl(fd, UI_SET_KEYBIT, KEY_1);
    ioctl(fd, UI_SET_KEYBIT, KEY_2);
    ioctl(fd, UI_SET_KEYBIT, KEY_3);
    ioctl(fd, UI_SET_KEYBIT, KEY_4);
    ioctl(fd, UI_SET_KEYBIT, KEY_5);
    ioctl(fd, UI_SET_KEYBIT, KEY_6);
    ioctl(fd, UI_SET_KEYBIT, KEY_7);
    ioctl(fd, UI_SET_KEYBIT, KEY_8);
    ioctl(fd, UI_SET_KEYBIT, KEY_9);
    ioctl(fd, UI_SET_KEYBIT, KEY_BLUE);
    ioctl(fd, UI_SET_KEYBIT, KEY_RED);
    ioctl(fd, UI_SET_KEYBIT, KEY_GREEN);
    ioctl(fd, UI_SET_KEYBIT, KEY_YELLOW);
    ioctl(fd, UI_SET_KEYBIT, KEY_CHANNELUP);
    ioctl(fd, UI_SET_KEYBIT, KEY_CHANNELDOWN);
    ioctl(fd, UI_SET_KEYBIT, KEY_EXIT);
    ioctl(fd, UI_SET_KEYBIT, KEY_BACK);
    ioctl(fd, UI_SET_KEYBIT, KEY_HOME);
    ioctl(fd, UI_SET_KEYBIT, KEY_MENU);
    ioctl(fd, UI_SET_KEYBIT, KEY_SUBTITLE);
    ioctl(fd, UI_SET_KEYBIT, KEY_MINUS);
    
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; // TODO: change vendor
    usetup.id.product = 0x5678; // TODO: change product
    strcpy(usetup.name, "Wiimote");
    
    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);
    sleep(1);

    CECController* ct = new CECController(fd);
    WiimoteController* wt = new WiimoteController(fd);
    
    ct->start();
    wt->start();

    return app.exec();
}
