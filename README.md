# SigViewer

SigViewer is an application for viewing biosignals such as EEG or MEG time series. In addition to viewing raw data, SigViewer can also create, edit, and display event information (such as annotations or artifact selections).

- [SigViewer 0.6.4 (Windows)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/sigviewer-0.6.4-win64.exe)
- [SigViewer 0.6.4 (macOS)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/sigviewer-0.6.4-macos.dmg)
- [SigViewer 0.6.4 (Linux)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/sigviewer-0.6.4-linux.zip) ([Arch](https://aur.archlinux.org/packages/sigviewer/), [Debian](https://tracker.debian.org/pkg/sigviewer), [Ubuntu](https://launchpad.net/ubuntu/+source/sigviewer))
- [SigViewer 0.6.4 (Source)](https://github.com/cbrnr/sigviewer/archive/v0.6.4.zip)


## Building SigViewer

SigViewer requires a standard-compliant C++17 build toolchain, for example recent versions of [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/). Furthermore, SigViewer depends on [Qt 6](https://www.qt.io/) and [CMake](https://cmake.org/) 3.21 or later.

SigViewer also depends on [libbiosig](http://biosig.sourceforge.net/) and [libxdf](https://github.com/xdf-modules/libxdf), which are built from source automatically by running:

```
cmake -P external/build_deps.cmake
```

This downloads the source releases, builds them, and installs the resulting libraries into the `external/` directory. You only need to run this once (or again when the pinned versions change).


### macOS

Install the Xcode Command Line Tools by running `xcode-select --install`, then install Qt 6 and CMake via [Homebrew](https://brew.sh/):

```
brew install qt cmake
```

Then build SigViewer:

```
cmake -P external/build_deps.cmake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.logicalcpu)
```

The application bundle is produced at `build/sigviewer.app`. To create a stand-alone disk image, run:

```
macdeployqt build/sigviewer.app -dmg
```


### Linux

Install a C++ toolchain, Qt 6, and CMake with your package manager. Then build SigViewer:

```
cmake -P external/build_deps.cmake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```


### Windows

Building on Windows is currently not supported (coming soon).
