# Contributing to SigViewer

If you would like to implement a new feature, fix an existing bug, or help improve SigViewer in another way, such as improving its documentation, please consider submitting a [pull request](https://github.com/cbrnr/sigviewer/pulls). It can be helpful to open an [issue](https://github.com/cbrnr/sigviewer/issues) first to discuss a planned contribution with the maintainers.

Before you start working on a contribution, please follow the guidelines in this document.

## Setting up the development environment

SigViewer requires a C++17-compliant build toolchain (such as a recent version of [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/)), [Qt 6](https://www.qt.io/), and [CMake](https://cmake.org/) 3.21 or later. You will also need [Git](https://git-scm.com/).

SigViewer builds [libbiosig](http://biosig.sourceforge.net/) and [libxdf](https://github.com/xdf-modules/libxdf) from source and statically links them by default. The build commands below install the pinned versions into `external/`.

### macOS

Install the Xcode Command Line Tools:

```
xcode-select --install
```

Then install Qt 6 and CMake with [Homebrew](https://brew.sh/):

```
brew install qt cmake
```

### Linux

Install a C++ toolchain, Qt 6, and CMake with your package manager. On Arch Linux:

```
sudo pacman -S --needed base-devel cmake qt6-base qt6-tools qt6-svg
```

On Debian or Ubuntu:

```
sudo apt install cmake build-essential qt6-base-dev qt6-tools-dev libqt6svg6-dev
```

### Windows

Install [MSYS2](https://www.msys2.org/), open the MINGW64 shell, and install the required dependencies:

```
pacman -S --needed \
    mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-ninja \
    mingw-w64-x86_64-qt6-base \
    mingw-w64-x86_64-qt6-tools \
    mingw-w64-x86_64-qt6-svg \
    mingw-w64-x86_64-libiconv \
    autoconf \
    automake \
    make
```

## Forking and cloning SigViewer

On the [repository website](https://github.com/cbrnr/sigviewer), click **Fork** to create your own copy of SigViewer. Clone that fork and change into the project directory:

```
git clone <your-fork-url>
cd sigviewer
```

## Building the project

First, build the pinned dependencies. This command is the same on every platform:

```
cmake -P external/build_deps.cmake
```

Then configure and build SigViewer for your platform.

### macOS

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.logicalcpu)
```

The app bundle is produced at `build/SigViewer.app`.

### Linux

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

The executable is produced at `build/sigviewer`.

### Windows

Run these commands from the MINGW64 shell:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build
```

The executable is produced at `build/sigviewer.exe`.

### Linking against system dependencies

The CMake option `SIGVIEWER_SYSTEM_DEPS` controls how libbiosig and libxdf are obtained:

- `OFF` (default) statically links the pinned versions built from source. Release builds use this configuration.
- `ON` requires system packages instead; configuration fails if either is unavailable. Ensure that the installed versions satisfy SigViewer's documented minimums.

System packages are located through `find_package(libxdf ${LIBXDF_VERSION} CONFIG)` and the bundled `cmake/FindLibBiosig.cmake` module. libxdf's version is enforced automatically. libbiosig has no reliable version check, so make sure it satisfies `LIBBIOSIG_VERSION` in `CMakeLists.txt`.

## Testing

The test suite uses [Qt Test](https://doc.qt.io/qt-6/qttest-index.html) and [CTest](https://cmake.org/cmake/help/latest/manual/ctest.1.html). After building, run all tests with:

```
ctest --test-dir build
```

For verbose output, use `ctest --test-dir build -V`. To run a single test, for example the data block tests:

```
ctest --test-dir build -R test_data_block
```

The test executables can also be run directly. CTest sets `QT_QPA_PLATFORM=offscreen` automatically, so a display is not required.

## Creating a new branch

Before modifying the codebase, create a branch for your contribution:

```
git switch -c <branch-name>
```

## Making a pull request

Commit your changes and push the branch to your fork. GitHub will then offer to create a pull request. Choose a clear title and describe the contribution; reference a related issue in the description if there is one.

### Adding a changelog entry

For user-facing changes, add an entry to the appropriate section at the top of [`CHANGELOG.md`](CHANGELOG.md). Use **Added** for new features, **Fixed** for bug fixes, and **Changed** for modifications to existing functionality. Follow the existing entry format to reference the pull request and, where appropriate, its author.

### Updating translations

UI strings are kept in `.ts` files under `src/translations/`. After adding or changing a translatable string, update those files with:

```
cmake --build build --target sigviewer_lupdate
```

Commit the resulting `.ts` files. The `.qm` binaries are compiled automatically during the regular build.

## Updating dependencies

`external/build_deps.cmake` downloads the pinned source releases, builds them, and installs the static libraries into `external/`. Run it again after changing a pinned version.

The pinned versions must remain in sync in both of these files:

- `CMakeLists.txt` (`LIBXDF_VERSION` and `LIBBIOSIG_VERSION` near the top)
- `external/build_deps.cmake` (the same two variables near the top)

After updating both values, run `cmake -P external/build_deps.cmake`. Pass `-DFORCE_REBUILD=ON` to rebuild both dependencies even when `external/versions.cmake` already records the version, or use `-DFORCE_REBUILD_LIBXDF=ON` or `-DFORCE_REBUILD_LIBBIOSIG=ON` to rebuild only one.

## Creating a release

Creating a release requires write access to the repository and its GitHub releases.

1. Update `VERSION` in `CMakeLists.txt`.
2. Commit the change:

   ```
   git commit -am "Release <version>"
   ```

3. Tag the commit and push the commit and tag:

   ```
   git tag v<version>
   git push origin main --tags
   ```

Pushing the tag triggers `.github/workflows/release.yml`, which builds, packages, and publishes the release artifacts for Linux x86-64, Linux ARM64, macOS ARM64, and Windows x86-64. The release contains:

- `sigviewer-<version>-linux-x86_64.tar.gz`
- `sigviewer-<version>-linux-aarch64.tar.gz`
- `sigviewer-<version>-macos-arm64.dmg`
- `sigviewer-<version>-windows-x86_64.exe`

Release notes are generated automatically from pull requests and commits since the previous tag.

Finally, update the versioned download links in `README.md`.

### Packaging macOS builds

To create a distributable macOS disk image, first bundle the required Qt frameworks:

```
$(brew --prefix qt)/bin/macdeployqt build/SigViewer.app
```

For a production release, code-sign the app with a Developer ID certificate, notarize it with Apple, and staple the resulting ticket before creating the disk image. Distributing an unsigned or unnotarized app triggers Gatekeeper warnings. The required order is:

1. Sign the `.app` with `codesign`.
2. Submit the app, packaged as a zip, to `xcrun notarytool` and wait for approval.
3. Staple the notarization ticket to the `.app` with `xcrun stapler`.
4. Create the disk image from the stapled app.

See [`.github/workflows/release.yml`](.github/workflows/release.yml) for the commands used in CI.

Then create the disk image:

```
hdiutil create -volname "SigViewer" -srcfolder build/SigViewer.app -ov -format UDZO build/SigViewer-<version>.dmg
```

### Packaging Windows builds

Collect the executable, the application icon, Qt DLLs, and remaining MinGW runtime dependencies into a staging folder:

```
mkdir -p build/sigviewer-windows
cp build/sigviewer.exe build/sigviewer-windows/
cp src/images/sigviewer.ico build/sigviewer-windows/
windeployqt6 --release build/sigviewer-windows/sigviewer.exe
# Copy any remaining MinGW DLL dependencies
find build/sigviewer-windows \( -name '*.exe' -o -name '*.dll' \) | xargs ldd 2>/dev/null \
    | awk '$3 ~ /\/mingw64\//' | awk '{print $3}' | sort -u \
    | while IFS= read -r dep; do
        dest="build/sigviewer-windows/$(basename "$dep")"
        [ -f "$dest" ] || cp "$dep" "$dest"
      done
```

Then build the installer using [Inno Setup](https://jrsoftware.org/isinfo.php), available for example through `winget install JRSoftware.InnoSetup`:

```
& 'C:\\Program Files (x86)\\Inno Setup 6\\ISCC.exe' /Dversion=<version> deploy\\windows\\sigviewer-windows.iss
```

The installer is written to `build/SigViewer-<version>.exe`.
