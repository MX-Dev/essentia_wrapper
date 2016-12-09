#.rst:
# FindEssentia
# ----------
#
# Find the native Essentia includes and library
#
# This module defines::
#
#  Essentia_FOUND
#  Essentia_INCLUDE_DIRS, where to find gl2.h ...
#  Essentia_LIBRARIES, the libraries to link against to use OpenGL ES.
#  ESSENTIA_FOUND, If false, do not try to use OpenGL ES.
#

include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/TargetArch.cmake)
target_architecture(Essentia_TARGET_ARCH)

get_filename_component(Essentia_BASE_PATH "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)

include(CheckIncludeFiles)

# Overwrite cross compiling toolchain stuff to search the module 
# in the system file system
set(OLD_CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ${CMAKE_FIND_ROOT_PATH_MODE_LIBRARY})
set(OLD_CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ${CMAKE_FIND_ROOT_PATH_MODE_INCLUDE})
set(OLD_CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ${CMAKE_FIND_ROOT_PATH_MODE_PACKAGE})

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE NEVER)

find_path(Essentia_INCLUDE_DIRS essentia.h PATHS "${Essentia_BASE_PATH}/include" NO_DEFAULT_PATH)

IF(MSVC)
    IF(${Essentia_TARGET_ARCH} STREQUAL i386)
        set(Essentia_LIB_DIR "${Essentia_BASE_PATH}/lib/windows/win32")
    ELSEIF(${Essentia_TARGET_ARCH} STREQUAL x86_64)
        set(Essentia_LIB_DIR "${Essentia_BASE_PATH}/lib/windows/x64")
    ENDIF()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(Essentia_LIB_DIR "${Essentia_BASE_PATH}/lib/darwin")
elseif(ANDROID)
    set(Essentia_LIB_DIR "${Essentia_BASE_PATH}/lib/android/${ANDROID_ABI}")
else()
    set(Essentia_LIB_DIR "${Essentia_BASE_PATH}/lib/linux/${Essentia_TARGET_ARCH}")
endif()

message(STATUS ${Essentia_LIB_DIR})

find_library(Essentia_LIBRARIES NAMES "libessentia" "essentia"  PATHS "${Essentia_LIB_DIR}"  NO_DEFAULT_PATH)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ${OLD_CMAKE_FIND_ROOT_PATH_MODE_LIBRARY})
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ${OLD_CMAKE_FIND_ROOT_PATH_MODE_INCLUDE})
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ${OLD_CMAKE_FIND_ROOT_PATH_MODE_PACKAGE})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Essentia
    DEFAULT_MSG
    Essentia_INCLUDE_DIRS
    Essentia_LIBRARIES
)

mark_as_advanced(
    Essentia_INCLUDE_DIRS
    Essentia_LIBRARIES
    Essentia_LIB_DIR
)
