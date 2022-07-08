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

K_PLUGIN_CLASS_WITH_JSON(RemoteController, "kcm_mediacenter_remotecontrollers.json")

RemoteController::RemoteController(QObject *parent, const QVariantList &args)
    : KQuickAddons::ConfigModule(parent, args)
{
    setButtons(Help);
    KAboutData *about = new KAboutData(QStringLiteral("kcm_mediacenter_remotecontrollers"), //
                                       i18n("Configure Remote Controllers"),
                                       QStringLiteral("2.0"),
                                       QString(),
                                       KAboutLicense::LGPL);
    setAboutData(about);
}

RemoteController::~RemoteController()
{
}

QString RemoteController::getCecKeyConfig(const QString key)
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-remotecontrollersrc"));
    static KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        return grp.readEntry(key, QString());
    }

    return "Null";
}

void RemoteController::setCecKeyConfig(const QString button, const QString key)
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-remotecontrollersrc"));
    static KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(button, key);
        grp.sync();
        emit cecConfigChanged(button);
    }
}

int RemoteController::getCecKeyFromRemotePress()
{
    QDBusMessage msg = QDBusMessage::createMethodCall("org.kde.plasma-remotecontrollers", "/CEC", "", "sendNextKey");
    QDBusMessage response = QDBusConnection::sessionBus().call(msg);
    QList<QVariant> responseArg = response.arguments();
    return responseArg.at(0).toInt();
}

#include "remotecontroller.moc"