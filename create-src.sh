#!/bin/sh

version_major=`sed -n -e 's/^VERSION_MAJOR = \([0-9]*\)$/\1/p' sigviewer.pro`
version_minor=`sed -n -e 's/^VERSION_MINOR = \([0-9]*\)$/\1/p' sigviewer.pro`
version_build=`sed -n -e 's/^VERSION_BUILD = \([0-9]*\)$/\1/p' sigviewer.pro`
version=$version_major.$version_minor.$version_build

mkdir -p sigviewer-$version-src
cp * sigviewer-$version-src
cp -r deb_building_stuff sigviewer-$version-src
cp -r src sigviewer-$version-src/src

rm sigviewer-$version-src/*.pro.user
find sigviewer-$version-src -name "Makefile*" -delete

tar czvf sigviewer-$version-src.tar.gz --exclude=.* sigviewer-$version-src
rm -rf sigviewer-$version-src
