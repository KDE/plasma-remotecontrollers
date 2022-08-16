cmake_minimum_required(VERSION 3.16)
project(plasma-remotecontrollers)
set(PROJECT_VERSION "5.25.80")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.15.0")
set(KF_MIN_VERSION "5.68.0")

find_package(ECM CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(FeatureSummary)
include(ECMQtDeclareLoggingCategory)
include(CMakePackageConfigHelpers)
include(ECMInstallIcons)
include(ECMPackageConfigHelpers)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

find_package(PkgConfig REQUIRED)
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
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
    KCMUtils
    Package
    WindowSystem
)
find_package(Wayland REQUIRED COMPONENTS Client)
pkg_check_modules(Libcec IMPORTED_TARGET libcec>=6)
add_feature_info(Libcec Libcec_FOUND "Adds support forr CEC devices")

pkg_check_modules(XWiimote IMPORTED_TARGET libxwiimote)
add_feature_info(XWiimote XWiimote_FOUND "Adds support for Wiimotes, not currently recommended due to unmaintained Wiimote")

pkg_check_modules(Evdev REQUIRED IMPORTED_TARGET libevdev)
add_feature_info(Evdev Evdev_FOUND "Adds support for controllers")

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

ecm_qt_declare_logging_category(SRCS HEADER plasmarc-evdev-debug.h IDENTIFIER PLASMARC_EVDEV CATEGORY_NAME org.kde.plasma.remotecontrollers.evdev DESCRIPTION "Plasma Remote Controllers - evdev" DEFAULT_SEVERITY Info EXPORT PLASMARC)

set(CMAKE_CXX_STANDARD 17)

find_package(PlasmaWaylandProtocols REQUIRED)
find_package(QtWaylandScanner REQUIRED)

ecm_add_qtwayland_client_protocol(SRCS
    PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/fake-input.xml
    BASENAME fake-input
)

add_executable(plasma-remotecontrollers
    src/main.cpp
    src/controllermanager.cpp
    src/notificationsmanager.cpp
    src/device.cpp
    src/abstractsystem.cpp
    src/uinputsystem.cpp
    src/kwinfakeinputsystem.cpp
    src/evdev/evdevcontroller.cpp
    src/evdev/evdevdbusinterface.cpp
    src/devicesmodel.cpp
    ${SRCS}
)

target_link_libraries(plasma-remotecontrollers
    ${CMAKE_DL_LIBS}
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    Qt5::WaylandClient
    KF5::ConfigCore
    KF5::Notifications
    KF5::Solid
    KF5::I18n
    KF5::WindowSystem
    PkgConfig::Evdev
    Wayland::Client
)

if(XWiimote_FOUND)
    target_sources(plasma-remotecontrollers
        PUBLIC
            src/wiimote/wiimotecontroller.cpp
            src/wiimote/wiimote.cpp)
    target_link_libraries(plasma-remotecontrollers PkgConfig::XWiimote)
    add_definitions(-DHAS_XWIIMOTE)
endif()

install(FILES src/libcec/org.kde.plasma-remotecontrollers.CEC.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES src/evdev/org.kde.plasma-remotecontrollers.EVDEV.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

install(TARGETS plasma-remotecontrollers DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(org.kde.plasma-remotecontrollers.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop DESTINATION ${KDE_INSTALL_SYSCONFDIR}/xdg/autostart)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop DESTINATION ${KDE_INSTALL_APPDIR})

install(FILES plasma-remotecontrollersrc DESTINATION ${KDE_INSTALL_CONFDIR})
install(FILES plasma-remotecontrollers.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR})
install(FILES 40-uinput.rules DESTINATION ${KDE_INSTALL_LIBDIR}/udev/rules.d)

add_library(remotecontrollersplugin SHARED)

target_sources(remotecontrollersplugin PRIVATE
    src/remotecontrollersplugin.cpp
    src/devicesmodel.cpp
    src/controllermanager.cpp
    src/notificationsmanager.cpp
    src/device.cpp
    src/abstractsystem.cpp
    src/uinputsystem.cpp
    src/kwinfakeinputsystem.cpp
    src/evdev/evdevcontroller.cpp
    src/evdev/evdevdbusinterface.cpp
    ${SRCS}
)

if (TARGET PkgConfig::Libcec)
    target_sources(remotecontrollersplugin
        PUBLIC src/libcec/ceccontroller.cpp)
    target_link_libraries(remotecontrollersplugin PkgConfig::Libcec)
    add_definitions(-DHAS_LIBCEC)
endif()

target_link_libraries(remotecontrollersplugin
    ${CMAKE_DL_LIBS}
    Qt::Quick
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    Qt5::WaylandClient
    KF5::ConfigCore
    KF5::Notifications
    KF5::Solid
    KF5::I18n
    KF5::WindowSystem
    PkgConfig::Evdev
    Wayland::Client
)

install(TARGETS remotecontrollersplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/remotecontrollers)
install(FILES src/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/remotecontrollers)

add_subdirectory(kcm)

ecm_qt_install_logging_categories(
        EXPORT PLASMARC
        FILE plasma-remotecontrollers.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
    )
