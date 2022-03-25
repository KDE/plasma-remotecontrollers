cmake_minimum_required(VERSION 3.16)
project(plasma-remotecontrollers VERSION 1.0)

set(QT_MIN_VERSION "5.15.0")
set(KF_MIN_VERSION "5.68.0")

find_package(ECM CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(FeatureSummary)
include(ECMQtDeclareLoggingCategory)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

find_package(PkgConfig REQUIRED)
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Gui
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
)
pkg_check_modules(Libcec REQUIRED IMPORTED_TARGET libcec>=6)
add_feature_info(Libcec Libcec_FOUND "Adds support forr CEC devices")

pkg_check_modules(XWiimote IMPORTED_TARGET libxwiimote)
add_feature_info(XWiimote XWiimote_FOUND "Adds support for Wiimotes")

pkg_check_modules(Evdev REQUIRED IMPORTED_TARGET libevdev)
add_feature_info(Evdev Evdev_FOUND "Adds support for controllers")

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

ecm_qt_declare_logging_category(SRCS HEADER plasmarc-evdev-debug.h IDENTIFIER PLASMARC_EVDEV CATEGORY_NAME org.kde.plasma.remotecontrollers.evdev DESCRIPTION "Plasma Remote Controllers - evdev" DEFAULT_SEVERITY Info EXPORT PLASMARC)

add_executable(plasma-remotecontrollers
    src/main.cpp
    src/controllermanager.cpp
    src/notificationsmanager.cpp
    src/device.cpp
    src/libcec/ceccontroller.cpp
    src/evdev/evdevcontroller.cpp
    ${SRCS}
)

target_link_libraries(plasma-remotecontrollers
    ${CMAKE_DL_LIBS}
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    KF5::ConfigCore
    KF5::Notifications
    KF5::Solid
    KF5::I18n
    PkgConfig::Libcec
    PkgConfig::Evdev
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

install(TARGETS plasma-remotecontrollers DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(org.kde.plasma-remotecontrollers.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop DESTINATION ${KDE_INSTALL_SYSCONFDIR}/xdg/autostart)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop DESTINATION ${KDE_INSTALL_APPDIR})

install(FILES plasma-remotecontrollersrc DESTINATION ${KDE_INSTALL_CONFDIR})
install(FILES plasma-remotecontrollers.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR})
install(FILES 40-uinput.rules DESTINATION ${KDE_INSTALL_LIBDIR}/udev/rules.d)

add_subdirectory(kcm)

ecm_qt_install_logging_categories(
        EXPORT PLASMARC
        FILE plasma-remotecontrollers.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
    )
