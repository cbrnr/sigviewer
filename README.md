SigViewer
=========

SigViewer is a viewing application for biosignals such as EEG or MEG time series. In addition to viewing raw data, SigViewer can also create, edit, and display event information (such as annotations or artifact selections).

Download
--------
- [SigViewer 0.6.2 (Windows 64bit)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.2/sigviewer-0.6.2-win64.exe)
- [SigViewer 0.6.2 (Windows 32bit)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.2/sigviewer-0.6.2-win32.exe)
- [SigViewer 0.6.4 (macOS)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/sigviewer-0.6.4-macos.dmg)
- [SigViewer 0.6.4 (Linux)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/sigviewer-0.6.4-linux.zip)
    * [SigViewer 0.6.4 (Arch Linux AUR)](https://aur.archlinux.org/packages/sigviewer/)
    * [SigViewer 0.5.1 (Debian Stable)](https://packages.debian.org/stretch/sigviewer)
    * [SigViewer 0.6.2 (Debian Testing)](https://packages.debian.org/testing/science/sigviewer)
    * [SigViewer 0.6.2 (Debian Unstable)](https://packages.debian.org/sid/science/sigviewer)
    * [SigViewer 0.5.1 (Ubuntu 18.04 LTS)](https://packages.ubuntu.com/bionic/sigviewer)
    * [SigViewer 0.6.2 (Ubuntu 19.04)](https://packages.ubuntu.com/disco/sigviewer)
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
    - [External dependencies (Windows 64bit)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.2/external-0.6.2-win64.zip)
    - [External dependencies (Windows 32bit)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.2/external-0.6.2-win32.zip)
    - [External dependencies (macOS)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/external-0.6.4-macos.zip)
    - [External dependencies (Linux)](https://github.com/cbrnr/sigviewer/releases/download/v0.6.4/external-0.6.4-linux.zip)

### Windows
SigViewer requires Windows 7 or Windows 10. Other versions might work, but have not been tested. First, download the offline installer for [Qt 5.9.6 for Windows](http://download.qt.io/official_releases/qt/5.9/5.9.6/qt-opensource-windows-x86-5.9.6.exe). Run the installation wizard and make sure to also select MinGW 5.3 in the Tools group. Once the installation is completed, a new folder _Qt 5.9.6_ is added to the Start menu. It contains the command prompt _Qt 5.9.6 for Desktop_, which has all required build tools (`qmake` and `mingw32-make`) added to its path. Make sure you use this command prompt if you want to build on the command line. Alternatively, you can build SigViewer with Qt Creator, which is installed along with Qt by default. Here, we describe the build process using Qt Creator.

1. Download and unzip the [SigViewer source](https://github.com/cbrnr/sigviewer/archive/v0.6.2.zip).
1. Provide all external dependencies by downloading the [external archive](https://github.com/cbrnr/sigviewer/releases/download/v0.6.2/external-0.6.2-win32.zip) and extracting it inside `$sigviewer` (see below for more details and options regarding the external dependencies).
1. Open `sigviewer.pro` in Qt Creator.
1. Accept the default configuration by clicking on _Configure Project_.
1. Click on _Build Project_ (the hammer icon) to build SigViewer.
1. Click on _Run_ (the play icon) to start SigViewer.
1. To create a stand-alone version of SigViewer, open the _Qt 5.9.6 for Desktop_ command prompt, change into `$sigviewer/bin/release`, and run `windeployqt sigviewer.exe`. SigViewer now runs on any Windows machine (no previous Qt installation is required) if the contents of this directory is distributed together with `sigviewer.exe` (see `$sigviewer/deploy/windows/README.md` for details on how to create a stand-alone installer package).


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
Building libbiosig on Windows is currently not possible. Please use our pre-built binary included in the [external archive](https://github.com/cbrnr/sigviewer/releases/download/v0.6.2/external-0.6.2-windows.zip), which we built using [MXE](http://mxe.cc/).

To build libxdf from source, follow these steps:

1. Download and unzip the [libxdf source](https://github.com/Yida-Lin/libxdf/archive/v0.98.zip) (SigViewer 0.6.2 uses libxdf 0.98).
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
