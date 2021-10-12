cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(CMakeDependentOption)

if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW) # CMake 3.12
endif ()

if (POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

project(lemonade CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(COMPILE_CUDA OFF CACHE BOOL "Compile GPU version")
SET(COMPILE_WASM OFF CACHE BOOL "Don't do wasm whatever this is")

SET(USE_SENTENCEPIECE ON CACHE BOOL "Download and compile SentencePiece")
SET(USE_STATIC_LIBS ON CACHE BOOL "Link statically against non-system libs")
SET(USE_WASM_COMPATIBLE_SOURCE OFF CACHE BOOL "Don't build wasm compatible sources")

find_package(Threads REQUIRED) # Cross-platform compatible way to get threads?



include(GetVersionFromFile)
message(STATUS "Project name: ${PROJECT_NAME}")
message(STATUS "Project version: ${PROJECT_VERSION_STRING_FULL}")


set(AUTHOR "Jerin Philip <jerinphilip@live.in>")

set(PROJECT_SHORTNAME "bergamot")
set(PROJECT_LONGNAME "Bergamot")
set(PROJECT_DESCRIPTION "Bergamot provides client-side translation on the local machine.")
set(PROJECT_HOMEPAGE "https://github.com/jerinphilip/lemonade")
set(PROJECT_LICENSE "GPL")

set(IBUS_BUS_NAME "org.freedesktop.IBus.Bergamot")
set(IBUS_COMMANDLINE "")
set(IBUS_TEXTDOMAIN "bergamot-ime")
set(IBUS_ICON "/usr/share/icons/bergamot.svg")
set(IBUS_LAYOUT "us")
set(IBUS_LANGUAGE "en")


find_package(PkgConfig)
PKG_CHECK_MODULES(GLIB2 REQUIRED glib-2.0)
PKG_CHECK_MODULES(IBUS REQUIRED ibus-1.0)

add_subdirectory(3rd_party)
add_subdirectory(lemonade)
add_subdirectory(ibus)


