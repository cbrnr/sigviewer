#!/bin/sh

version=`head -n 1 src/version.txt`

mkdir -p sigviewer-$version-src
cp * sigviewer-$version-src
cp -r deb_building_stuff sigviewer-$version-src
mkdir -p sigviewer-$version-src/external
cp -r src sigviewer-$version-src/src

tar czvf sigviewer-$version-src.tar.gz --exclude=.* --exclude=*.pro.user --exclude=Makefile* sigviewer-$version-src

rm -rf sigviewer-$version-src
