# Copyright 2019 Ole Kliemann, Malte Kliemann
#
# This file is part of DrMock.
#
# DrMock is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DrMock is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DrMock.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required (VERSION 3.13)

#######################################
# Project data.
#######################################

project(
  DrMock
  VERSION 0.1.0 
  DESCRIPTION "C++17 testing and mocking framework"
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)

#######################################
# Dependencies.
#######################################

# Find libclang.
find_library(
  LIBCLANG_PATH
  NAMES clang clang-6.0 clang-7.0 clang-8.0
  PATH_SUFFIXES lib
  HINTS 
    /Library/Developer/CommandLineTools/usr
    /usr/lib/llvm-7/lib
)
if (NOT LIBCLANG_PATH)
  message(FATAL_ERROR "libclang not found.")
endif()

# Write the libclang path to the mocker.cfg.
configure_file(
  ${CMAKE_SOURCE_DIR}/python/mocker/mocker.cfg.in 
  ${CMAKE_SOURCE_DIR}/python/mocker/mocker.cfg
)

# If enabled, find Qt.
if (DEFINED ENV{DRMOCK_QT_PATH})
  find_package(Qt5 COMPONENTS Core)
  if (${Qt5_FOUND})
    # Set CMP0071 to NEW so that generated files are automoc'ed.
    cmake_policy(SET CMP0071 NEW)
    set(CMAKE_AUTOMOC ON)
  endif()
endif()

#######################################
# Source files.
#######################################

add_subdirectory(src)

#######################################
# Configure macros.
#######################################

# List of all macro .cmake files.
set(DrMockMacros
  cmake/${PROJECT_NAME}Macros.cmake
)

# Copy .cmake files into binary folder during build.
foreach (pathToFile ${${PROJECT_NAME}Macros})
  get_filename_component(name ${pathToFile} NAME)
  configure_file(
    ${pathToFile} 
    ${CMAKE_CURRENT_BINARY_DIR}/${name} 
    COPYONLY
  )
endforeach()

#######################################
# Configure install.
#######################################

# If install directory is default, install to `prefix` instead.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/prefix CACHE PATH "..." FORCE)
endif()

set(CMAKE_INSTALL_INCLUDEDIR "include/${PROJECT_NAME}")
set(CMAKE_INSTALL_LIBDIR "lib")
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Uncomment to alias the libraries on export.
set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME Core)

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
  DIRECTORY src/ 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING 
    PATTERN "*.h" 
    PATTERN "*.tpp"
)

#######################################
# Configure export.
#######################################

install(
  EXPORT 
    ${PROJECT_NAME}Targets
  FILE
    ${PROJECT_NAME}Targets.cmake
  NAMESPACE
    ${PROJECT_NAME}::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION 
    ${PROJECT_VERSION}
  COMPATIBILITY 
    SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION 
    ${INSTALL_CONFIGDIR}
)

# Install config files.
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION 
    ${INSTALL_CONFIGDIR}
)

# Install macros to the config directory.
foreach (pathToFile ${${PROJECT_NAME}Macros})
  get_filename_component(name ${pathToFile} NAME)
  install(
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/${name}
    DESTINATION 
      ${INSTALL_CONFIGDIR}
  )
endforeach()

export(
  EXPORT 
    ${PROJECT_NAME}Targets 
  FILE 
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake 
  NAMESPACE 
    ${PROJECT_NAME}::
)

export(PACKAGE ${PROJECT_NAME})

#######################################
# Testing.
#######################################

enable_testing()
add_subdirectory(tests)
