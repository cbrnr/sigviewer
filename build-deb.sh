#!/bin/sh
dir=.deb-build

mkdir $dir
mkdir $dir/sigviewer
mkdir $dir/sigviewer/usr
mkdir $dir/sigviewer/usr/bin
mkdir $dir/sigviewer/DEBIAN

cp ./bin/sigviewer ./$dir/sigviewer/usr/bin/

architecture=`dpkg-architecture -l | grep DEB_BUILD_ARCH= | sed -e '/DEB_BUILD_ARCH=/s/DEB_BUILD_ARCH=//'`

sed -e '/Architecture: /s/<architecture-via-script>/'$architecture'/' ./deb_control_template >./$dir/sigviewer/DEBIAN/control

dpkg -b ./$dir/sigviewer sigviewer_$architecture.deb

rm $dir/sigviewer/usr/bin/*
rm $dir/sigviewer/DEBIAN/*
rmdir $dir/sigviewer/usr/bin
rmdir $dir/sigviewer/usr
rmdir $dir/sigviewer/DEBIAN
rmdir $dir/sigviewer
rmdir $dir