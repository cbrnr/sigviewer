SigViewer
=========

SigViewer is a viewing application for biosignals such as EEG or MEG time series. In addition to viewing raw data, SigViewer can also create, edit, and display event information (such as annotations or artifact selections).


Building SigViewer
------------------

SigViewer depends on the following external libraries:

- [Qt 5](https://www.qt.io/download/)
- [libbiosig](http://biosig.sourceforge.net/)
- [libxdf](https://github.com/Yida-Lin/libxdf)

General instructions:

1. Install Qt 5 development packages
2. Download the "external" archive for the appropriate platform from http://sigviewer.sourceforge.net/
3. Extract the "external" archive into SigViewer's source folder (the folder where "sigviewer.pro" is located)
4. Download libxdf for the appropriate platform from https://github.com/Yida-Lin/libxdf/releases
5. Extract the contents of the libxdf archive (`libxdf.a` goes into `external/lib` and `xdf.h` goes into `external/include`)
4. Run `qmake sigviewer.pro && make`

The SigViewer binary can be found in the "bin" directory (release or debug subfolders).


Deploying SigViewer
-------------------

Debian/Ubuntu:

1. Build SigViewer
2. To create a .deb package, run `./build-deb.sh`
