find_package(PkgConfig REQUIRED)
pkg_check_modules(PKG_libcec QUIET libcec)

find_path(Libcec_INCLUDE_DIRS
    NAMES cec.h
    PATH_SUFFIXES libcec
    PATHS ${PKG_libcec_INCLUDEDIR} ${PKG_libcec_INCLUDE_DIRS}
)

find_library(Libcec_LIBRARIES
    NAMES cec
    PATHS ${PKG_libcec_LIBDIR} ${PKG_libcec_LIBRARY_DIRS}
)

set(Libcec_VERSION ${PKG_Libcec_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libcec
    FOUND_VAR
        Libcec_FOUND
    REQUIRED_VARS
        Libcec_INCLUDE_DIRS
        Libcec_LIBRARIES
    VERSION_VAR
        Libcec_VERSION
)
mark_as_advanced(
    Libcec_VERSION
)

if(Libcec_FOUND AND NOT TARGET Libcec::Libcec)
    add_library(Libcec::Libcec UNKNOWN IMPORTED)
    set_target_properties(Libcec::Libcec PROPERTIES
        IMPORTED_LOCATION "${Libcec_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${Libcec_INCLUDE_DIRS}"
    )
endif()

