/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "controllermanager.h"
#include "evdev/evdevcontroller.h"

#ifdef HAS_LIBCEC
#include "libcec/ceccontroller.h"
#endif
#ifdef HAS_XWIIMOTE
# include "wiimote/wiimotecontroller.h"
#endif // HAS_XWIIMOTE
#ifdef HAS_JOYSTICK
# include "joystick/devicemodel.h"
#endif // HAS_XWIIMOTE



#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDBusConnection>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <KAboutData>
#include <KDBusService>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // KStatusNotifierItem needs a QApplication
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("plasma-remotecontrollers");

    KAboutData about(QStringLiteral("plasma-remotecontrollers"),
                     i18n("Plasma Remote Controllers"), PROJECT_VERSION,
                     i18n("System update status notifier"), KAboutLicense::GPL,
                     i18n("© 2022 Plasma Development Team"));
    about.setProductName("Plasma Bigscreen/Remote Controllers");
    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                        i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    KAboutData::setApplicationData(about);

    KDBusService::StartupOptions startup = {};
    {
        QCommandLineParser parser;
        QCommandLineOption replaceOption({QStringLiteral("replace")},
                                        i18n("Replace an existing instance"));
        parser.addOption(replaceOption);
        about.setupCommandLine(&parser);
        parser.process(app);
        about.processCommandLine(&parser);

        if (parser.isSet(replaceOption)) {
            startup |= KDBusService::Replace;
        }
    }

    KDBusService service(KDBusService::Unique | startup);

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup generalGroup = config->group("General");
    if (generalGroup.readEntry("EnableEvdev", false)) {
        new EvdevController();
    }

#ifdef HAS_LIBCEC
    if (generalGroup.readEntry("EnableCEC", true)) {
        new CECController();
    }
#endif

#ifdef HAS_JOYSTICK
    if (generalGroup.readEntry("EnableJoystick", true)) {
        new DeviceModel();
    }
#endif // HAS_JOYSTICK


#ifdef HAS_XWIIMOTE
    if (generalGroup.readEntry("EnableWiimote", true)) {
        new WiimoteController();
    }
#endif // HAS_XWIIMOTE

    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning() << "Cannot connect to the D-Bus session bus.\nPlease check your system settings and try again.";
        return 1;
    }

    return app.exec();
}
