#include "controllermanager.h"
#include "notificationsmanager.h"

#include <QDebug>

#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>

ControllerManager::ControllerManager(QObject *parent)
    : QObject(parent)
{
    // Setup notifications
    QObject::connect(this, &ControllerManager::deviceConnected,
                     &NotificationsManager::instance(), &NotificationsManager::notifyNewDevice);
    QObject::connect(this, &ControllerManager::deviceDisconnected,
                     &NotificationsManager::instance(), &NotificationsManager::notifyDisconnectedDevice);

    struct uinput_setup usetup;
    m_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    
    if (m_fd < 0) {
        qCritical() << "Couldn't open /dev/uinput, make sure the kernel module is loaded";
        exit(1);
    }
    
    // Register all keys we want to press with this application
    ioctl(m_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_PLAY);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_STOP);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_PAUSE);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_REWIND);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_FASTFORWARD);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_ENTER);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_SELECT);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_UP);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_DOWN);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_LEFT);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_RIGHT);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_0);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_1);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_2);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_3);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_4);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_5);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_6);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_7);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_8);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_9);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_BLUE);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_RED);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_GREEN);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_YELLOW);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_CHANNELUP);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_CHANNELDOWN);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_EXIT);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_BACK);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_HOME);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_MENU);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_SUBTITLE);
    ioctl(m_fd, UI_SET_KEYBIT, KEY_MINUS);
    
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; // TODO: change vendor
    usetup.id.product = 0x5678; // TODO: change product
    strcpy(usetup.name, "Wiimote");
    
    ioctl(m_fd, UI_DEV_SETUP, &usetup);
    ioctl(m_fd, UI_DEV_CREATE);
    sleep(1);
}

ControllerManager &ControllerManager::instance()
{
    static ControllerManager _instance;
    return _instance;
}

void ControllerManager::newDevice(Device *device)
{
    qInfo() << "New device connected:" << device->getName();

    int index = m_connectedDevices.size() - 1;
    device->setIndex(index < 0 ? 0 : index);

    QThread *deviceThread = new QThread();
    device->moveToThread(deviceThread);
    connect(deviceThread, &QThread::started, device, &Device::watchEvents);
    connect(device, &Device::deviceDisconnected, this, &ControllerManager::removeDevice);

    DeviceEntry *deviceEntry = new DeviceEntry {
        device,
        deviceThread
    };

    m_connectedDevices.append(deviceEntry);
    deviceThread->start();

    // Don't send notifications for CEC devices, since we expect them to always be available
    if (device->getDeviceType() != DeviceCEC)
        emit deviceConnected(device);
}

void ControllerManager::removeDevice(int deviceIndex)
{
    DeviceEntry *removedDevice = m_connectedDevices.at(deviceIndex);
    m_connectedDevices.remove(deviceIndex);

    qInfo() << "Device disconnected:" << removedDevice->device->getName();

    removedDevice->thread->quit();
    removedDevice->thread->wait();

    emit deviceDisconnected(removedDevice->device);
    delete removedDevice;
}

bool ControllerManager::isConnected(QString uniqueIdentifier)
{
    if (m_connectedDevices.size() < 1)
        return false;

    return std::find_if(m_connectedDevices.begin(), m_connectedDevices.end(), [&uniqueIdentifier](DeviceEntry *other) {
        return other->device->getUniqueIdentifier() == uniqueIdentifier;
    }) != m_connectedDevices.end();
}

void ControllerManager::emitKey(int key, bool pressed)
{
    emitEvent(EV_KEY, key, pressed ? 1 : 0);
    emitEvent(EV_SYN, SYN_REPORT, 0);
}

void ControllerManager::emitEvent(int type, int code, int val)
{
    struct input_event ie;
    
    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;
    
    write(m_fd, &ie, sizeof(ie));
}

ControllerManager::~ControllerManager()
{
    for (int i = 0; i < m_connectedDevices.size(); i++) {
        m_connectedDevices.at(i)->thread->quit();
        m_connectedDevices.at(i)->thread->wait();
    }

    m_connectedDevices.clear();
}
