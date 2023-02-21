cmake_minimum_required(VERSION 3.16)
project(plasma-remotecontrollers)
set(PROJECT_VERSION "5.27.1")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.15.2")
set(KF_MIN_VERSION "5.68.0")

find_package(ECM CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(FeatureSummary)
include(ECMQtDeclareLoggingCategory)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

find_package(PkgConfig REQUIRED)
find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Gui
    WaylandClient
)
find_package(KF5 ${KF_MIN_VERSION} COMPONENTS REQUIRED
    Config
    Notifications
    I18n
    Solid
    CoreAddons
    Declarative
    DBusAddons
    KCMUtils
    Package
    WindowSystem
)
find_package(Wayland REQUIRED COMPONENTS Client)
find_package(ScreenSaverDBusInterface CONFIG REQUIRED)

pkg_check_modules(Libcec IMPORTED_TARGET libcec>=6)
add_feature_info(Libcec Libcec_FOUND "Adds support forr CEC devices")

pkg_check_modules(XWiimote IMPORTED_TARGET libxwiimote)
add_feature_info(XWiimote XWiimote_FOUND "Adds support for Wiimotes, not currently recommended due to unmaintained Wiimote")

pkg_check_modules(Evdev REQUIRED IMPORTED_TARGET libevdev)
add_feature_info(Evdev Evdev_FOUND "Adds support for controllers")

find_package(LibTaskManager REQUIRED)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_CXX_STANDARD 17)

find_package(PlasmaWaylandProtocols REQUIRED)
if (QT_MAJOR_VERSION EQUAL "5")
    find_package(QtWaylandScanner REQUIRED)
endif()

add_subdirectory(src)
add_subdirectory(kcm)

configure_file(org.kde.plasma-remotecontrollers.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop DESTINATION ${KDE_INSTALL_SYSCONFDIR}/xdg/autostart)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop DESTINATION ${KDE_INSTALL_APPDIR})

install(FILES plasma-remotecontrollersrc DESTINATION ${KDE_INSTALL_CONFDIR})
install(FILES plasma-remotecontrollers.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
install(FILES 40-uinput.rules DESTINATION ${KDE_INSTALL_LIBDIR}/udev/rules.d)

install(FILES org.kde.plasma.remotecontrollers.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_qt_install_logging_categories(
        EXPORT PLASMARC
        FILE plasma-remotecontrollers.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
    )

ki18n_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
