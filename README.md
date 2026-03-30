# SigViewer

SigViewer is an application for viewing biosignals such as EEG or MEG time series. In addition to viewing raw data, SigViewer can also create, edit, and display events (such as annotations or artifact selections).

- [SigViewer 0.6.5 (Windows)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.5/sigviewer-0.6.5-windows-x86_64.exe)
- [SigViewer 0.6.5 (macOS)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.5/sigviewer-0.6.5-macos-arm64.dmg)
- [SigViewer 0.6.5 (Linux x86-64)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.5/sigviewer-0.6.5-linux-x86_64.tar.gz)
- [SigViewer 0.6.5 (Linux AARCH64)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.5/sigviewer-0.6.5-linux-aarch64.tar.gz)
- [SigViewer 0.6.5 (Source)](https://github.com/cbrnr/sigviewer/archive/v0.6.5.zip)


## Building SigViewer

SigViewer requires a standard-compliant C++17 build toolchain, for example recent versions of [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/). Furthermore, SigViewer depends on [Qt 6](https://www.qt.io/) and [CMake](https://cmake.org/) 3.21 or later.

In addition, SigViewer depends on [libbiosig](http://biosig.sourceforge.net/) and [libxdf](https://github.com/xdf-modules/libxdf), which are built from source automatically.


### macOS

Install the Xcode Command Line Tools by running `xcode-select --install`, then install Qt 6 and CMake via [Homebrew](https://brew.sh/):

```
brew install qt cmake
```

Then build SigViewer:

```
cmake -P external/build_deps.cmake
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.logicalcpu)
```

The application bundle is produced at `build/SigViewer.app`.

To create a disk image that can be distributed, first bundle all required Qt frameworks into the app:

```
$(brew --prefix qt)/bin/macdeployqt build/SigViewer.app
```

For a production release the app bundle must be **code-signed** with a Developer ID certificate, **notarized** with Apple, and **stapled** before the DMG is created. Distributing an unsigned or un-notarized app will trigger Gatekeeper warnings. The order is:

1. Sign the `.app` with `codesign`
2. Submit the `.app` (as a zip) to `xcrun notarytool` and wait for approval
3. Staple the notarization ticket to the `.app` with `xcrun stapler`
4. Create the DMG from the stapled `.app`

See `.github/workflows/release.yml` for the exact commands used in CI.

Once the app is signed, notarized, and stapled, create the disk image:

```
hdiutil create -volname "SigViewer" -srcfolder build/SigViewer.app -ov -format UDZO build/SigViewer-<version>.dmg
```


### Linux

Install a C++ toolchain, Qt 6, and CMake with your package manager. On Debian/Ubuntu:

```
sudo apt install cmake build-essential qt6-base-dev qt6-tools-dev
```

Then build SigViewer:

```
cmake -P external/build_deps.cmake
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

The executable is produced at `build/sigviewer`.


### Windows

Install [MSYS2](https://www.msys2.org/) and open the MINGW64 shell. Install the required dependencies:

```
pacman -S --needed \
    mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-ninja \
    mingw-w64-x86_64-qt6-base \
    mingw-w64-x86_64-qt6-tools \
    mingw-w64-x86_64-libiconv \
    autoconf \
    automake \
    make
```

Then build SigViewer from the MINGW64 shell:

```
cmake -P external/build_deps.cmake
cmake -B build -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build
```

The executable is produced at `build/sigviewer.exe`.

Collect the executable together with all required Qt DLLs and MinGW runtime libraries into a staging folder:

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

Then build the installer using [Inno Setup](https://jrsoftware.org/isinfo.php) (available from `winget install JRSoftware.InnoSetup`):

```
& 'C:\Program Files (x86)\Inno Setup 6\ISCC.exe' /Dversion=<version> deploy\windows\sigviewer-windows.iss
```

The installer is written to `build/SigViewer-<version>.exe`.


## Testing

The test suite uses [Qt Test](https://doc.qt.io/qt-6/qttest-index.html) and [CTest](https://cmake.org/cmake/help/latest/manual/ctest.1.html). After building, run all tests from the `build/` directory:

```
ctest --test-dir build
```

For verbose output:

```
ctest --test-dir build -V
```

To run a single test by name (e.g. just the data block tests):

```
ctest --test-dir build -R tst_data_block
```

The test executables (`tst_data_block`, `tst_color_manager`, `tst_event_manager`, `tst_editing_commands`, `tst_event_table_widget`, `tst_file_handling`, `tst_gui`) can also be run directly — CTest sets `QT_QPA_PLATFORM=offscreen` automatically so no real display is required.


## Creating a release

To publish a new release:

1. Update the `VERSION` field in `CMakeLists.txt` to the new version number.
2. Commit the change:
   ```
   git commit -am "Release <version>"
   ```
3. Tag the commit and push both the commit and the tag:
   ```
   git tag v<version>
   git push origin main --tags
   ```

Pushing the tag triggers the `.github/workflows/release.yml` workflow, which builds SigViewer on all three platforms (Linux x86-64, Linux ARM64, macOS ARM64, Windows x86-64), packages the release artifacts, and publishes a new GitHub release with the following assets attached:

- `sigviewer-<version>-linux-x86_64.tar.gz`
- `sigviewer-<version>-linux-aarch64.tar.gz`
- `sigviewer-<version>-macos-arm64.dmg`
- `sigviewer-<version>-windows-x86_64.exe`

Release notes are generated automatically from the merged pull requests and commits since the previous tag.


## Updating translations

SigViewer's UI strings are kept in `.ts` files under `src/translations/`. After adding or changing translatable strings in the source code, update the `.ts` files by running:

```
cmake --build build --target sigviewer_lupdate
```

The resulting `.ts` files should be committed to the repository. The `.qm` binary files are compiled automatically from them during the regular build.


## Updating dependencies

The script `external/build_deps.cmake` downloads the pinned source releases, builds them, and installs the resulting static libraries into the `external/` directory. You only need to run this once (or again after changing the pinned versions).

The pinned versions are defined in two places that must always be kept in sync:

- `CMakeLists.txt` (`LIBXDF_VERSION` and `LIBBIOSIG_VERSION` near the top of the file)
- `external/build_deps.cmake` (the same two variables near the top of that file)

To upgrade a dependency, update both of those variables to the new version and re-run `cmake -P external/build_deps.cmake`. Pass `-DFORCE_REBUILD=ON` to force a full rebuild even if the stamp file `external/versions.cmake` already records the same version, or use `-DFORCE_REBUILD_LIBXDF=ON` / `-DFORCE_REBUILD_LIBBIOSIG=ON` to rebuild only one of the two libraries.
