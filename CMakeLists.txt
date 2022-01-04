cmake_minimum_required(VERSION 3.16)
project(joyclick)

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
find_package(Libcec REQUIRED)
find_package(XWiimote)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_executable(joyclick
    src/main.cpp
    src/uinput.cpp
    src/notifier.cpp
    src/controller.cpp
    src/libcec/ceccontroller.cpp)

target_link_libraries(joyclick
    Qt5::Core
    Qt5::Gui
    KF5::ConfigCore
    KF5::Notifications
    Libcec::Libcec
)

if(XWiimote_FOUND)
    target_sources(joyclick
        PUBLIC
            src/wiimote/wiimotecontroller.cpp
            src/wiimote/wiimote.cpp)
    target_link_libraries(joyclick XWiimote::XWiimote)
    add_definitions(-DHAS_XWIIMOTE)
endif()

install(TARGETS joyclick DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(org.kde.joyclick.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.joyclick.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.joyclick.desktop DESTINATION ${KDE_INSTALL_SYSCONFDIR}/xdg/autostart)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.joyclick.desktop DESTINATION ${KDE_INSTALL_APPDIR})

install(FILES joyclickrc DESTINATION ${KDE_INSTALL_CONFDIR}/xdg)
install(FILES joyclick.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})
