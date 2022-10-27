ecm_qt_declare_logging_category(SRCS HEADER plasmarc-evdev-debug.h IDENTIFIER PLASMARC_EVDEV CATEGORY_NAME org.kde.plasma.remotecontrollers.evdev DESCRIPTION "Plasma Remote Controllers - evdev" DEFAULT_SEVERITY Info EXPORT PLASMARC)

ecm_add_qtwayland_client_protocol(SRCS
    PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/fake-input.xml
    BASENAME fake-input
)

add_executable(plasma-remotecontrollers
    main.cpp
    controllermanager.cpp
    notificationsmanager.cpp
    device.cpp
    abstractsystem.cpp
    uinputsystem.cpp
    kwinfakeinputsystem.cpp
    evdev/evdevcontroller.cpp
    evdev/evdevdbusinterface.cpp
    devicesmodel.cpp
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
    KF5::DBusAddons
    KF5::CoreAddons
    PW::LibTaskManager
    PkgConfig::Evdev
    Wayland::Client
)
target_compile_definitions(plasma-remotecontrollers PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

add_library(remotecontrollersplugin SHARED)
kconfig_add_kcfg_files(remotecontrollersplugin remotecontrollers.kcfgc GENERATE_MOC)
kconfig_add_kcfg_files(plasma-remotecontrollers remotecontrollers.kcfgc GENERATE_MOC)

target_sources(remotecontrollersplugin PRIVATE
    remotecontrollersplugin.cpp
    devicesmodel.cpp
    controllermanager.cpp
    notificationsmanager.cpp
    device.cpp
    abstractsystem.cpp
    uinputsystem.cpp
    kwinfakeinputsystem.cpp
    evdev/evdevcontroller.cpp
    evdev/evdevdbusinterface.cpp
    ${SRCS}
)

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
    PW::LibTaskManager
    PkgConfig::Evdev
    Wayland::Client
)

if(XWiimote_FOUND)
    set(WIIMOTE_SRCS
        wiimote/wiimotecontroller.cpp
        wiimote/wiimote.cpp)
    target_sources(plasma-remotecontrollers PUBLIC ${WIIMOTE_SRCS})
    target_sources(remotecontrollersplugin PUBLIC ${WIIMOTE_SRCS})
    target_link_libraries(plasma-remotecontrollers PkgConfig::XWiimote)
    target_link_libraries(remotecontrollersplugin PkgConfig::XWiimote)
    add_definitions(-DHAS_XWIIMOTE)
endif()

if (TARGET PkgConfig::Libcec)
    set(LIBCEC_SRCS libcec/ceccontroller.cpp)
    target_sources(plasma-remotecontrollers PUBLIC ${LIBCEC_SRCS})
    target_sources(remotecontrollersplugin PUBLIC ${LIBCEC_SRCS})
    target_link_libraries(plasma-remotecontrollers PkgConfig::Libcec)
    target_link_libraries(remotecontrollersplugin PkgConfig::Libcec)
    add_definitions(-DHAS_LIBCEC)
endif()


install(TARGETS remotecontrollersplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/remotecontrollers)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/remotecontrollers)

install(FILES libcec/org.kde.plasma-remotecontrollers.CEC.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES evdev/org.kde.plasma-remotecontrollers.EVDEV.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

install(TARGETS plasma-remotecontrollers DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
