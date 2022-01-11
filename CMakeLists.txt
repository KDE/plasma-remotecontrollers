cmake_minimum_required(VERSION 3.16)
project(plasma-remotecontrollers)

set(QT_MIN_VERSION "5.15.0")
set(KF_MIN_VERSION "5.68.0")

find_package(ECM CONFIG REQUIRED)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
)
find_package(KF5 ${KF_MIN_VERSION} COMPONENTS REQUIRED
    Config
    Notifications
)
find_package(Libcec 6 REQUIRED)
pkg_check_modules(WIIUSE IMPORTED_TARGET wiiuse)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_executable(plasma-remotecontrollers
    src/main.cpp
    src/controllermanager.cpp
    src/notificationsmanager.cpp
    src/device.cpp
    src/libcec/ceccontroller.cpp)

target_link_libraries(plasma-remotecontrollers
    ${CMAKE_DL_LIBS}
    Qt5::Core
    Qt5::Gui
    KF5::ConfigCore
    KF5::Notifications
    Libcec::Libcec
)

if(WIIUSE_FOUND)
    target_sources(plasma-remotecontrollers
        PUBLIC
            src/wiimote/wiimotecontroller.cpp
            src/wiimote/wiimote.cpp)
    target_link_libraries(plasma-remotecontrollers PkgConfig::WIIUSE)
    add_definitions(-DHAS_WIIUSE)
endif()

install(TARGETS plasma-remotecontrollers DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(org.kde.plasma-remotecontrollers.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop DESTINATION ${KDE_INSTALL_SYSCONFDIR}/xdg/autostart)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.plasma-remotecontrollers.desktop DESTINATION ${KDE_INSTALL_APPDIR})

install(FILES plasma-remotecontrollersrc DESTINATION ${KDE_INSTALL_CONFDIR}/xdg)
install(FILES plasma-remotecontrollers.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})
