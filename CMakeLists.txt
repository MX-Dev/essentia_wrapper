cmake_minimum_required(VERSION 2.8.6)

#
# If the user specifies -DINSTALL_DIR on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set INSTALL_DIR
#
if(DEFINED INSTALL_DIR)
    set(INSTALL_DIR ${INSTALL_DIR} CACHE STRING "Installation directory")
else()
    set(INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../_bin/\${BUILD_TYPE}" CACHE STRING "Installation directory")
endif()

#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take theirdefinition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
  build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
  Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build,
  options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
  RelWithDebInfo MinSizeRel.")
ENDIF()

project(essentia-wrapper)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#include cotire
include(cotire)

# libtype and install type
set(LIB_TYPE STATIC)
set(INSTALL_LIB_TYPE ARCHIVE)

if(BUILD_SHARED_LIBS)
    # User wants to build Dynamic Libraries, so change the LIB_TYPE variable to CMake keyword 'SHARED'
    set(LIB_TYPE SHARED)
    IF(MSVC OR MINGW)
        SET(INSTALL_LIB_TYPE RUNTIME)
    ELSE()
        SET(INSTALL_LIB_TYPE LIBRARY)
    ENDIF()
endif()

# include common modules
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CMakeDetermineSystem)

# Setup Essentia
set(OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/extern/essentia/cmake)
find_package(Essentia)
set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH})

option(BUILD_SHARED_LIBS "Build Shared Libraries" ON)
option(MAINTAINER_INSTALL "Install the library for use it binary in other projects" OFF)

# You can tweak some common (for all subprojects) stuff here. For example:
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

IF("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
ENDIF()

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_COLOR_MAKEFILE   ON)

# Remove 'lib' prefix for shared libraries on Windows
IF(WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
ENDIF()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    # using Clang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -std=c++11 \
        -Wno-overflow")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    # using GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -std=c++11 \
        -Wno-overflow \
        -Wno-deprecated-declarations")
    if(MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc)
    endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Intel)
  # using Intel C++
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
      # using Visual Studio C++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -MP  \
        -D_WIN32_WINNT=0x0600 \
        -WX -D_CRT_SECURE_NO_WARNINGS \
        -wd\"4503\" \
        -wd\"4101\" \
        -wd\"4251\" \
        -DNOMINMAX \
        -WX")
endif()

include_directories(${PROJECT_BINARY_DIR})
include_directories(${Essentia_INCLUDE_DIRS})

set(sources_dir ${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE sources ${sources_dir}/*.*)

set(include_dirs ${include_dirs} ${sources_dir})

include_directories(${include_dirs})

# Expose public includes to other
# subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${include_dirs} ${PROJECT_BINARY_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(${PROJECT_NAME}_SOURCES
    ${sources}
    CACHE INTERNAL "${PROJECT_NAME}: Sources" FORCE)
    
# add groups for visual studio
FOREACH(file in ${${PROJECT_NAME}_SOURCES})
    IF(EXISTS ${file})
        file(RELATIVE_PATH relpath ${PROJECT_SOURCE_DIR} ${file})
        get_filename_component(path ${relpath} DIRECTORY)
        string(REPLACE "/" "\\" groupname ${path})
        set(${PROJECT_NAME}_SOURCE_GROUPS ${${PROJECT_NAME}_SOURCE_GROUPS} ${groupname} CACHE INTERNAL "" FORCE)
        set(${PROJECT_NAME}_SOURCE_GROUP_${groupname} ${${PROJECT_NAME}_SOURCE_GROUP_${groupname}} ${file} CACHE INTERNAL "" FORCE)
    ENDIF()
ENDFOREACH()    

FOREACH(group in ${${PROJECT_NAME}_SOURCE_GROUPS})
        SOURCE_GROUP(${group} FILES ${${PROJECT_NAME}_SOURCE_GROUP_${group}})
ENDFOREACH()    

set(ADD_LIBRARIES
    ${ADD_LIBRARIES}
    ${Essentia_LIBRARIES}
)

add_library(${PROJECT_NAME} ${LIB_TYPE} ${sources})
target_link_libraries(${PROJECT_NAME} ${ADD_LIBRARIES})

#generate header for windows dll
include(GenerateExportHeader)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
generate_export_header(${PROJECT_NAME}
             BASE_NAME ${PROJECT_NAME_UPPER}
             EXPORT_MACRO_NAME ${PROJECT_NAME_UPPER}_API
             EXPORT_FILE_NAME ${PROJECT_NAME}_exports.h
             STATIC_DEFINE ${PROJECT_NAME_UPPER}_BUILT_AS_STATIC
)

# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Cotire
set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_PREFIX_HEADER_INCLUDE_PATH "${PROJECT_SOURCE_DIR}/../boost")
set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
if(ANDROID)
    set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_ENABLE_PRECOMPILED_HEADER FALSE)
endif()
cotire(${PROJECT_NAME})

if(MAINTAINER_INSTALL)
        install(TARGETS ${PROJECT_NAME} ${INSTALL_LIB_TYPE} DESTINATION ${INSTALL_DIR}/lib)
        install(FILES
            ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_exports.h
            ${sources_dir}/essentia_wrapper.h
            DESTINATION ${INSTALL_DIR}/include)
else()
    if(BUILD_SHARED_LIBS)
        install(TARGETS ${PROJECT_NAME} ${INSTALL_LIB_TYPE} DESTINATION ${INSTALL_DIR})
    endif()
endif()
