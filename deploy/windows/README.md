Install the [Qt Installer Framework](https://download.qt.io/official_releases/qt-installer-framework/) and make sure its bin folder is added to the path.

The `sigviewer.exe` binary needs to be present in `$sigviewer/bin/release` - if not, build it. Open a command prompt, change into `$sigviewer/bin/release` and type:

    windeployqt sigviewer.exe

Next, move the contents of the `$sigviewer/bin/release` folder into `$sigviewer/deploy/windows/packages/org.sigviewer.sigviewer/data`. Furthermore, copy the file `$sigviewer/src/images/sigviewer.ico` to this folder.

Finally, change into `$sigviewer/deploy/windows` in the command prompt and type:

    binarycreator --offline-only -c config/config.xml -p packages sigviewer
