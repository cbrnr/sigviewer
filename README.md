SigViewer
=========

SigViewer is a viewing application for biosignals such as EEG or MEG time series. In addition to viewing raw data, SigViewer can also create, edit, and display event information (such as annotations or artifact selections).


Building SigViewer
------------------

SigViewer depends on the following external libraries:

- [Qt4](http://download.qt.io/archive/qt/4.8/4.8.6/)
- [libbiosig](http://biosig.sourceforge.net/)

General instructions:

1. Install Qt4 development packages
2. Download the "external" archive for the appropriate platform from http://sigviewer.sourceforge.net/
3. Extract the "external" archive into SigViewer's source folder (the folder where "sigviewer.pro" is located)
4. Run `qmake sigviewer.pro && make`

The SigViewer binary can be found in the "bin" directory (release or debug subfolders).


Deploying SigViewer
-------------------

Debian/Ubuntu:
1. Build SigViewer
2. To create a .deb package, run `./build-deb.sh`
