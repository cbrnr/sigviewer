#!/bin/sh
dir=.deb-build

mkdir $dir
mkdir $dir/sigviewer
mkdir $dir/sigviewer/usr
mkdir $dir/sigviewer/usr/bin
mkdir $dir/sigviewer/usr/share
mkdir $dir/sigviewer/usr/share/pixmaps
mkdir $dir/sigviewer/usr/share/applications
mkdir $dir/sigviewer/DEBIAN

cp ./bin/sigviewer ./$dir/sigviewer/usr/bin/
cp ./deb_building_stuff/sigviewer128.png ./$dir/sigviewer/usr/share/pixmaps/
cp ./deb_building_stuff/sigviewer.desktop ./$dir/sigviewer/usr/share/applications/

architecture=`dpkg-architecture -l | grep DEB_BUILD_ARCH= | sed -e '/DEB_BUILD_ARCH=/s/DEB_BUILD_ARCH=//'`

sed -e '/Architecture: /s/<architecture-via-script>/'$architecture'/' ./deb_building_stuff/deb_control_template >./$dir/sigviewer/DEBIAN/control

dpkg -b ./$dir/sigviewer sigviewer_$architecture.deb

rm $dir/sigviewer/usr/bin/*
rm $dir/sigviewer/DEBIAN/*
rm $dir/sigviewer/usr/share/pixmaps/*
rm $dir/sigviewer/usr/share/applications/*
rmdir $dir/sigviewer/usr/share/pixmaps
rmdir $dir/sigviewer/usr/share/applications
rmdir $dir/sigviewer/usr/share
rmdir $dir/sigviewer/usr/bin
rmdir $dir/sigviewer/usr
rmdir $dir/sigviewer/DEBIAN
rmdir $dir/sigviewer
rmdir $dir