#include "evdevdbusinterface.h"
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>

EvdevDbusInterface::EvdevDbusInterface(QObject *parent)
    : QObject(parent)
{
    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.plasma-remotecontrollers")) {
        QDBusConnection::sessionBus().registerService("org.kde.plasma-remotecontrollers");
    }
    QDBusConnection::sessionBus().registerObject("/EVDEV", this, QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllSlots);
}

EvdevDbusInterface::~EvdevDbusInterface()
{
}

void EvdevDbusInterface::emitKeyPress(int keyCode)
{
    QDBusMessage msg = QDBusMessage::createSignal("/EVDEV", "org.kde.plasma.remotecontrollers.EVDEV", "keyPress");
    QList<QVariant> arguments;
    arguments << QVariant(keyCode);
    msg.setArguments(arguments);
    QDBusConnection::sessionBus().send(msg);
}