# build_deps.cmake
#
# Builds the external dependencies of SigViewer from source:
#   libxdf    — downloaded from the GitHub releases page and built with CMake
#   libbiosig — downloaded from SourceForge and built with autoconf and make
#
# Both are installed into the external/ directory, and external/versions.cmake
# is written so that the main build can confirm the correct versions are present.
#
# Usage (from the repository root):
#
#   cmake [-DDEPS_DIR=<path>] [-DFORCE_REBUILD=ON] -P external/build_deps.cmake
#
# DEPS_DIR defaults to <repo>/external. Override it if you want the
# artifacts placed elsewhere and will pass the same path via -DDEPS_DIR=
# when configuring the main build.
#
# By default each library is skipped when it is already present at the
# correct version (as recorded in external/versions.cmake). Pass
# -DFORCE_REBUILD=ON to always rebuild everything from scratch, or use
# -DFORCE_REBUILD_LIBXDF=ON / -DFORCE_REBUILD_LIBBIOSIG=ON to force a
# rebuild of just that one library.
#
# Requirements:
#   libxdf    — CMake, a C++20-capable compiler
#   libbiosig — autoconf, make, a C/C++ compiler (Unix, macOS, or MSYS2 on Windows)
#
# -- Maintainer notes --------------------------------------------------------
# Both libraries are built from source; only the version numbers ever need
# updating when upgrading a dependency.
#
# When upgrading libxdf:
#   1. Update LIBXDF_VERSION below (and in CMakeLists.txt).
#
# When upgrading libbiosig:
#   1. Update LIBBIOSIG_VERSION below (and in CMakeLists.txt).
# ----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

# -- Pinned versions (must match CMakeLists.txt) -----------------------------
set(LIBXDF_VERSION    "0.99.10")
set(LIBBIOSIG_VERSION "3.9.4")

# -- Resolve the destination directory ---------------------------------------
if(NOT DEFINED DEPS_DIR)
    # This script lives in external/, so its parent directory is DEPS_DIR.
    cmake_path(GET CMAKE_CURRENT_LIST_FILE PARENT_PATH DEPS_DIR)
endif()
message(STATUS "Building dependencies into: ${DEPS_DIR}")

# -- Read the installed version stamp (written at the end of this script) ----
set(_versions_file "${DEPS_DIR}/versions.cmake")
if(EXISTS "${_versions_file}")
    include("${_versions_file}")
else()
    set(LIBXDF_VERSION_INSTALLED    "")
    set(LIBBIOSIG_VERSION_INSTALLED "")
endif()

# Per-library force flags override the stamp.
if(FORCE_REBUILD OR FORCE_REBUILD_LIBXDF)
    set(LIBXDF_VERSION_INSTALLED "")
endif()
if(FORCE_REBUILD OR FORCE_REBUILD_LIBBIOSIG)
    set(LIBBIOSIG_VERSION_INSTALLED "")
endif()

# -- Helper: build libxdf from source ----------------------------------------
function(_build_libxdf_from_source version dest_dir)
    set(_src_url     "https://github.com/xdf-modules/libxdf/archive/refs/tags/v${version}.tar.gz")
    set(_src_archive "${dest_dir}/_libxdf-${version}-src.tar.gz")
    set(_tmp_dir     "${dest_dir}/_libxdf-${version}-tmp")
    set(_bld_dir     "${dest_dir}/_libxdf-${version}-build")
    set(_inst_dir    "${dest_dir}/_libxdf-${version}-install")

    message(STATUS "Downloading libxdf ${version} source…")
    file(DOWNLOAD "${_src_url}" "${_src_archive}"
        SHOW_PROGRESS
        STATUS _dl_status
    )
    list(GET _dl_status 0 _dl_code)
    list(GET _dl_status 1 _dl_msg)
    if(NOT _dl_code EQUAL 0)
        file(REMOVE "${_src_archive}")
        message(FATAL_ERROR "Download failed for ${_src_url}:\n  ${_dl_msg}")
    endif()

    message(STATUS "Extracting libxdf source…")
    file(MAKE_DIRECTORY "${_tmp_dir}")
    file(ARCHIVE_EXTRACT INPUT "${_src_archive}" DESTINATION "${_tmp_dir}")
    file(REMOVE "${_src_archive}")

    # GitHub archives extract into a single subdirectory; find it.
    file(GLOB _src_dirs LIST_DIRECTORIES true "${_tmp_dir}/*")
    list(GET _src_dirs 0 _src_dir)

    message(STATUS "Configuring libxdf…")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -S "${_src_dir}"
            -B "${_bld_dir}"
            -DCMAKE_BUILD_TYPE=Release
            -DBUILD_SHARED_LIBS=OFF
            -DXDF_NO_SYSTEM_PUGIXML=ON
            -DCMAKE_INSTALL_PREFIX=${_inst_dir}
        RESULT_VARIABLE _cfg_result
    )
    if(NOT _cfg_result EQUAL 0)
        message(FATAL_ERROR "libxdf configure step failed (exit code ${_cfg_result})")
    endif()

    message(STATUS "Building libxdf…")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" --build "${_bld_dir}" --config Release --parallel
        RESULT_VARIABLE _bld_result
    )
    if(NOT _bld_result EQUAL 0)
        message(FATAL_ERROR "libxdf build step failed (exit code ${_bld_result})")
    endif()

    message(STATUS "Installing libxdf…")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" --install "${_bld_dir}"
        RESULT_VARIABLE _inst_result
    )
    if(NOT _inst_result EQUAL 0)
        message(FATAL_ERROR "libxdf install step failed (exit code ${_inst_result})")
    endif()

    # Copy the header and static library into the flat external/{include,lib} layout.
    file(GLOB _headers "${_inst_dir}/include/xdf.h")
    file(GLOB _libs    "${_inst_dir}/lib/libxdf.a" "${_inst_dir}/lib64/libxdf.a")
    file(COPY ${_headers} DESTINATION "${dest_dir}/include")
    file(COPY ${_libs}    DESTINATION "${dest_dir}/lib")

    # Clean up all temporary directories.
    file(REMOVE_RECURSE "${_tmp_dir}" "${_bld_dir}" "${_inst_dir}")

    message(STATUS "libxdf ${version} built and installed.")
endfunction()

# -- Helper: build libbiosig from source -------------------------------------
function(_build_libbiosig_from_source version dest_dir)
    set(_src_url     "https://sourceforge.net/projects/biosig/files/BioSig%20for%20C_C%2B%2B/src/biosig-${version}.src.tar.xz/download")
    set(_src_archive "${dest_dir}/_libbiosig-${version}-src.tar.xz")
    set(_tmp_dir     "${dest_dir}/_libbiosig-${version}-tmp")

    message(STATUS "Downloading libbiosig ${version} source…")
    file(DOWNLOAD "${_src_url}" "${_src_archive}"
        SHOW_PROGRESS
        STATUS _dl_status
    )
    list(GET _dl_status 0 _dl_code)
    list(GET _dl_status 1 _dl_msg)
    if(NOT _dl_code EQUAL 0)
        file(REMOVE "${_src_archive}")
        message(FATAL_ERROR "Download failed for ${_src_url}:\n  ${_dl_msg}")
    endif()

    message(STATUS "Extracting libbiosig source…")
    file(MAKE_DIRECTORY "${_tmp_dir}")
    file(ARCHIVE_EXTRACT INPUT "${_src_archive}" DESTINATION "${_tmp_dir}")
    file(REMOVE "${_src_archive}")

    # Source tarballs extract into a single subdirectory; find it.
    file(GLOB _src_dirs LIST_DIRECTORIES true "${_tmp_dir}/*")
    list(GET _src_dirs 0 _src_dir)

    find_program(_sh NAMES sh bash REQUIRED)

    message(STATUS "Configuring libbiosig…")
    execute_process(
        COMMAND "${_sh}" "${_src_dir}/configure"
        WORKING_DIRECTORY "${_src_dir}"
        RESULT_VARIABLE _cfg_result
    )
    if(NOT _cfg_result EQUAL 0)
        message(FATAL_ERROR "libbiosig configure step failed (exit code ${_cfg_result})")
    endif()

    find_program(_make_exe NAMES make gmake REQUIRED)

    # win32/getline.c calls getdelim() without including <stdio.h>.  On
    # MinGW-w64, getdelim() is gated behind _POSIX_C_SOURCE >= 200809L, so
    # prepend both the feature-test macro and the include before building.
    if(CMAKE_HOST_WIN32)
        set(_getline_c "${_src_dir}/biosig4c++/win32/getline.c")
        file(READ "${_getline_c}" _getline_content)
        file(WRITE "${_getline_c}" "#define _POSIX_C_SOURCE 200809L\n#include <stdio.h>\n${_getline_content}")
    endif()

    message(STATUS "Building libbiosig…")
    execute_process(
        COMMAND "${_make_exe}" lib
        WORKING_DIRECTORY "${_src_dir}"
        RESULT_VARIABLE _bld_result
    )
    if(NOT _bld_result EQUAL 0)
        message(FATAL_ERROR "libbiosig build step failed (exit code ${_bld_result})")
    endif()

    # Copy libbiosig.a and all public headers into the flat external/{lib,include}
    # layout. make lib builds everything inside the biosig4c++ subdirectory.
    file(COPY "${_src_dir}/biosig4c++/libbiosig.a" DESTINATION "${dest_dir}/lib")
    file(GLOB _biosig_headers "${_src_dir}/biosig4c++/*.h")
    file(COPY ${_biosig_headers} DESTINATION "${dest_dir}/include")

    # Clean up the temporary directory.
    file(REMOVE_RECURSE "${_tmp_dir}")

    message(STATUS "libbiosig ${version} built and installed.")
endfunction()

# -- Build each library only when needed ------------------------------------
file(MAKE_DIRECTORY "${DEPS_DIR}/include" "${DEPS_DIR}/lib")

if(LIBXDF_VERSION_INSTALLED STREQUAL LIBXDF_VERSION)
    message(STATUS "libxdf ${LIBXDF_VERSION} is already up to date — skipping.")
else()
    _build_libxdf_from_source("${LIBXDF_VERSION}" "${DEPS_DIR}")
endif()

if(LIBBIOSIG_VERSION_INSTALLED STREQUAL LIBBIOSIG_VERSION)
    message(STATUS "libbiosig ${LIBBIOSIG_VERSION} is already up to date — skipping.")
else()
    _build_libbiosig_from_source("${LIBBIOSIG_VERSION}" "${DEPS_DIR}")
endif()

# -- Write the version stamp file --------------------------------------------
# This file is read by CMakeLists.txt to verify that the correct versions
# are present before configuring the build.
file(WRITE "${_versions_file}"
    "# Auto-generated by build_deps.cmake — do not edit manually.\n"
    "set(LIBXDF_VERSION_INSTALLED    \"${LIBXDF_VERSION}\")\n"
    "set(LIBBIOSIG_VERSION_INSTALLED \"${LIBBIOSIG_VERSION}\")\n"
)

message(STATUS "")
message(STATUS "Dependencies ready. You can now configure the build:")
message(STATUS "  cmake -S . -B build")
