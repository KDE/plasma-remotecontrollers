include(ECMFindQmlModule)
ecm_find_qmlmodule(org.kde.plasma.core 2.0)

# KI18N Translation Domain for this library.
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_mediacenter_remotecontrollers\")

set(kcm_mediacenter_remotecontrollers_SRCS remotecontroller.cpp kcmdbusinterface.cpp keydisplayprovider.cpp keymapmodel.cpp devicesmodel.cpp
    remotecontroller.h kcmdbusinterface.h keydisplayprovider.h keymapmodel.h devicesmodel.h)

kcoreaddons_add_plugin(kcm_mediacenter_remotecontrollers SOURCES ${kcm_mediacenter_remotecontrollers_SRCS} INSTALL_NAMESPACE "plasma/kcms/systemsettings")
kcmutils_generate_desktop_file (kcm_mediacenter_remotecontrollers)

target_link_libraries(kcm_mediacenter_remotecontrollers
    Qt::DBus
    KF6::I18n
    KF6::KCMUtils
    KF6::QuickAddons
)

kpackage_install_package(package kcm_mediacenter_remotecontrollers kcms)
