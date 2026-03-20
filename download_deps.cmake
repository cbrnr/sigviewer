# download_deps.cmake
#
# Downloads the pinned pre-built versions of libxdf and libbiosig, verifies
# their SHA-256 checksums, extracts them into the external/ directory, and
# writes external/versions.cmake so that the main build can confirm the
# correct versions are present.
#
# Usage (from the repository root):
#
#   cmake [-DDEPS_DIR=<path>] -P download_deps.cmake
#
# DEPS_DIR defaults to <repo>/external. Override it if you want the
# artifacts placed elsewhere and will pass the same path via -DDEPS_DIR=
# when configuring the main build.
#
# -- Maintainer notes --------------------------------------------------------
# When upgrading a dependency:
#   1. Build the new pre-built artifacts for every supported platform.
#   2. Upload each archive to a stable URL (e.g. a GitHub release).
#   3. Update the version strings and URL/SHA256 entries below.
#   4. Make the same version change in CMakeLists.txt.
# ----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

# -- Pinned versions (must match CMakeLists.txt) -----------------------------
set(LIBXDF_VERSION    "0.99.10")
set(LIBBIOSIG_VERSION "3.9.4")

# -- Resolve the destination directory ---------------------------------------
if(NOT DEFINED DEPS_DIR)
    # __FILE__ is the path to this script (which lives at the repository root)
    cmake_path(GET CMAKE_CURRENT_LIST_FILE PARENT_PATH _repo_root)
    set(DEPS_DIR "${_repo_root}/external")
endif()
message(STATUS "Installing pre-built dependencies into: ${DEPS_DIR}")
    
# -- Platform + architecture detection ---------------------------------------
if(CMAKE_HOST_APPLE)
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE _host_arch
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(_host_arch STREQUAL "arm64")
        set(_platform "macos-arm64")
    else()
        set(_platform "macos-x86_64")
    endif()
elseif(CMAKE_HOST_WIN32)
    set(_platform "windows-x86_64")
elseif(CMAKE_HOST_UNIX)
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE _host_arch
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(_host_arch STREQUAL "aarch64")
        set(_platform "linux-aarch64")
    else()
        set(_platform "linux-x86_64")
    endif()
else()
    message(FATAL_ERROR "Unsupported host platform. Cannot download pre-built dependencies.")
endif()
message(STATUS "Detected platform: ${_platform}")

# -- Artifact catalogue ------------------------------------------------------
# Each entry is a list: URL SHA256
# TODO: replace the placeholder URLs and hashes with the real ones after
#       building and uploading the pre-built artifacts.
#
# Suggested hosting: create a GitHub release on cbrnr/sigviewer named
# "deps-<date>" and attach archives following the naming convention:
#   libxdf-<version>-<platform>.zip
#   libbiosig-<version>-<platform>.zip
# Each archive should contain:
#   include/   — public headers
#   lib/       — static library (libxdf.a, libbiosig.a)

set(_base_url "https://github.com/cbrnr/sigviewer/releases/download/deps")

# libxdf
set(_LIBXDF_macos-arm64_URL    "${_base_url}/libxdf-${LIBXDF_VERSION}-macos-arm64.zip")
set(_LIBXDF_macos-arm64_SHA256 "TODO_set_sha256_for_libxdf_macos_arm64")

set(_LIBXDF_macos-x86_64_URL    "${_base_url}/libxdf-${LIBXDF_VERSION}-macos-x86_64.zip")
set(_LIBXDF_macos-x86_64_SHA256 "TODO_set_sha256_for_libxdf_macos_x86_64")

set(_LIBXDF_linux-x86_64_URL    "${_base_url}/libxdf-${LIBXDF_VERSION}-linux-x86_64.zip")
set(_LIBXDF_linux-x86_64_SHA256 "TODO_set_sha256_for_libxdf_linux_x86_64")

set(_LIBXDF_linux-aarch64_URL    "${_base_url}/libxdf-${LIBXDF_VERSION}-linux-aarch64.zip")
set(_LIBXDF_linux-aarch64_SHA256 "TODO_set_sha256_for_libxdf_linux_aarch64")

set(_LIBXDF_windows-x86_64_URL    "${_base_url}/libxdf-${LIBXDF_VERSION}-windows-x86_64.zip")
set(_LIBXDF_windows-x86_64_SHA256 "TODO_set_sha256_for_libxdf_windows_x86_64")

# libbiosig
set(_LIBBIOSIG_macos-arm64_URL    "${_base_url}/libbiosig-${LIBBIOSIG_VERSION}-macos-arm64.zip")
set(_LIBBIOSIG_macos-arm64_SHA256 "TODO_set_sha256_for_libbiosig_macos_arm64")

set(_LIBBIOSIG_macos-x86_64_URL    "${_base_url}/libbiosig-${LIBBIOSIG_VERSION}-macos-x86_64.zip")
set(_LIBBIOSIG_macos-x86_64_SHA256 "TODO_set_sha256_for_libbiosig_macos_x86_64")

set(_LIBBIOSIG_linux-x86_64_URL    "${_base_url}/libbiosig-${LIBBIOSIG_VERSION}-linux-x86_64.zip")
set(_LIBBIOSIG_linux-x86_64_SHA256 "TODO_set_sha256_for_libbiosig_linux_x86_64")

set(_LIBBIOSIG_linux-aarch64_URL    "${_base_url}/libbiosig-${LIBBIOSIG_VERSION}-linux-aarch64.zip")
set(_LIBBIOSIG_linux-aarch64_SHA256 "TODO_set_sha256_for_libbiosig_linux_aarch64")

set(_LIBBIOSIG_windows-x86_64_URL    "${_base_url}/libbiosig-${LIBBIOSIG_VERSION}-windows-x86_64.zip")
set(_LIBBIOSIG_windows-x86_64_SHA256 "TODO_set_sha256_for_libbiosig_windows_x86_64")

# -- Helper: download, verify, and extract one archive -----------------------
function(_fetch_dep lib_upper_name version platform dest_dir)
    set(_url    "${_${lib_upper_name}_${platform}_URL}")
    set(_sha256 "${_${lib_upper_name}_${platform}_SHA256}")

    if(_sha256 MATCHES "^TODO_")
        message(FATAL_ERROR
            "No pre-built artifact configured for ${lib_upper_name} on ${platform}.\n"
            "See download_deps.cmake for instructions on adding one.")
    endif()

    message(STATUS "Downloading ${lib_upper_name} ${version} for ${platform}…")

    set(_archive "${dest_dir}/${lib_upper_name}-${version}-${platform}.zip")
    file(DOWNLOAD "${_url}" "${_archive}"
        EXPECTED_HASH SHA256=${_sha256}
        SHOW_PROGRESS
        STATUS _dl_status
    )
    list(GET _dl_status 0 _dl_code)
    list(GET _dl_status 1 _dl_msg)
    if(NOT _dl_code EQUAL 0)
        file(REMOVE "${_archive}")
        message(FATAL_ERROR "Download failed for ${_url}:\n  ${_dl_msg}")
    endif()

    message(STATUS "Extracting ${lib_upper_name}…")
    set(_extract_dir "${dest_dir}/_tmp_${lib_upper_name}")
    file(ARCHIVE_EXTRACT INPUT "${_archive}" DESTINATION "${_extract_dir}")
    file(REMOVE "${_archive}")

    # Copy headers and library into the flat external/{include,lib} layout.
    file(GLOB _headers "${_extract_dir}/include/*.h")
    file(GLOB _libs    "${_extract_dir}/lib/*")
    file(COPY ${_headers} DESTINATION "${dest_dir}/include")
    file(COPY ${_libs}    DESTINATION "${dest_dir}/lib")
    file(REMOVE_RECURSE "${_extract_dir}")

    message(STATUS "${lib_upper_name} ${version} installed.")
endfunction()

# -- Download both libraries -------------------------------------------------
file(MAKE_DIRECTORY "${DEPS_DIR}/include" "${DEPS_DIR}/lib")

_fetch_dep(LIBXDF    "${LIBXDF_VERSION}"    "${_platform}" "${DEPS_DIR}")
_fetch_dep(LIBBIOSIG "${LIBBIOSIG_VERSION}" "${_platform}" "${DEPS_DIR}")

# -- Write the version stamp file --------------------------------------------
# This file is read by CMakeLists.txt to verify that the correct versions
# are present before configuring the build.
set(_versions_file "${DEPS_DIR}/versions.cmake")
file(WRITE "${_versions_file}"
    "# Auto-generated by download_deps.cmake — do not edit manually.\n"
    "set(LIBXDF_VERSION_INSTALLED    \"${LIBXDF_VERSION}\")\n"
    "set(LIBBIOSIG_VERSION_INSTALLED \"${LIBBIOSIG_VERSION}\")\n"
)

message(STATUS "")
message(STATUS "Dependencies ready. You can now configure the build:")
message(STATUS "  cmake -S . -B build")
