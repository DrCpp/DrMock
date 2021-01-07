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

macro(DrMockEnableQt)
    cmake_policy(SET CMP0071 NEW)
    set(CMAKE_AUTOMOC ON)
endmacro()

# DrMockTest(
#   TESTS test1 [test2 [test3 ...]]
#   [LIBS lib1 [lib2 [lib3 ...]]]
#   [OPTIONS opt1 [opt2 [opt3 ...]]]
#   [RESOURCES res1 [res2 [res3 ...]]]
# )
#
# Create a test executable from every element of TESTS and link it
# against all element of LIBS.
#
# Each executable is build with compile options OPTIONS; if OPTIONS is
# undefined, then the following options are used: `-Wall`, `-Werror`,
# `-g`, `-fPIC`, `-pedantic`, `-O0`.
#
# The RESOURCES parameter may be used to include other source files
# `res1`, etc. in the executable.
function(DrMockTest)
    cmake_parse_arguments(
        PARSED_ARGS
        ""
        ""
        "LIBS;TESTS;OPTIONS;RESOURCES"
        ${ARGN}
    )
    foreach (path ${PARSED_ARGS_TESTS})
        # Check if `path` exists, throw otherwise.
        get_filename_component(absolutePath ${path} ABSOLUTE)
        if (NOT EXISTS ${absolutePath})
            message(FATAL_ERROR "DrMockTest error: Failed to find ${path}.")
        endif()

        # Register test.
        get_filename_component(name "${path}" NAME_WE)
        add_executable("${name}" "${path}" ${PARSED_ARGS_RESOURCES})
        target_link_libraries(
            "${name}"
            DrMock::Core
            ${PARSED_ARGS_LIBS}
        )
        add_test(NAME "${name}" COMMAND "${name}")
        if (NOT PARSED_ARGS_OPTIONS)
            target_compile_options("${name}" PRIVATE
                -Wall -Werror -g -fPIC -pedantic -O0
            )
        else()
            target_compile_options("${name}" PRIVATE
                ${PARSED_ARGS_OPTIONS}
            )
        endif()
    endforeach()
endfunction()

# drmock_get_qt5_module_include_dirs(MODULE <module> INCLUDE_DIRS <include_dirs>)
#
# Write list of include dirs of Qt5 module <module> to <include_dirs>.
#
# Example: drmock_get_qt5_module_include_dirs(Qt5::Core core_include_dirs)
function(drmock_get_qt5_module_include_dirs)
    cmake_parse_arguments(ARGS
        ""
        "MODULE;INCLUDE_DIRS"
        ""
        ${ARGN}
    )

    STRING(REGEX REPLACE
        "::"
        ""
        module_name
        ${ARGS_MODULE}
    )
    set(${ARGS_INCLUDE_DIRS} ${${module_name}_INCLUDE_DIRS} PARENT_SCOPE)
endfunction()


# drmock_get_qt5_module_framework_path(MODULE <module>
#                                      FRAMEWORK_PATH <framework_path>
#                                      [QT_PATH <qt_path>])
#
# Write the framework path of the Qt5 module <module> to <framework_path>.
#
# Requires the path to the Qt5 lib/ dir. Specify the path by setting <qt_path> or 
# the environment variable DRMOCK_QT_PATH.
#
# Raises: FATAL_ERROR If path to Qt5 lib/ dir is not specified.
function(drmock_get_qt5_module_framework_path)
    cmake_parse_arguments(ARGS
        ""
        "MODULE;FRAMEWORK_PATH;QT_PATH"
        ""
        ${ARGN}
    )
    if (NOT ARGS_QT_PATH)
        if (NOT DEFINED ENV{DRMOCK_QT_PATH})
            message(FATAL_ERROR "drmock_get_qt5_module_framework_path: error: no Qt5 path defined")
        endif()
        set(ARGS_QT_PATH $ENV{DRMOCK_QT_PATH})
    endif()

    string(REGEX REPLACE "\\\\" "" ARGS_QT_PATH ${ARGS_QT_PATH})
    file(TO_CMAKE_PATH ARGS_QT_PATH ${ARGS_QT_PATH})
    file(TO_NATIVE_PATH "${ARGS_QT_PATH}/lib" framework_path)
    set(${ARGS_FRAMEWORK_PATH} ${framework_path} PARENT_SCOPE)
endfunction()

# DrMockModule(
#   TARGET
#   HEADERS header1 [header2 [header3 ...]]
#   [IFILE]
#   [MOCKFILE]
#   [ICLASS]
#   [MOCKCLASS]
#   [GENERATOR]
#   [LIBS lib1 [lib2 [lib3 ...]]]
#   [QTMODULES module1 [module2 [module3 ...]]]
#   [INCLUDE include1 [include2 [include3 ...]]]
#   [FRAMEWORKS framework1 [framework2 [framework3 ...]]]
# )
#
# Create a library `TARGET` that contains mock objects for the
# specified header files.
#
# The name of the output files is determined by matching each input
# filename (with extensions removed) against `IFILE` and replacing the
# subexpression character `\\1` in `mockFile` with the content of the
# unique capture group of `IFILE`, then adding on the previously removed
# file extension. The class name of the mock object is computed in
# analogous fashion.
#
# TARGET
#   The name of the library that is created.
#
# HEADERS
#   A list of header files. Every header file must match the regex
#   provided via the `IFILE` argument.
#
# IFILE
#   A regex that describes the pattern that matches the project's
#   interface header filenames. The regex must contain exactly one
#   capture group that captures the unadorned filename. The default
#   value is ``I([a-zA-Z0-9].*)"`.
#
# MOCKFILE
#   A string that describes the pattern that the project's mock object
#   header filenames match. The string must contain exactly one
#   subexpression character `"\\1"`. The default value is `"\\1Mock"`.
#
# ICLASS
#   A regex that describes the pattern that matches the project's
#   interface class names. The regex must contain exactly one capture
#   group that captures the unadorned class name. Each of the specified
#   header files must contain exactly one class that matches this regex.
#   The default value is `IFILE`.
#
# MOCKCLASS
#   A string that describes the pattern that the project's mock object
#   class names match. The regex must contain exactly one subexpression
#   character `"\\1"`. The default value is `MOCKFILE`.
#
# GENERATOR
#   A path to the generator script of DrMock. Default value is the
#   current path.
#
# LIBS
#   A list of libraries that `TARGET` is linked against. Default value
#   is undefined (treated as empty list).
#
# QTMODULES
#   A list of Qt5 modules that `TARGET` is linked against. If
#   `QTMODULES` is defined (even if it's empty), the `HEADERS` will be
#   added to the sources of `TARGET`, thus allowing the interfaces that
#   are Q_OBJECT to be mocked. Default value is undefined.
#
# INCLUDE
#   A list of include path's that are required to parse the `HEADERS`.
#   The include paths of Qt5 modules passed in the `QTMODULES` parameter
#   are automatically added to this list.

#   The default value contains ${CMAKE_CURRENT_SOURCE_DIR} (the
#   directory that `DrMockModule` is called from) and the current
#   directory's include path.
#
# FRAMEWORKS
#   A list of macOS framework path's that are required to parse the
#   `HEADERS`. Default value is undefined (treated as empty list).
function(DrMockModule)
    cmake_parse_arguments(
        PARSED_ARGS
        ""
        "TARGET;IFILE;MOCKFILE;ICLASS;MOCKCLASS;GENERATOR;INSTALLFLAG"
        "HEADERS;LIBS;QTMODULES;INCLUDE;FRAMEWORKS;OPTIONS"
        ${ARGN}
    )

    # Check for missing arguments.
    if (NOT PARSED_ARGS_TARGET)
        message(FATAL_ERROR "DrMockModule error: TARGET parameter missing")
    endif()

    # Check if HEADERS is non-empty.
    if (NOT PARSED_ARGS_HEADERS)
        message(FATAL_ERROR "DrMockModule error: HEADER parameter missing or empty")
    endif()

    # Check that all HEADERS exist.
    foreach(filename ${PARSED_ARGS_HEADERS})
        get_filename_component(absolutePathToFilename ${filename} ABSOLUTE)
        if (NOT EXISTS ${absolutePathToFilename})
            message(FATAL_ERROR "DrMockModule error: file ${filename} not found")
        endif()
    endforeach()

    # Optional arguments.
    if (NOT PARSED_ARGS_ICLASS)
        set(PARSED_ARGS_ICLASS "I([a-zA-Z0-9].*)")
    endif()

    if (NOT PARSED_ARGS_MOCKCLASS)
        set(PARSED_ARGS_MOCKCLASS "\\1Mock")
    endif()

    if(NOT PARSED_ARGS_IFILE)
        set(PARSED_ARGS_IFILE ${PARSED_ARGS_ICLASS})
    endif()

    if (NOT PARSED_ARGS_MOCKFILE)
        set(PARSED_ARGS_MOCKFILE ${PARSED_ARGS_MOCKCLASS})
    endif()

    if (NOT PARSED_ARGS_GENERATOR)
        set(PARSED_ARGS_GENERATOR "DrMockGenerator")
    endif()

    # Define a list to hold the paths of the source files.
    set(sources)
    # Make a directory for the mock object's header and source files.
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/DrMock/mock")

    # If Qt is enabled, add the Qt framework and include paths.
    foreach (module ${PARSED_ARGS_QTMODULES})
        if (APPLE)
            drmock_get_qt5_module_framework_path(MODULE module FRAMEWORK_PATH qtFrameworkPath)
            list(APPEND PARSED_ARGS_FRAMEWORKS ${qtFrameworkPath})
        endif()

        drmock_get_qt5_module_include_dirs(MODULE ${module} INCLUDE_DIRS moduleVar)
        list(APPEND PARSED_ARGS_INCLUDE ${moduleVar})

        list(APPEND PARSED_ARGS_LIBS ${module})
    endforeach()

    # Append the current CMake include path to the include path of the
    # mocker. Also, add CMAKE_CURRENT_SOURCE_DIR to this list.
    get_property(includeDirs
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY INCLUDE_DIRECTORIES
    )
    list(APPEND PARSED_ARGS_INCLUDE ${DrMock_PREFIX_PATH})
    list(APPEND PARSED_ARGS_INCLUDE ${includeDirs})
    list(APPEND PARSED_ARGS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR})

    foreach (header ${PARSED_ARGS_HEADERS})
        ###################################
        # Path computations.
        ###################################

        # Get the relative path from the current source directory to the
        # directory containing `header`.
        get_filename_component(
            absolutePathToHeader
            ${header}
            ABSOLUTE
        )  # /[...]/project/[DIRS]/IExample.h
        get_filename_component(
            absoluteDir
            ${absolutePathToHeader}
            DIRECTORY
        )  #/[...]/project/[DIRS]
        file(RELATIVE_PATH
            relativePathToHeader
            # ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}
            "${absoluteDir}"
        )  # [DIRS]
        # If `relativePathToHeader` is empty, set it to equal to the current
        # path. This will later allowing uncompilcated path joins.
        if ("${relativePathToHeader}" EQUAL "")
            set(relativePathToHeader ".")
        endif ()

        # Get the filename of `header`.
        get_filename_component(headerFilename ${header} NAME)  # IExample.h
        # Remove the file extension.
        STRING(REGEX REPLACE
            "\\.[^.]*$"
            ""
            headerFilenameWithoutExtension
            ${headerFilename}
        )  # IExample
        # Strip the interface cast from the file name.
        STRING(REGEX REPLACE
            ${PARSED_ARGS_IFILE}
            "\\1"
            unadornedFilename
            ${headerFilenameWithoutExtension}
        )  # Example
        # Cast the unadorned filename onto the mock cast.
        STRING(REGEX REPLACE
            "\\\\1"
            ${unadornedFilename}
            mockFilenameWithoutExtension
            ${PARSED_ARGS_MOCKFILE}
        )  # ExampleMock

        # Get the header file's file extension.
        string(REGEX MATCH "\\.([^.]*)$" headerFileExtension ${headerFilename})  # .h, .hpp
        # Compute the mock object's header and source file names.
        set(mockHeaderFilename
            "${mockFilenameWithoutExtension}${headerFileExtension}"
        )  # ExampleMocke.h, ExampleMock.hpp
        set(mockSourceFilename ${mockFilenameWithoutExtension}.cpp)  # ExampleMock.cpp
        # Create a directory for the mock object's header and source files.
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/DrMock/mock/${relativePathToHeader}")
        # Compute the path to the mock object's header and source files
        # relative to cmake's current binary source directory.
        set(mockHeaderOutputPath "DrMock/mock/${relativePathToHeader}/${mockHeaderFilename}")
        set(mockSourceOutputPath "DrMock/mock/${relativePathToHeader}/${mockSourceFilename}")
        # Compute the path to the mock object's header and source files
        # relative to the working directory of the mock script (at the
        # moment, this is cmake's current binary source directory.
        set(mockHeaderPathAbsolute
            "${CMAKE_CURRENT_BINARY_DIR}/${mockHeaderOutputPath}"
        )  # [DIRS]/ExampleMock.[ext]
        set(mockSourcePathAbsolute
            "${CMAKE_CURRENT_BINARY_DIR}/${mockSourceOutputPath}"
        )  # [DIRS]/ExampleMock.cpp

        # Prepare quoted argument lists to deal with escaped characters.
        drmock_options_from_list(
            OPTION "-I"
            INPUT ${PARSED_ARGS_INCLUDE}
            RESULT generator_option_include_directory
        )
        if (PARSED_ARGS_FRAMEWORKS)
            drmock_options_from_list(
                OPTION "-iframework"
                INPUT ${PARSED_ARGS_FRAMEWORKS}
                RESULT generator_option_iframework
            )
        endif()

        ###################################
        # Calling the generator.
        ###################################

        # Call the mocker command.
        set(command)
        list(APPEND command drmock-gen)
        list(APPEND command ${absolutePathToHeader})
        list(APPEND command ${mockHeaderPathAbsolute})
        list(APPEND command --input-class \"${PARSED_ARGS_ICLASS}\")
        list(APPEND command --output-class \"${PARSED_ARGS_MOCKCLASS}\")
        list(APPEND command ${generator_option_include_directory})
        list(APPEND command ${generator_option_iframework})
        list(APPEND command "--std=c++${CMAKE_CXX_STANDARD}")
        list(APPEND command ${PARSED_ARGS_OPTIONS})
        add_custom_command(
            OUTPUT
                ${mockHeaderOutputPath}
                ${mockSourceOutputPath}
            COMMAND ${command}
            DEPENDS ${absolutePathToHeader}
            COMMENT "Mocking ${header}..."
        )

        # Add the source path to the list of sources.
        set(sources ${sources} ${mockSourceOutputPath})
        if (PARSED_ARGS_QTMODULES)
            set(sources ${sources} ${header})
        endif()
    endforeach()

    # Create the mock library and link it against DrMock.
    add_library(${PARSED_ARGS_TARGET} SHARED ${sources})
    # Include the headers in the mock directory.
    target_include_directories(
        ${PARSED_ARGS_TARGET}
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/DrMock
    )
    # Link against DrMock and the other provided libs.
    target_link_libraries(${PARSED_ARGS_TARGET} DrMock::Core ${PARSED_ARGS_LIBS})
endfunction()

# drmock_options_from_list(OPTION <option>
#                          INPUT <input1> <input2> ... 
#                          RESULT <result>)
#
# Set `result` to be equal to "<option><input1>;<option><input2>;..."
function(drmock_options_from_list)
    cmake_parse_arguments(
        ARGS
        ""
        "OPTION;RESULT"
        "INPUT"
        ${ARGN}
    )
    set(result)
    foreach (each ${ARGS_INPUT})
        list(APPEND result "${ARGS_OPTION}\"${each}\"")
    endforeach()
    set(${ARGS_RESULT} ${result} PARENT_SCOPE)
endfunction()
