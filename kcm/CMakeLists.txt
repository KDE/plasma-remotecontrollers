include(ECMFindQmlModule)
ecm_find_qmlmodule(org.kde.plasma.core 2.0)

# KI18N Translation Domain for this library.
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_mediacenter_remotecontrollers\")

set(kcm_mediacenter_remotecontrollers_SRCS remotecontroller.cpp kcmdbusinterface.cpp keydisplayprovider.cpp keymapmodel.cpp devicesmodel.cpp
    remotecontroller.h kcmdbusinterface.h keydisplayprovider.h keymapmodel.h devicesmodel.h)

kcmutils_add_qml_kcm(kcm_mediacenter_remotecontrollers SOURCES ${kcm_mediacenter_remotecontrollers_SRCS})
target_link_libraries(kcm_mediacenter_remotecontrollers
    Qt::DBus
    KF6::I18n
    KF6::KCMUtilsQuick
)
