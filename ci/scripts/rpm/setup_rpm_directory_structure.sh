#!/bin/bash

echo "--> Create directory and everything we need to deliver"
mkdir phonexa-screens-${VERSION_NUMBER}
cp -R CMakeLists.txt cmake/ desktop/ icons/ LICENSE README.md src/ translations/ phonexa-screens-${VERSION_NUMBER}/
echo "--> Package directory"
tar -cvzf phonexa-screens-${VERSION_NUMBER}.tar.gz phonexa-screens-${VERSION_NUMBER}/
echo "--> Move package to SOURCE directory"
mkdir phonexa-screens-${VERSION_NUMBER}/SOURCES
mv phonexa-screens-${VERSION_NUMBER}.tar.gz phonexa-screens-${VERSION_NUMBER}/SOURCES/
echo "--> Copy spec file to SPEC directory"
mkdir phonexa-screens-${VERSION_NUMBER}/SPECS
cp phonexa-screens.spec phonexa-screens-${VERSION_NUMBER}/SPECS/phonexa-screens-${VERSION_NUMBER}.spec
