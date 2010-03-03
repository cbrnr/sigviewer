#!/bin/sh
if [ $# != 1 ]
  then echo 'architecture missing... please run again with i386 or x86_64 or ...'
else


cp ./bin/sigviewer ./deb/sigviewer/usr/bin/

architecture=$1

sed -e '/Architecture: /s/<architecture-via-script>/'$architecture'/' ./deb/sigviewer/DEBIAN/control_template >./deb/sigviewer/DEBIAN/control

dpkg -b ./deb/sigviewer sigviewer_$architecture.deb

fi