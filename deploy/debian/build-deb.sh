#!/bin/sh
dir=/tmp/sigviewer-deb-build

# create temporary build directories
mkdir -p $dir/sigviewer/usr/bin
mkdir -p $dir/sigviewer/usr/share
mkdir -p $dir/sigviewer/usr/share/pixmaps
mkdir -p $dir/sigviewer/usr/share/applications
mkdir -p $dir/sigviewer/DEBIAN

# copy necessary files
cp ../../bin/release/sigviewer $dir/sigviewer/usr/bin/
cp sigviewer128.png $dir/sigviewer/usr/share/pixmaps/
cp sigviewer.desktop $dir/sigviewer/usr/share/applications/

# get current version of sigviewer
version_major=`sed -n -e 's/^VERSION_MAJOR = \([0-9]*\)$/\1/p' ../../sigviewer.pro`
version_minor=`sed -n -e 's/^VERSION_MINOR = \([0-9]*\)$/\1/p' ../../sigviewer.pro`
version_build=`sed -n -e 's/^VERSION_BUILD = \([0-9]*\)$/\1/p' ../../sigviewer.pro`
version=$version_major.$version_minor.$version_build

# get local architecture
architecture=`dpkg-architecture -l | grep DEB_BUILD_ARCH= | sed -e '/DEB_BUILD_ARCH=/s/DEB_BUILD_ARCH=//'`

# get file size of the binary
filesizestring=`ls -s ../../bin/release/sigviewer`
set -- $filesizestring
filesize=$1

# replace architecture, file size and version in the control file
sed -e '/Architecture: /s/<architecture-via-script>/'$architecture'/' deb_control_template | sed -e '/Installed-Size: /s/<bin-size-via-script>/'$filesize'/' | sed -e '/Version: /s/<version>/'$version'/' | sed -e '/Replaces: /s/<version>/'$version'/' > $dir/sigviewer/DEBIAN/control

# build the SigViewer package
dpkg -b $dir/sigviewer sigviewer-$version-$architecture.deb

# delete all temporary build directories
rm -r $dir
