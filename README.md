SigViewer
=========

SigViewer is a viewing application for biosignals such as EEG or MEG time series. In addition to viewing raw data, SigViewer can also create, edit, and display event information (such as annotations or artifact selections).

Download
--------
- [SigViewer 0.6.0 (Windows)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.0/sigviewer-0.6.0-windows.exe) (Windows 7-10, 32bit)
- [SigViewer 0.6.0 (macOS)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.0/sigviewer-0.6.0-macos.dmg) (macOS 10.9-10.12, 64bit)
- [SigViewer 0.6.0 (Linux)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.0/sigviewer-0.6.0-linux.zip) (64bit) / [SigViewer 0.6.0 (Arch Linux AUR)](https://aur.archlinux.org/packages/sigviewer/)
- [SigViewer 0.6.0 (Source)](https://github.com/cbrnr/sigviewer/archive/v0.6.0.zip)

Screenshots
-----------
![screenshot-1](https://github.com/cbrnr/sigviewer/raw/master/screenshot-1.png)

Building SigViewer
------------------
SigViewer requires a standard-compliant C++11 build toolchain, for example recent versions of [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/). Other compilers such as [MSVC](https://en.wikipedia.org/wiki/Visual_C%2B%2B) might work, but are not tested. Furthermore, SigViewer depends on [Qt](https://www.qt.io/). Current SigViewer builds use Qt 5.8 (previous or future versions are not guaranteed to work).

SigViewer depends on [libbiosig](http://biosig.sourceforge.net/) and [libxdf](https://github.com/Yida-Lin/libxdf). There are two options to get these external dependencies for your platform:

1. Build these dependencies yourself (see separate descriptions below).
2. Use our pre-built binaries. The corresponding archive contains binary versions of libbiosig and libxdf and must be extracted into SigViewer’s source folder (which we denote as `$sigviewer`).
    - [External dependencies (Windows)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.0/external-0.6.0-windows.zip) (Windows 7-10, 32bit)
    - [External dependencies (macOS)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.0/external-0.6.0-macos.zip) (macOS 10.9-10.12, 64bit)
    - [External dependencies (Linux)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.0/external-0.6.0-linux.zip) (64bit)

Note that if you are on Windows, you cannot build libbiosig yourself, and therefore you have to use our pre-built binary.

### Windows
SigViewer requires Windows 7 or Windows 10. Other versions might work, but have not been tested. First, download the offline installer for [Qt 5.8.0 for Windows 32-bit (MinGW 5.3.0)](http://download.qt.io/official_releases/qt/5.8/5.8.0/qt-opensource-windows-x86-mingw530-5.8.0.exe). Run the installation wizard and make sure to also select MinGW 5.3 in the Tools group. Once the installation is completed, a new folder _Qt 5.8.0_ is added to the Start menu. It contains the command prompt _Qt 5.8 for Desktop_, which has all required build tools (`qmake` and `mingw32-make`) added to its path. Make sure you use this command prompt if you want to build on the command line. Alternatively, you can build SigViewer with Qt Creator, which is installed along with Qt 5.8 by default. Here, we describe the build process using Qt Creator.

1. Download and unzip the [SigViewer source](https://github.com/cbrnr/sigviewer/archive/v0.6.0.zip).
1. Provide all external dependencies by downloading the [external archive](https://github.com/cbrnr/sigviewer/releases/download/v0.6.0/external-0.6.0-windows.zip) and extracting it inside `$sigviewer` (see below for more details and options regarding the external dependencies).
1. Open `sigviewer.pro` in Qt Creator .
1. Accept the default configuration by clicking on _Configure Project_.
1. Click on _Build Project_ (the hammer icon) to build SigViewer.
1. Click on _Run_ (the play icon) to start SigViewer.
1. To create a stand-alone version of SigViewer, open a command prompt, change into `$sigviewer/bin/release`, and run `windeployqt sigviewer.exe`. SigViewer now runs on any Windows machine (no previous Qt installation is required) if the contents of this directory is distributed together with `sigviewer.exe`.


### macOS
Mac OS X (now renamed to macOS) 10.9 or later is required. First, install XCode from the App Store. Next, download the offline installer for [Qt 5.8.0 for macOS](http://download.qt.io/official_releases/qt/5.8/5.8.0/qt-opensource-mac-x64-clang-5.8.0.dmg). Run the installation wizard. Make sure that `qmake` is available on the path if you want to build SigViewer on the command line. Alternatively, you can build SigViewer with Qt Creator, which is installed along with Qt 5.8 by default. Here, we describe the build process using the command line, but if you want to use Qt Creator instead refer to the description for building SigViewer on Windows.

1. Download and unzip the [SigViewer source](https://github.com/cbrnr/sigviewer/archive/v0.6.0.zip).
1. Provide all external dependencies:
    - Either download the [external archive](https://github.com/cbrnr/sigviewer/releases/download/v0.6.0/external-0.6.0-macos.zip) and extract it inside `$sigviewer`.
    - Or copy the necessary files from libbiosig and libxdf builds to the corresponding folders as detailed in the build descriptions for libbiosig and libxdf below.
1. In a terminal, run `qmake`.
1. Run `make` (or if you want to use more cores to build in parallel, run `make -j 4` if you want to use four cores). The SigViewer binary is built in the `bin/release` folder.
1. To create a stand-alone version of SigViewer, open a terminal, change into `$sigviewer/bin/release` and run `macdeployqt sigviewer.app -dmg`. This creates a disk image with the app, which can then be dragged to the Applications folder.

### Linux
Install the GNU toolchain and Qt 5 with your native package manager. You can either build on the command line or with Qt Creator (which you then need to install).

1. Download and unzip the [SigViewer source](https://github.com/cbrnr/sigviewer/archive/v0.6.0.zip).
1. Provide all external dependencies:
    - Either download the [external archive](https://github.com/cbrnr/sigviewer/releases/download/v0.6.0/external-0.6.0-linux.zip) and extract it inside `$sigviewer`.
    - Or copy the necessary files from libbiosig and libxdf builds to the corresponding folders as detailed in the build descriptions for libbiosig and libxdf below.
1. In a terminal, run `qmake`.
1. Run `make` (or if you want to use more cores to build in parallel, run `make -j 4` if you want to use four cores). The SigViewer binary is built in the `bin/release` folder.
1. You can directly start the executable in a terminal. If you use Arch Linux or Ubuntu/Debian Linux, we provide native packages for these distributions.

Building external dependencies
------------------------------
### Windows
Building libbiosig on Windows is currently not possible. Please use our pre-built binary included in the [external archive](https://github.com/cbrnr/sigviewer/releases/download/v0.6.0/external-0.6.0-windows.zip).

To build libxdf from source, follow these steps:

1. Download and unzip the [libxdf source](https://github.com/Yida-Lin/libxdf/archive/v0.92.zip) (SigViewer 0.6.0 uses libxdf 0.92).
1. On the command line, run `qmake` followed by `mingw32-make` (or build the project with Qt Creator).
2. Copy `xdf.h` into `$sigviewer/external/include` and `libbiosig.a` to `$sigviewer/external/lib`.

### macOS
To build libbiosig from source, follow these steps:

1. Building libbiosig requires `gawk`. The easiest way to install it is via [Homebrew](http://brew.sh/) (`brew install gawk`).
1. Download and unzip [BioSig for C/C++](https://sourceforge.net/projects/biosig/files/BioSig%20for%20C_C%2B%2B/src/biosig4c%2B%2B-1.8.4b.src.tar.gz) (SigViewer 0.6.0 uses libbiosig 1.8.4b).
1. Change the following lines in `Makefile`:
    - Change line 199 to: `# DEFINES += -D=WITH_ZLIB`
    - Change line 207 to: `# DEFINES += -D=WITH_CHOLMOD`
    - Change `10.7` in lines 148 and 151 to `10.9`
1. In a terminal, run `make libbiosig.a`.
1. Copy `biosig.h` and `gdftime.h` to `$sigviewer/external/include` and `libbiosig.a` to `$sigviewer/external/lib`.

To build libxdf from source, follow these steps:

1. Download and unzip the [libxdf source](https://github.com/Yida-Lin/libxdf/archive/v0.92.zip) (SigViewer 0.6.0 uses libxdf 0.92).
1. In a terminal, run `qmake` followed by `make`.
1. Copy `xdf.h` into `$sigviewer/external/include` and `libbiosig.a` to `$sigviewer/external/lib`.

### Linux
To build libbiosig from source, follow these steps:

1. Download and unzip [BioSig for C/C++](https://sourceforge.net/projects/biosig/files/BioSig%20for%20C_C%2B%2B/src/biosig4c%2B%2B-1.8.4b.src.tar.gz) (SigViewer 0.6.0 uses libbiosig 1.8.4b).
1. Change the following lines in `Makefile`:
    - Change line 199 to: `# DEFINES += -D=WITH_ZLIB`
    - Change line 207 to: `# DEFINES += -D=WITH_CHOLMOD`
1. In a terminal, run `make libbiosig.a`.
1. Copy `biosig.h` and `gdftime.h` to `$sigviewer/external/include` and `libbiosig.a` to `$sigviewer/external/lib`.

To build libxdf from source, follow these steps:

1. Download and unzip the [libxdf source](https://github.com/Yida-Lin/libxdf/archive/v0.92.zip) (SigViewer 0.6.0 uses libxdf 0.92).
1. In a terminal, run `qmake` followed by `make`.
1. Copy `xdf.h` into `$sigviewer/external/include` and `libbiosig.a` to `$sigviewer/external/lib`.
