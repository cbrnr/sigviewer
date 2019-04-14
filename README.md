SigViewer
=========

SigViewer is a viewing application for biosignals such as EEG or MEG time series. In addition to viewing raw data, SigViewer can also create, edit, and display event information (such as annotations or artifact selections).
This version (or fork) fixes a regression w.r.t. reading of event files stored in GDF
format, and can also save events in GDF formatat, like SigViewer 0.5.1 and earlier.

Download
--------
- [SigViewer 0.6.3 (Windows 64bit)](https://pub.ist.ac.at/~schloegl/software/sigviewer/sigviewer-0.6.3-win64.exe) (Windows 64bit)
- [SigViewer 0.6.3 (Windows 32bit)](https://pub.ist.ac.at/~schloegl/software/sigviewer/sigviewer-0.6.3-win32.exe) (Windows 32bit)
- MacOSX: [homebrew] (https://brew.sh/)
    brew tap schloegl/biosig
    brew install sigviewer
- [SigViewer 0.6.2 (Debian 10 (buster), Ubuntu 19.04)] (https://tracker.debian.org/pkg/sigviewer)
    apt-get install sigviewer
- [SigViewer (src)]
    git clone https://github.com/schloegl/sigviewer.git

Screenshots
-----------
![screenshot-1](https://github.com/schloegl/sigviewer/raw/master/screenshot-1.png)

Building SigViewer
------------------

### Windows
SigViewer for windows can be build best with the [MXE] toolchain
(http://mxe.cc). All dependencies are provided in [this repository]  (http://github.com/schloegl/mxe.git)
1. Install [MXE requirements] (https://mxe.cc/#requirements)
1. Get MXE repository with Biosig && SigViewer make files (https://github.com/schloegl/mxe.git)
1. Compile the required prerequisites with
    make sigviewer
1. The resulting binaries are available in
    usr/*/bin/sigviewer*.exe

### macOS
Compiling SigViewer for Mac is tested by using homebrew.

1. Install [homebrew](http://brew.sh) using
   /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
1. Add Biosig/SigViewer recipies
   brew tap schloegl/biosig
1. Install SigViewer and its prerequisites
   brew install sigviewer

### Linux
1. Install prerequisites:
    * [libbiosig](https://biosig.sourceforge.io),
    * [libxdf](https://github.com/xdf-modules/libxdf)
    * Qt5.
    On Debian and Ubuntu, it is easiest with
        apt-get build-dep sigviewer
1. Download and extract sources
1. Compile with:
    qmake -qt=qt5 && make && sudo make install

Building external dependencies
------------------------------
External dependencies can be installed in the same way than above.

