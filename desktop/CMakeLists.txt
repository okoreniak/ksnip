# Add desktop file and desktop icon to target machine
# Add metadata file

if(UNIX AND NOT APPLE)
    install(PROGRAMS com.phonexa.phonexa-screens.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    install(FILES phonexa-screens.svg DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
    install(FILES com.phonexa.phonexa-screens.appdata.xml DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
endif()
