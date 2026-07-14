# FindLibBiosig.cmake
#
# Finds a system-installed libbiosig (https://biosig.sourceforge.net/).
#
# libbiosig has no CMake package config, but does ship a pkg-config file
# (libbiosig.pc). When found, this module also links whatever transitive
# dependencies (iconv, expat, libxml2, ...) that build of libbiosig actually
# needed, via pkg-config's Libs.private — this varies per build, so it can't
# be hardcoded. Added unconditionally: if libbiosig is a shared library that
# already carries these itself, the extra links are harmless. Without a
# libbiosig.pc (e.g. a manual install), only the header/library are located,
# with no transitive dependencies.
#
# No version check is performed: libbiosig has no reliable way to report its
# release version anywhere (header macros, get_biosig_version(), and
# libbiosig.pc's Version field are all the same internal API/ABI counter,
# not the release version). Callers must ensure the installed version
# satisfies SigViewer's documented minimum (see CMakeLists.txt).
#
# Defines the imported target LibBiosig::LibBiosig on success.

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    # Looked up by the .pc filename (libbiosig.pc), not the "Name:" field
    # inside it (pkg-config never consults that for lookup).
    pkg_check_modules(PC_LIBBIOSIG QUIET libbiosig)
endif()

find_path(LibBiosig_INCLUDE_DIR
    NAMES biosig.h
    HINTS ${PC_LIBBIOSIG_INCLUDE_DIRS}
)
find_library(LibBiosig_LIBRARY
    NAMES biosig
    HINTS ${PC_LIBBIOSIG_LIBRARY_DIRS}
)

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

    # PC_LIBBIOSIG_STATIC_LIBRARIES is "Libs" + "Libs.private" combined (bare
    # names, e.g. "biosig;iconv;expat;xml2"). Drop "biosig" itself —
    # IMPORTED_LOCATION already points at the exact file — and link the rest.
    if(PC_LIBBIOSIG_FOUND)
        set(_libbiosig_transitive_libs ${PC_LIBBIOSIG_STATIC_LIBRARIES})
        list(REMOVE_ITEM _libbiosig_transitive_libs biosig)
        if(_libbiosig_transitive_libs)
            target_link_libraries(LibBiosig::LibBiosig INTERFACE ${_libbiosig_transitive_libs})
        endif()
        unset(_libbiosig_transitive_libs)
    endif()
endif()

mark_as_advanced(LibBiosig_INCLUDE_DIR LibBiosig_LIBRARY)
