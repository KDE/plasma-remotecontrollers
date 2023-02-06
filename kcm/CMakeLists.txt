include(ECMFindQmlModule)
ecm_find_qmlmodule(org.kde.plasma.core 2.0)

# KI18N Translation Domain for this library.
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_mediacenter_remotecontrollers\")

########### next target ###############

set(kcm_mediacenter_remotecontrollers_PART_SRCS remotecontroller.cpp kcmdbusinterface.cpp keydisplayprovider.cpp keymapmodel.cpp devicesmodel.cpp
    remotecontroller.h kcmdbusinterface.h keydisplayprovider.h keymapmodel.h devicesmodel.h)

add_library(kcm_mediacenter_remotecontrollers MODULE ${kcm_mediacenter_remotecontrollers_PART_SRCS})

target_link_libraries(kcm_mediacenter_remotecontrollers
    Qt::DBus
    KF5::I18n
    KF5::KCMUtils
    KF5::QuickAddons
)

########### install files ###############

install(TARGETS kcm_mediacenter_remotecontrollers DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)
install(FILES kcm_mediacenter_remotecontrollers.desktop  DESTINATION  ${KDE_INSTALL_KSERVICESDIR})
kpackage_install_package(package kcm_mediacenter_remotecontrollers kcms)
