cmake_minimum_required(VERSION 3.0)
project(joyclick)

set(QT_MIN_VERSION "5.15.0")

find_package(ECM CONFIG REQUIRED)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
)
find_package(XWiimote REQUIRED)
pkg_check_modules(LIBXWIIMOTE libxwiimote IMPORTED_TARGET)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

add_executable(joyclick
    src/main.cpp
    src/wiimote/wiimotecontroller.cpp
    src/wiimote/wiimote.cpp)

target_link_libraries(joyclick
    Qt5::Core
    Qt5::Gui
    XWiimote::XWiimote
)

install(TARGETS joyclick DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(org.kde.joyclick.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.joyclick.desktop @ONLY)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.joyclick.desktop DESTINATION ${KDE_INSTALL_APPDIR} )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
