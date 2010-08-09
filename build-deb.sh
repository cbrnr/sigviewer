#!/bin/sh
dir=.deb-build

# create temporary build directories
mkdir -p $dir/sigviewer/usr/bin
mkdir -p $dir/sigviewer/usr/share
mkdir -p $dir/sigviewer/usr/share/pixmaps
mkdir -p $dir/sigviewer/usr/share/applications
mkdir -p $dir/sigviewer/DEBIAN

# copy necessary files
cp ./bin/release/sigviewer ./$dir/sigviewer/usr/bin/
cp ./deb_building_stuff/sigviewer128.png ./$dir/sigviewer/usr/share/pixmaps/
cp ./deb_building_stuff/sigviewer.desktop ./$dir/sigviewer/usr/share/applications/

# get current version of sigviewer
version=`head -n 1 src/version.txt`

# get local architecture
architecture=`dpkg-architecture -l | grep DEB_BUILD_ARCH= | sed -e '/DEB_BUILD_ARCH=/s/DEB_BUILD_ARCH=//'`

# get file size of the binary
filesizestring=`ls -s bin/release/sigviewer`
set -- $filesizestring
filesize=$1

# replace architecture, file size and version in the control file
sed -e '/Architecture: /s/<architecture-via-script>/'$architecture'/' ./deb_building_stuff/deb_control_template  |  sed -e '/Installed-Size: /s/<bin-size-via-script>/'$filesize'/' |  sed -e '/Version: /s/<version>/'$version'/'  |  sed -e '/Replaces: /s/<version>/'$version'/'  > ./$dir/sigviewer/DEBIAN/control

# build the SigViewer package
dpkg -b ./$dir/sigviewer sigviewer-$version-$architecture.deb

# delete all temporary build directories
rm -r $dir

tar czvf extern-$architecture.tar.gz --exclude=".*" extern
