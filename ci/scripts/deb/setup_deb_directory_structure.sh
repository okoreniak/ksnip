#!/bin/bash

echo "--> Create directory and copy everything we need to deliver"
mkdir phonexa-screens-${VERSION_NUMBER}
cp -R CMakeLists.txt cmake/ desktop/ icons/ LICENSE README.md src/ translations/ phonexa-screens-${VERSION_NUMBER}/
echo "--> Package source content"
tar -cvzf phonexa-screens_${VERSION_NUMBER}.orig.tar.gz phonexa-screens-${VERSION_NUMBER}/
echo "--> Copy source package to debian directory"
cp -R ci/debian phonexa-screens-${VERSION_NUMBER}/
