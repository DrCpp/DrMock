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
function(drmock_test)
    cmake_parse_arguments(
        ARGS
        ""
        ""
        "LIBS;TESTS;OPTIONS;RESOURCES"
        ${ARGN}
    )
    foreach (path ${ARGS_TESTS})
        # Check if `path` exists, throw otherwise.
        get_filename_component(absolute_path ${path} ABSOLUTE)
        if (NOT EXISTS ${absolute_path})
            message(FATAL_ERROR "drmock_test: error: failed to find ${path}")
        endif()

        get_filename_component(name ${path} NAME_WE)
        add_executable(${name} ${path} ${ARGS_RESOURCES})
        target_link_libraries(
            ${name}
            DrMock::DrMock
            ${ARGS_LIBS}
        )
        add_test(NAME ${name} COMMAND ${name})
        if (NOT ARGS_OPTIONS)
            target_compile_options(${name} PRIVATE -Wall -Werror -g -fPIC -pedantic -O0)
        else()
            target_compile_options(${name} PRIVATE ${ARGS_OPTIONS})
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


function(drmock_remove_file_extension)
    cmake_parse_arguments(
        ARGS
        ""
        "STRING;RESULT"
        ""
        ${ARGN}
    )
    set(_result)
    STRING(REGEX REPLACE "\\.[^.]*$" "" _result ${ARGS_STRING})
    set(${ARGS_RESULT} ${_result} PARENT_SCOPE)
endfunction()


function(drmock_join_paths)
    cmake_parse_arguments(
        ARGS
        ""
        "RESULT"
        "PATHS"
        ${ARGN}
    )

    foreach (path ${ARGS_PATHS})
        file(TO_CMAKE_PATH path ${path})
        if (NOT _result)
            set(_result ${path})
            continue()
        endif()
        set(_result "${_result}/${path}")
    endforeach()
    file(TO_NATIVE_PATH ${_result} _result)

    set(${ARGS_RESULT} ${_result} PARENT_SCOPE)
endfunction()



# drmock_path_to_output(HEADER
#                       IFILE
#                       MOCKFILE
#                       OUTPUT_HEADER
#                       OUTPUT_SOURCE)
#
# Compute output header and output source file names; write them to
# `OUTPUT_HEADER` and `OUTPUT_SOURCE`, resp.
function(drmock_path_to_output)
    cmake_parse_arguments(
        ARGS
        ""
        "HEADER;IFILE;MOCKFILE;OUTPUT_HEADER;OUTPUT_SOURCE"
        ""
        ${ARGN}
    )
    set(_output_header)
    set(_output_source)

    get_filename_component(absolute_path_to_header ${header} ABSOLUTE) # /[...]/project/[DIRS]/IExample.h
    get_filename_component(absolute_directory_of_header ${absolute_path_to_header} DIRECTORY) #/[...]/project/[DIRS]
    file(RELATIVE_PATH relative_path_from_source_dir_to_header ${CMAKE_CURRENT_SOURCE_DIR} ${absolute_directory_of_header})  # [DIRS]

    # If `relative_path_from_source_dir_to_header` is empty, set it to equal to the current
    # path. This will later allowing uncompilcated path joins.
    if ("${relative_path_from_source_dir_to_header}" EQUAL "")
        set(relative_path_from_source_dir_to_header ".")
    endif ()

    # Get the filename of `header`.
    get_filename_component(header_filename ${header} NAME)  # IExample.h
    # Remove the file extension.
    drmock_remove_file_extension(STRING ${header_filename}
                                 RESULT header_filename_without_extension)  # IExample
    # Strip the interface cast from the file name.
    STRING(REGEX REPLACE
        ${ARGS_IFILE}
        "\\1"
        captured_expression_from_filename
        ${header_filename_without_extension}
    )  # Example
    # Cast the unadorned filename onto the mock cast.
    STRING(REGEX REPLACE
        "\\\\1"
        ${captured_expression_from_filename}
        mock_filename_without_extension
        ${ARGS_MOCKFILE}
    )  # ExampleMock

    # Get the header file's file extension.
    string(REGEX MATCH "\\.([^.]*)$" header_file_extension ${header_filename})  # .h, .hpp
    # Compute the mock object's header and source file names.
    set(mock_header_filename ${mock_filename_without_extension}${header_file_extension})  # ExampleMock.h, ExampleMock.hpp
    set(mock_source_filename ${mock_filename_without_extension}.cpp)  # ExampleMock.cpp
    # Create a directory for the mock object's header and source files.
    drmock_join_paths(RESULT path_to_mock_subdirectory
                      PATHS ${CMAKE_CURRENT_BINARY_DIR}
                            DrMock/mock
                            ${relative_path_from_source_dir_to_header})
    file(MAKE_DIRECTORY ${path_to_mock_subdirectory})
    # Compute the path to the mock object's header and source files
    # relative to cmake's current binary source directory.
    drmock_join_paths(
        RESULT _output_header
        PATHS DrMock/mock ${relative_path_from_source_dir_to_header} ${mock_header_filename})
    drmock_join_paths(
        RESULT _output_source
        PATHS DrMock/mock ${relative_path_from_source_dir_to_header} ${mock_source_filename})

    set(${ARGS_OUTPUT_HEADER} ${_output_header} PARENT_SCOPE)
    set(${ARGS_OUTPUT_SOURCE} ${_output_source} PARENT_SCOPE)
endfunction()


function(drmock_library)
    cmake_parse_arguments(
        ARGS
        ""
        "TARGET;IFILE;MOCKFILE;ICLASS;MOCKCLASS;GENERATOR"
        "HEADERS;LIBS;QTMODULES;INCLUDE;FRAMEWORKS;OPTIONS"
        ${ARGN}
    )
    _drmock_required_param(ARGS_TARGET
        "drmock_library: TARGET parameter missing")
    _drmock_required_param(ARGS_HEADERS
        "drmock_library: HEADER parameter missing or empty")
    _drmock_optional_param(ARGS_ICLASS "I([a-zA-Z0-9].*)")
    _drmock_optional_param(ARGS_MOCKCLASS "\\1Mock")
    _drmock_optional_param(ARGS_IFILE "${ARGS_ICLASS}")
    _drmock_optional_param(ARGS_MOCKFILE "${ARGS_MOCKCLASS}")
    _drmock_optional_param(ARGS_GENERATOR "DrMockGenerator")

    # Make a directory for the mock object's header and source files.
    drmock_join_paths(RESULT drmock_directory
                      PATHS ${CMAKE_CURRENT_BINARY_DIR} DrMock)  # Required later.
    drmock_join_paths(RESULT mock_directory
                      PATHS drmock_directory mock)
    file(MAKE_DIRECTORY mock_directory)

    # Define a list to hold the paths of the source files.
    set(sources)

    # If Qt is enabled, add the Qt framework and include paths.
    foreach (module ${ARGS_QTMODULES})
        if (APPLE)
            drmock_get_qt5_module_framework_path(MODULE module FRAMEWORK_PATH qt_framework_path)
            list(APPEND ARGS_FRAMEWORKS ${qt_framework_path})
        endif()

        drmock_get_qt5_module_include_dirs(MODULE ${module} INCLUDE_DIRS moduleVar)
        list(APPEND ARGS_INCLUDE ${moduleVar})

        list(APPEND ARGS_LIBS ${module})
    endforeach()

    # Append the current CMake include path to the include path of the
    # mocker. Also, add CMAKE_CURRENT_SOURCE_DIR to this list.
    get_property(include_directories
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY INCLUDE_DIRECTORIES
    )
    list(APPEND ARGS_INCLUDE ${DrMock_PREFIX_PATH})
    list(APPEND ARGS_INCLUDE ${include_directories})
    list(APPEND ARGS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR})

    foreach (header ${ARGS_HEADERS})
        ###################################
        # Path computations.
        ###################################
        drmock_path_to_output(
            IFILE ${ARGS_IFILE}
            MOCKFILE ${ARGS_MOCKFILE}
            HEADER ${header}
            OUTPUT_HEADER mock_header_path
            OUTPUT_SOURCE mock_source_path
        )
        get_filename_component(absolute_path_to_header ${header} ABSOLUTE)

        # Compute the path to the mock object's header and sourcfiles
        # relative to the working directory of the mock script (at the
        # moment, this is cmake's current binary source directory.
        drmock_join_paths(
            RESULT path_from_working_dir_to_output_header
            PATHS ${CMAKE_CURRENT_BINARY_DIR} ${mock_header_path})

        drmock_options_from_list(
            OPTION "-I"
            INPUT ${ARGS_INCLUDE}
            RESULT generator_option_include_directory
        )
        if (ARGS_FRAMEWORKS)
            drmock_options_from_list(
                OPTION "-iframework"
                INPUT ${ARGS_FRAMEWORKS}
                RESULT generator_option_iframework
            )
        endif()

        ###################################
        # Calling the generator.
        ###################################

        # Call the mocker command.
        set(command)
        list(APPEND command drmock-gen)
        list(APPEND command ${absolute_path_to_header})
        list(APPEND command ${path_from_working_dir_to_output_header})
        list(APPEND command --input-class \"${ARGS_ICLASS}\")
        list(APPEND command --output-class \"${ARGS_MOCKCLASS}\")
        list(APPEND command ${generator_option_include_directory})
        list(APPEND command ${generator_option_iframework})
        list(APPEND command "--std=c++${CMAKE_CXX_STANDARD}")
        list(APPEND command ${ARGS_OPTIONS})
        add_custom_command(
            OUTPUT
                ${mock_header_path}
                ${mock_source_path}
            COMMAND ${command}
            DEPENDS ${absolute_path_to_header}
            COMMENT "Mocking ${header}..."
        )

        list(APPEND sources ${mock_source_path})
        if (ARGS_QTMODULES)
            list(APPEND sources ${header})  # Need header when using AUTO_MOC!
        endif()
    endforeach()

    add_library(${ARGS_TARGET} SHARED ${sources})
    target_include_directories(${ARGS_TARGET} PUBLIC ${drmock_directory})
    target_link_libraries(${ARGS_TARGET} DrMock::DrMock ${ARGS_LIBS})
endfunction()


# Check if optional parameter is defined, and replace it with default
# value otherwise.
function(_drmock_optional_param param default)
    if (NOT DEFINED ${param})
        set(${param} ${default} PARENT_SCOPE)
    endif()
endfunction()


# Check if required parameter is defined and raise a FATAL_ERROR with
# error message `what` otherwise.
function(_drmock_required_param param what)
    if (NOT DEFINED ${param})
        if (NOT DEFINED what)
            set(what "${param} missing")
        endif()
        message(FATAL_ERROR ${what})
    endif()
endfunction()


# drmock_options_from_list(OPTION <option>
#                          INPUT <input1>;<input2>;... 
#                          RESULT <result>)
#
# Set `result` to "<option><input1>;<option><input2>;..."
function(drmock_options_from_list)
    cmake_parse_arguments(
        ARGS
        ""
        "OPTION;RESULT"
        "INPUT"
        ${ARGN}
    )
    set(_result)
    foreach (each ${ARGS_INPUT})
        list(APPEND _result "${ARGS_OPTION}\"${each}\"")
    endforeach()
    set(${ARGS_RESULT} ${_result} PARENT_SCOPE)
endfunction()
