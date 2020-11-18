#!/bin/bash

if [[ "${BINARY_TYPE}" == "AppImage" ]]; then
    source ci/scripts/appImage/build_appImage.sh
elif [[ "${BINARY_TYPE}" == "deb" ]]; then
    docker exec build-container bash -c "source ci/scripts/deb/build_deb.sh"
    mv phonexa-screens_*.deb phonexa-screens-${VERSION}.deb
elif [[ "${BINARY_TYPE}" == "rpm" ]]; then
    docker exec build-container bash -c "source ci/scripts/rpm/build_rpm.sh"
    sudo chown -R 2000:2000 phonexa-screens-${VERSION_NUMBER}
    mv phonexa-screens-${VERSION_NUMBER}/RPMS/x86_64/phonexa-*.rpm phonexa-screens-${VERSION}.rpm
elif [[ "${BINARY_TYPE}" == "exe" ]]; then
    mkdir build && cd build
    cmake .. -G"NMake Makefiles" -DCMAKE_CXX_COMPILER=cl -DVERSION_SUFIX=${VERSION_SUFFIX} -DBUILD_NUMBER=${BUILD_NUMBER} -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
    nmake
    cd ..
	
    echo "--> Package Windows"
    mkdir packageDir
    mv build/src/phonexa-screens.exe packageDir/phonexa-screens.exe
    windeployqt.exe packageDir/phonexa-screens.exe
    cp build/translations/ksnip_*.qm ./packageDir/translations/
    cp kImageAnnotator/build/translations/kImageAnnotator_*.qm ./packageDir/translations/
    cp /c/openssl/openssl-1.0.2o-x32-VC2017/libeay32.dll ./packageDir/
    cp /c/openssl/openssl-1.0.2o-x32-VC2017/ssleay32.dll ./packageDir/
	
    7z a phonexa-screens-${VERSION}-windows.zip ./packageDir/*
elif [[ "${BINARY_TYPE}" == "app" ]]; then
    mkdir build && cd build
    cmake .. -DVERSION_SUFIX=${VERSION_SUFFIX} -DBUILD_NUMBER=${BUILD_NUMBER} -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
    make
    cd ..

    echo "--> Package MacOS"
    mkdir packageDir
    mv build/src/phonexa-screens.app packageDir/phonexa-screens.app
    macdeployqt packageDir/phonexa-screens.app
    cp ./build/translations/ksnip_*.qm ./packageDir/phonexa-screens.app/Contents/Resources/
    cp ./build/translations/kImageAnnotator_*.qm ./packageDir/phonexa-screens.app/Contents/Resources/
    sudo hdiutil create phonexa-screens-${VERSION}.dmg -volname "Phonexa-screens" -fs HFS+ -srcfolder packageDir/
fi
