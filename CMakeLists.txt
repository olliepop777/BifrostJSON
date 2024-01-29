cmake_minimum_required(VERSION 3.17)

# BIFROST_LOCATION must be passed-in or defined by an environment variable.
# Passed-in on the CMake command line overrides the environment variable.
if( NOT BIFROST_LOCATION )
    if( DEFINED ENV{BIFROST_LOCATION} )
        set( BIFROST_LOCATION $ENV{BIFROST_LOCATION})
    else()
        message( FATAL_ERROR "Bifrost: BIFROST_LOCATION cmake variable must be defined.")
    endif()
endif()

if( NOT BIFROST_JSON_RELEASE_VERSION )
    set( BIFROST_JSON_RELEASE_VERSION 1.0.0)
endif()

set(CMAKE_CXX_STANDARD 17)
# Project Name
project(
    BifrostJSON 
    VERSION ${BIFROST_JSON_RELEASE_VERSION}
    LANGUAGES CXX
)

set(BIFROST_JSON_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}-${PROJECT_VERSION}")

# Bifrost setup and utilities...
include(${BIFROST_LOCATION}/sdk/cmake/setup.cmake)

#Third-party external libs
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann_json-3.11.2)

# Verbose CMake
set( CMAKE_VERBOSE_MAKEFILE TRUE)

# Find Bifrost
find_package(Bifrost REQUIRED SDK)

# Build from sources
add_subdirectory(src)
