# FindLibBiosig.cmake
#
# Finds a system-installed libbiosig (https://biosig.sourceforge.net/).
#
# libbiosig ships neither a CMake package config nor a pkg-config file, so it
# cannot be located via find_package(... CONFIG) or PkgConfig. It also has no
# reliable way to report its release version at build time: the
# BIOSIG_VERSION_* macros in its headers (and the get_biosig_version()
# runtime function, which is computed from those same macros) are an
# internal API/ABI counter that libbiosig bumps on interface changes, not
# the package/release version — so they can't be used to check for a
# minimum release version. Because of that, this module performs no version
# check; it only verifies that the header and library are present. Callers
# are responsible for ensuring the installed version satisfies SigViewer's
# documented minimum (see CMakeLists.txt).
#
# Defines the imported target LibBiosig::LibBiosig on success.

find_path(LibBiosig_INCLUDE_DIR NAMES biosig.h)
find_library(LibBiosig_LIBRARY NAMES biosig)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibBiosig
    REQUIRED_VARS LibBiosig_LIBRARY LibBiosig_INCLUDE_DIR
)

if(LibBiosig_FOUND AND NOT TARGET LibBiosig::LibBiosig)
    add_library(LibBiosig::LibBiosig UNKNOWN IMPORTED)
    set_target_properties(LibBiosig::LibBiosig PROPERTIES
        IMPORTED_LOCATION             "${LibBiosig_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibBiosig_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(LibBiosig_INCLUDE_DIR LibBiosig_LIBRARY)
