# get current version of sigviewer
version=`head -n 1 src/version.txt`
revision=`svn info | grep Revision: | sed -e '/Revision:/s/Revision: //'`

# set directory and zip file name
name=sigviewer-$version-r$revision-src

# 
rm -r $name


# svn export of current working copy
svn export . ./$name

# zip directory
zip -r -q $name.zip $name

# remove temporary files
rm -r $name