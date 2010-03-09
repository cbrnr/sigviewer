#!/bin/sh
if [ $# != 1 ]
  then echo 'architecture missing... please run again with i386 or x86_64 or ...'
else

dir=.deb-build

mkdir $dir
mkdir $dir/sigviewer
mkdir $dir/sigviewer/usr
mkdir $dir/sigviewer/usr/bin
mkdir $dir/sigviewer/DEBIAN

cp ./bin/sigviewer ./$dir/sigviewer/usr/bin/

architecture=$1

sed -e '/Architecture: /s/<architecture-via-script>/'$architecture'/' ./deb_control_template >./$dir/sigviewer/DEBIAN/control

dpkg -b ./$dir/sigviewer sigviewer_$architecture.deb

rm $dir/sigviewer/usr/bin/*
rm $dir/sigviewer/DEBIAN/*
rmdir $dir/sigviewer/usr/bin
rmdir $dir/sigviewer/usr
rmdir $dir/sigviewer/DEBIAN
rmdir $dir/sigviewer
rmdir $dir

fi