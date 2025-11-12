cmake_minimum_required(VERSION 3.16)
project(plasma-remotecontrollers)
set(PROJECT_VERSION "6.4.50")
set(PROJECT_VERSION_MAJOR 6)

set(QT_MIN_VERSION "6.4.0")
set(KF_MIN_VERSION "6.0.0")

find_package(ECM ${KF_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(FeatureSummary)
include(ECMQtDeclareLoggingCategory)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

find_package(PkgConfig REQUIRED)
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Gui
    WaylandClient
)
find_package(KF6 ${KF_MIN_VERSION} COMPONENTS REQUIRED
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
    StatusNotifierItem
)
find_package(Wayland REQUIRED COMPONENTS Client)
find_package(ScreenSaverDBusInterface CONFIG REQUIRED)

pkg_check_modules(Libcec IMPORTED_TARGET libcec>=6)
add_feature_info(Libcec Libcec_FOUND "Adds support for CEC devices")

find_package(SDL3)
set_package_properties(SDL3 PROPERTIES
        TYPE OPTIONAL
        PURPOSE "Required for building SDL3 joystick support"
)
add_feature_info(SDL3 SDL3_FOUND "Adds support for joystick using SDL3")

pkg_check_modules(XWiimote IMPORTED_TARGET libxwiimote)
add_feature_info(XWiimote XWiimote_FOUND "Adds support for Wiimotes, not currently recommended due to unmaintained Wiimote")

pkg_check_modules(Evdev REQUIRED IMPORTED_TARGET libevdev)
add_feature_info(Evdev Evdev_FOUND "Adds support for controllers")

find_package(LibTaskManager REQUIRED)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_CXX_STANDARD 17)

find_package(PlasmaWaylandProtocols REQUIRED)

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
