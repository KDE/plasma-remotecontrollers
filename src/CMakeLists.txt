ecm_qt_declare_logging_category(SRCS HEADER plasmarc-evdev-debug.h IDENTIFIER PLASMARC_EVDEV CATEGORY_NAME org.kde.plasma.remotecontrollers.evdev DESCRIPTION "Plasma Remote Controllers - evdev" DEFAULT_SEVERITY Info EXPORT PLASMARC)
ecm_qt_declare_logging_category(SRCS HEADER plasmarc-debug.h IDENTIFIER PLASMARC CATEGORY_NAME org.kde.plasma.remotecontrollers DESCRIPTION "Plasma Remote Controllers" DEFAULT_SEVERITY Info EXPORT PLASMARC)

add_executable(plasma-remotecontrollers)
qt6_generate_wayland_protocol_client_sources(plasma-remotecontrollers FILES ${PLASMA_WAYLAND_PROTOCOLS_DIR}/fake-input.xml)
qt_add_dbus_interface(SRCS ${SCREENSAVER_DBUS_INTERFACE} screensaver_interface)

target_sources(plasma-remotecontrollers PRIVATE
    main.cpp
    controllermanager.cpp
    notificationsmanager.cpp
    device.cpp
    abstractsystem.cpp
    uinputsystem.cpp
    kwinfakeinputsystem.cpp
    evdev/evdevcontroller.cpp
    evdev/evdevdbusinterface.cpp
    controllermanagerdbusinterface.cpp
    controllermanager.h
    notificationsmanager.h
    device.h
    abstractsystem.h
    uinputsystem.h
    kwinfakeinputsystem.h
    evdev/evdevcontroller.h
    evdev/evdevdbusinterface.h
    controllermanagerdbusinterface.h
    ${SRCS}
)

target_link_libraries(plasma-remotecontrollers
    ${CMAKE_DL_LIBS}
    Qt::Core
    Qt::DBus
    Qt::Gui
    Qt::WaylandClient
    KF6::ConfigCore
    KF6::Notifications
    KF6::Solid
    KF6::I18n
    KF6::DBusAddons
    KF6::CoreAddons
    KF6::WindowSystem
    KF6::StatusNotifierItem
    PW::LibTaskManager
    PkgConfig::Evdev
    Wayland::Client
)
target_compile_definitions(plasma-remotecontrollers PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

kconfig_add_kcfg_files(plasma-remotecontrollers remotecontrollers.kcfgc GENERATE_MOC)

if(XWiimote_FOUND)
    set(WIIMOTE_SRCS
        wiimote/wiimotecontroller.cpp wiimote/wiimotecontroller.h
        wiimote/wiimote.cpp)
    target_sources(plasma-remotecontrollers PUBLIC ${WIIMOTE_SRCS})
    target_link_libraries(plasma-remotecontrollers PkgConfig::XWiimote)
    add_definitions(-DHAS_XWIIMOTE)
endif()

if (TARGET PkgConfig::Libcec)
    set(LIBCEC_SRCS libcec/ceccontroller.cpp libcec/ceccontroller.h)
    target_sources(plasma-remotecontrollers PUBLIC ${LIBCEC_SRCS})
    target_link_libraries(plasma-remotecontrollers PkgConfig::Libcec)
    add_definitions(-DHAS_LIBCEC)
endif()

install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/remotecontrollers)

install(FILES libcec/org.kde.plasma.remotecontrollers.CEC.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES evdev/org.kde.plasma.remotecontrollers.EVDEV.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES org.kde.plasma.remotecontrollers.ControllerManager.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

install(TARGETS plasma-remotecontrollers DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
