/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontroller.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusConnectionInterface>
#include <QtQml>

K_PLUGIN_CLASS_WITH_JSON(RemoteController, "kcm_mediacenter_remotecontrollers.json")

RemoteController::RemoteController(QObject *parent, const QVariantList &args)
    : KQuickAddons::ConfigModule(parent, args)
    , m_keyMapModel(new KeyMapModel(this))
{
    setButtons(Help);
    KAboutData *about = new KAboutData(QStringLiteral("kcm_mediacenter_remotecontrollers"), //
                                       i18n("Configure Remote Controllers"),
                                       QStringLiteral("2.0"),
                                       QString(),
                                       KAboutLicense::LGPL);
    setAboutData(about);

    // connect to the evdevKeyPressed signal from kcmDbusInterface
    connect(&kcmDbusInterface, &KcmDbusInterface::evdevKeyPressed, this, [this](int keyCode) {
        emit gamepadKeyPressed(keyCode);
    });

    const QByteArray uri("org.kde.private.kcm.remotecontrollers");
    qmlRegisterUncreatableType<KeyMapModel>(uri, 1, 0, "KeyMapModel", QStringLiteral("Cannot create an item of type KeyMapModel"));
}

RemoteController::~RemoteController()
{
}

QString RemoteController::cecKeyConfig(const QString &key)
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-remotecontrollersrc"));
    static KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        return grp.readEntry(key, QString());
    }

    return "Null";
}

QString RemoteController::gamepadKeyConfig(const QString &key)
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-remotecontrollersrc"));
    static KConfigGroup grp(config, QLatin1String("Gamepad"));

    if (grp.isValid()) {
        return grp.readEntry(key, QString());
    }

    return "Null";
}

void RemoteController::setCecKeyConfig(const QString &button, const QString &key)
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-remotecontrollersrc"));
    static KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(button, key);
        grp.sync();
        emit cecConfigChanged(button);
    }

    m_keyMapModel->refresh();
}

void RemoteController::setGamepadKeyConfig(const QString &button, const QString &key)
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-remotecontrollersrc"));
    static KConfigGroup grp(config, QLatin1String("Gamepad"));

    if (grp.isValid()) {
        grp.writeEntry(button, key);
        grp.sync();
        emit gamepadConfigChanged(button);
    }

    m_keyMapModel->refresh();
}

int RemoteController::cecKeyFromRemotePress()
{
    QDBusMessage msg = QDBusMessage::createMethodCall("org.kde.plasma-remotecontrollers", "/CEC", "", "sendNextKey");
    QDBusMessage response = QDBusConnection::sessionBus().call(msg);
    QList<QVariant> responseArg = response.arguments();
    return responseArg.at(0).toInt();
}

KeyMapModel *RemoteController::keyMapModel()
{
    if (!m_keyMapModel) {
        m_keyMapModel = new KeyMapModel(this);
    }
    return m_keyMapModel;
}

#include "remotecontroller.moc"