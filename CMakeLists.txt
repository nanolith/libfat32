cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0048 NEW)
PROJECT(libfat32 VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS ON)

string(REPLACE "." ";" LIBFAT32_VERSION_LIST "${CMAKE_PROJECT_VERSION}")
list(GET LIBFAT32_VERSION_LIST 0 LIBFAT32_VERSION_MAJOR)
list(GET LIBFAT32_VERSION_LIST 1 LIBFAT32_VERSION_MINOR)
list(GET LIBFAT32_VERSION_LIST 2 LIBFAT32_VERSION_REL)

INCLUDE(CheckSymbolExists)

find_package(PkgConfig REQUIRED)
pkg_search_module(minunit REQUIRED IMPORTED_TARGET minunit)

#Build config.h
configure_file(config.h.cmake include/libfat32/config.h)

#includes
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include)

#source files
FILE(GLOB_RECURSE LIBFAT32_SOURCES src/*.c)

#test source files
FILE(GLOB_RECURSE LIBFAT32_TEST_SOURCES test/*.cpp)

ADD_LIBRARY(fat32 STATIC ${LIBFAT32_SOURCES})
SET_PROPERTY(TARGET fat32 PROPERTY C_STANDARD 17)
TARGET_COMPILE_OPTIONS(
    fat32 PRIVATE -fPIC -O2
    -Wall -Werror -Wextra -Wpedantic -Wno-unused-command-line-argument)

ADD_EXECUTABLE(testfat32
    ${LIBFAT32_SOURCES} ${LIBFAT32_TEST_SOURCES})
SET_PROPERTY(TARGET testfat32 PROPERTY C_STANDARD 17)
SET_PROPERTY(TARGET testfat32 PROPERTY CXX_STANDARD 17)
TARGET_COMPILE_OPTIONS(
    testfat32 PRIVATE -g -O0 --coverage -I${minunit_INCLUDE_DIRS}
                       -Wall -Werror -Wextra -Wpedantic
                       -Wno-unused-command-line-argument)
TARGET_LINK_OPTIONS(testfat32 PRIVATE --coverage)
TARGET_LINK_LIBRARIES(testfat32 PkgConfig::minunit)

ADD_CUSTOM_TARGET(
    test
    COMMAND testfat32
    DEPENDS testfat32)

#Build a pkg-config file
SET(FAT32_PC "${CMAKE_BINARY_DIR}/libfat32.pc")
FILE(WRITE  ${FAT32_PC} "Name: libfat32")
FILE(APPEND ${FAT32_PC} "\nDescription: FAT32 library")
FILE(APPEND ${FAT32_PC} "\nVersion: ${CMAKE_PROJECT_VERSION}")
FILE(APPEND ${FAT32_PC} "\nURL: https://github.com/nanolith/libfat32")
FILE(APPEND ${FAT32_PC} "\nprefix=\${pcfiledir}/../..")
FILE(APPEND ${FAT32_PC} "\nlibdir=\${prefix}/lib")
FILE(APPEND ${FAT32_PC} "\nincludedir=\${prefix}/include")
FILE(APPEND ${FAT32_PC} "\nLibs: -L\${libdir} -lfat32")
FILE(APPEND ${FAT32_PC} "\nCflags: -I\${includedir}")
INSTALL(FILES ${FAT32_PC} DESTINATION lib/pkgconfig)

FILE(GLOB LIBFAT32_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/libfat32/*.h")

INSTALL(FILES ${LIBFAT32_INCLUDES} DESTINATION include/libfat32)
INSTALL(
    FILES ${CMAKE_BINARY_DIR}/include/libfat32/config.h
    DESTINATION include/libfat32)

#Install library
INSTALL(TARGETS fat32
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
