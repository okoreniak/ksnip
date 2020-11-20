cmake_minimum_required(VERSION 3.5)
set(KSNIP_EXECUTABLE phonexa-screens)
project(ksnip LANGUAGES CXX VERSION 1.8.1)
macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()


if (DEFINED VERSION_SUFIX AND NOT "${VERSION_SUFIX}" STREQUAL "")
    set(KSNIP_VERSION_SUFIX "-${VERSION_SUFIX}")
endif()

set(KSNIP_VERSION "${PROJECT_VERSION}${KSNIP_VERSION_SUFIX}")

include(GNUInstallDirs)

if (WIN32)
  set(KSNIP_LANG_INSTALL_DIR "translations")
  set(KIMAGEANNOTATOR_LANG_INSTALL_DIR "translations")
elseif (APPLE)
        set(KSNIP_LANG_INSTALL_DIR "../Resources")
        set(KIMAGEANNOTATOR_LANG_INSTALL_DIR "../Resources")
elseif (UNIX)
  set(KSNIP_LANG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/ksnip/translations")
  set(KIMAGEANNOTATOR_LANG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/kImageAnnotator/translations")
endif ()

configure_file(src/BuildConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/BuildConfig.h)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build Unit Tests" OFF)

if (UNIX AND NOT APPLE)
    # Without ECM we're unable to load XCB
    find_package(ECM REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

    find_package(X11 REQUIRED)
    find_package(XCB COMPONENTS XFIXES)
endif ()

set(QT_COMPONENTS Core Widgets Network Xml PrintSupport DBus Svg)
set(QT_MIN_VERSION 5.9.4)

if (X11_FOUND)
  list(APPEND QT_COMPONENTS X11Extras)
elseif (WIN32)
  list(APPEND QT_COMPONENTS WinExtras)
endif()

if (BUILD_TESTS)
  list(APPEND QT_COMPONENTS Test)
endif()

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED ${QT_COMPONENTS})

set(KIMAGEANNOTATOR_MIN_VERSION "0.4.0")
find_package(kImageAnnotator ${KIMAGEANNOTATOR_MIN_VERSION} REQUIRED)

if (kImageAnnotator_FOUND)
  include_directories(${kImageAnnotator_INCLUDE_DIRS})
  include_directories("${kImageAnnotator_DIR}/../../../include")

if(WIN32)
  set(LIBS ${LIBS} "${kImageAnnotator_DIR}/../../../lib/kImageAnnotator.lib")
  set(LIBS ${LIBS} "${kImageAnnotator_DIR}/../../../../kColorPicker/lib/kColorPicker.lib")
elseif(APPLE)
    set(LIBS ${LIBS} "${kImageAnnotator_DIR}/../../../lib/libkImageAnnotator.a")
    set(LIBS ${LIBS} "${kImageAnnotator_DIR}/../../../lib/libkColorPicker.a")
  #elseif(APPLE)
endif()
 # target_link_libraries(ksnip ${LIBS})

  #target_link_libraries (ksnip ${kImageAnnotator_LIBRARIES})
endif (kImageAnnotator_FOUND)
get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
message("inc_dirs = ${inc_dirs}")
#message(STATUS )


set(BASEPATH "${CMAKE_SOURCE_DIR}")
include_directories("${BASEPATH}")

add_subdirectory(src)
add_subdirectory(translations)
add_subdirectory(desktop)

if (BUILD_TESTS)
  configure_file(src/BuildConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/tests/BuildConfig.h)

  enable_testing()
  add_subdirectory(tests)

endif (BUILD_TESTS)
