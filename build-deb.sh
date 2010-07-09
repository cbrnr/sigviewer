#!/bin/sh
dir=.deb-build

mkdir -p $dir/sigviewer/usr/bin
mkdir -p $dir/sigviewer/usr/share
mkdir -p $dir/sigviewer/usr/share/pixmaps
mkdir -p $dir/sigviewer/usr/share/applications
mkdir -p $dir/sigviewer/DEBIAN

cp ./bin/sigviewer ./$dir/sigviewer/usr/bin/
cp ./deb_building_stuff/sigviewer128.png ./$dir/sigviewer/usr/share/pixmaps/
cp ./deb_building_stuff/sigviewer.desktop ./$dir/sigviewer/usr/share/applications/

architecture=`dpkg-architecture -l | grep DEB_BUILD_ARCH= | sed -e '/DEB_BUILD_ARCH=/s/DEB_BUILD_ARCH=//'`
filesizestring=`ls -s bin/sigviewer`
set -- $filesizestring
filesize=$1

sed -e '/Architecture: /s/<architecture-via-script>/'$architecture'/' ./deb_building_stuff/deb_control_template | sed -e '/Installed-Size: /s/<bin-size-via-script>/'$filesize'/' >./$dir/sigviewer/DEBIAN/control

dpkg -b ./$dir/sigviewer sigviewer_0_4_1_$architecture.deb

rm -r $dir