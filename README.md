SigViewer
=========

SigViewer is a viewing application for biosignals such as EEG or MEG time series. In addition to viewing raw data, SigViewer can also create, edit, and display event information (such as annotations or artifact selections).

Download
--------
- [SigViewer 0.6.4 (Windows)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/sigviewer-0.6.4-win64.exe)
- [SigViewer 0.6.4 (macOS)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/sigviewer-0.6.4-macos.dmg)
- [SigViewer 0.6.4 (Linux)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/sigviewer-0.6.4-linux.zip) ([Arch](https://aur.archlinux.org/packages/sigviewer/), [Debian](https://tracker.debian.org/pkg/sigviewer), [Ubuntu](https://launchpad.net/ubuntu/+source/sigviewer))
- [SigViewer 0.6.4 (Source)](https://github.com/cbrnr/sigviewer/archive/v0.6.4.zip)

Screenshots
-----------
![screenshot-1](https://github.com/cbrnr/sigviewer/raw/master/screenshot-1.png)

Building SigViewer
------------------
SigViewer requires a standard-compliant C++11 build toolchain, for example recent versions of [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/). Other compilers such as [MSVC](https://en.wikipedia.org/wiki/Visual_C%2B%2B) might work, but are not tested. Furthermore, SigViewer depends on [Qt](https://www.qt.io/). Current SigViewer builds use Qt 5.12 (previous or future versions are not guaranteed to work).

SigViewer also depends on [libbiosig](http://biosig.sourceforge.net/) and [libxdf](https://github.com/xdf-modules/libxdf). There are two options to get these external dependencies for your platform:

1. Build these dependencies yourself (see separate descriptions below).
2. Use our pre-built binaries. The corresponding archive contains binary versions of libbiosig and libxdf and must be extracted into SigViewer’s source folder (which we denote as `$sigviewer`).
    - [External dependencies (macOS)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/external-0.6.4-macos.zip)
    - [External dependencies (Linux)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/external-0.6.4-linux.zip)

### Windows
Building SigViewer on Windows is currently not supported. We provide binaries created with the [MXE](https://mxe.cc/) cross compilation environment.


### macOS
SigViewer requires Mac OS X (now renamed to macOS) 10.9 or later. First, install XCode from the App Store. Next, download and install [Qt 5.12.3 for macOS](http://download.qt.io/official_releases/qt/5.12/5.12.3/qt-opensource-mac-x64-5.12.3.dmg) or install Qt via [Homebrew](https://brew.sh/) by running `brew install qt` in a terminal. Make sure that `qmake` is available on the path if you want to build SigViewer in a terminal. Alternatively, you can build SigViewer with Qt Creator (please refer to the description for building SigViewer on Windows).

1. Download and unzip the [SigViewer source](https://github.com/cbrnr/sigviewer/archive/v0.6.4.zip).
1. Provide all external dependencies:
    - Either download the [external archive](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/external-0.6.4-macos.zip) and extract it inside `$sigviewer`.
    - Or copy the necessary files from libbiosig and libxdf builds to the corresponding folders as detailed in the build descriptions for libbiosig and libxdf below.
1. In a terminal, change to `$sigviewer` and run `qmake`.
1. Run `make` (or if you want to use more cores to build in parallel, run `make -j4` if you want to use four cores). The SigViewer binary is built in the `bin/release` folder.
1. To create a stand-alone version of SigViewer, open a terminal, change into `$sigviewer/bin/release` and run `macdeployqt sigviewer.app -dmg`. This creates a disk image with the app, which can then be dragged to the Applications folder.

### Linux
Install the GNU build toolchain and Qt 5 with your native package manager. You can either build on the command line or with Qt Creator (which you then need to install).

1. Download and unzip the [SigViewer source](https://github.com/cbrnr/sigviewer/archive/v0.6.4.zip).
1. Provide all external dependencies:
    - Either download the [external archive](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/external-0.6.4-linux.zip) and extract it inside `$sigviewer`.
    - Or copy the necessary files from libbiosig and libxdf builds to the corresponding folders as detailed in the build descriptions for libbiosig and libxdf below.
1. In a terminal, change to `$sigviewer` and run `qmake`.
1. Run `make` (or if you want to use more cores to build in parallel, run `make -j 4` if you want to use four cores). The SigViewer binary is built in the `bin/release` folder.

Building external dependencies
------------------------------
### Windows
Building libbiosig on Windows is currently not supported.

To build libxdf from source, follow these steps:

1. Download and unzip the [libxdf source](https://github.com/xdf-modules/libxdf/archive/v0.99.zip) (SigViewer 0.6.4 uses libxdf 0.99).
1. On the command line, run `qmake` followed by `mingw32-make` (or build the project with Qt Creator) (instead of `qmake`, you can also run `cmake .`).
2. Copy `xdf.h` into `$sigviewer/external/include` and `libxdf.a` to `$sigviewer/external/lib`.

### macOS
To build libbiosig from source, follow these steps:

1. Download and unzip [BioSig for C/C++](https://sourceforge.net/projects/biosig/files/BioSig%20for%20C_C%2B%2B/src/biosig4c%2B%2B-1.9.4.src.tar.gz) into `$libbiosig` (SigViewer 0.6.4 uses libbiosig 1.9.4).
1. Change line 156 of `Makefile.in` to `CFLAGS        += -mmacosx-version-min=10.9` (replace `10.13` with `10.9`).
1. In a terminal, change to `$libbiosig` and run `./configure`.
1. Run `make`.
1. Copy `biosig.h`, `biosig-dev.h`, `gdftime.h`, and `physicalunits.h` to `$sigviewer/external/include` and `libbiosig.a` to `$sigviewer/external/lib`.

To build libxdf from source, follow these steps:

1. Download and unzip the [libxdf source](https://github.com/xdf-modules/libxdf/archive/v0.99.zip) to `$libxdf` (SigViewer 0.6.4 uses libxdf 0.99).
1. In a terminal, change to `$libxdf` and run `qmake` followed by `make` (instead of `qmake`, you can also run `cmake .`).
1. Copy `xdf.h` into `$sigviewer/external/include` and `libxdf.a` to `$sigviewer/external/lib`.

### Linux
To build libbiosig from source, follow these steps:

1. Download and unzip [BioSig for C/C++](https://sourceforge.net/projects/biosig/files/BioSig%20for%20C_C%2B%2B/src/biosig4c%2B%2B-1.9.4.src.tar.gz) into `$libbiosig` (SigViewer 0.6.4 uses libbiosig 1.9.4).
1. In a terminal, change to `$libbiosig` and run `./configure`.
1. Run `make`.
1. Copy `biosig.h`, `biosig-dev.h`, `gdftime.h`, and `physicalunits.h` to `$sigviewer/external/include` and `libbiosig.a` to `$sigviewer/external/lib`.

To build libxdf from source, follow these steps:

1. Download and unzip the [libxdf source](https://github.com/xdf-modules/libxdf/archive/v0.99.zip) to `$libxdf` (SigViewer 0.6.4 uses libxdf 0.99).
1. In a terminal, change to `$libxdf` and run `qmake` followed by `make` (instead of `qmake`, you can also run `cmake .`).
1. Copy `xdf.h` into `$sigviewer/external/include` and `libxdf.a` to `$sigviewer/external/lib`.
