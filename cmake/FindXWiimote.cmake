find_package(PkgConfig REQUIRED)
pkg_check_modules(PKG_xwiimote QUIET libxwiimote)

find_path(XWiimote_INCLUDE_DIRS
    NAMES xwiimote.h
    PATHS ${PKG_xwiimote_INCLUDEDIR} ${PKG_xwiimote_INCLUDE_DIRS}
)

find_library(XWiimote_LIBRARIES
    NAMES xwiimote
    PATHS ${PKG_xwiimote_LIBDIR} ${PKG_xwiimote_LIBRARY_DIRS}
)

set(XWiimote_VERSION ${PKG_xwiimote_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XWiimote
    FOUND_VAR
        XWiimote_FOUND
    REQUIRED_VARS
        XWiimote_INCLUDE_DIRS
        XWiimote_LIBRARIES
    VERSION_VAR
        XWiimote_VERSION
)
mark_as_advanced(
    XWiimote_VERSION
)

if(XWiimote_FOUND AND NOT TARGET XWiimote::XWiimote)
    add_library(XWiimote::XWiimote UNKNOWN IMPORTED)
    set_target_properties(XWiimote::XWiimote PROPERTIES
        IMPORTED_LOCATION "${XWiimote_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${XWiimote_INCLUDE_DIRS}"
    )
endif()
