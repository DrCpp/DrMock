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

set(_DRMOCK_FILE_REGEX_DEFAULT_INPUT "I([a-zA-Z0-9].*)")
set(_DRMOCK_FILE_REGEX_DEFAULT_OUTPUT "\\1Mock")


macro(drmock_enable_qt)
    cmake_policy(SET CMP0071 NEW)
    set(CMAKE_AUTOMOC ON)
endmacro()


# drmock_test(TESTS test1 [test2 [test3 ...]]
#             [LIBS lib1 [lib2 [lib3 ...]]]
#             [OPTIONS opt1 [opt2 [opt3 ...]]]
#             [RESOURCES res1 [res2 [res3 ...]]]
# )
#
# Create a test executable from every element of TESTS and link it
# against all element of LIBS.
#
# Each executable is build with compile options OPTIONS.
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
    _drmock_required_param(ARGS_TESTS
        "drmock_test: TESTS parameter missing or empty")
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
            target_compile_options(${name} PRIVATE ${ARGS_OPTIONS})
        endif()
    endforeach()
endfunction()


# drmock_library(TARGET <target>
#                HEADERS header1 [header2 ...]
#                [IFILE <ifile>]
#                [MOCKFILE <mock_file>]
#                [ICLASS <iclass>]
#                [MOCKCLASS <mockclass>]
#                [LIBS lib1 [lib2 ...]]
#                [QTMODULES module1 [module2 ...]]
#                [INCLUDE include1 [include2 ...]]
#                [FRAMEWORKS framework1 [framework2 ...]]
#                [OPTIONS option1 [option2 ...]]
#
# Create a library `target` that contains mock objects for the
# specified header files.
#
# The name of the output files is determined by matching each input
# filename (with extensions removed) against <ifile> and replacing the
# subexpression character `\\1` in <mockfile> with the content of the
# unique capture group of <ifile>, then adding on the previously removed
# file extension. The class name of the mock object is computed in
# analogous fashion.
#
# Note that since `drmock_library` is implemented using
# `drmock_library2`, it may raise errors labled `drmock_library2`.
#
# TARGET
#     The name of the library that is created.
#
# HEADERS
#     A list of header files. Every header file must match the regex
#     provided via the <ifile> argument.
#
# IFILE
#     A regex that describes the pattern that matches the project's
#     interface header filenames. The regex must contain exactly one
#     capture group that captures the unadorned filename. The default
#     value is ``I([a-zA-Z0-9].*)"`.
#
# MOCKFILE
#     A string that describes the pattern that the project's mock object
#     header filenames match. The string must contain exactly one
#     subexpression character `"\\1"`. The default value is `"\\1Mock"`.
#
# ICLASS
#     A regex that describes the pattern that matches the project's
#     interface class names. The regex must contain exactly one capture
#     group that captures the unadorned class name. Each of the specified
#     header files must contain exactly one class that matches this regex.
#     The default value is <ifile>.
#
# MOCKCLASS
#     A string that describes the pattern that the project's mock object
#     class names match. The regex must contain exactly one subexpression
#     character `"\\1"`. The default value is <mockfile>.
#
# LIBS
#     A list of libraries that `TARGET` is linked against. Default value
#     is undefined (treated as empty list).
#
# QTMODULES
#     A list of Qt5 modules that `TARGET` is linked against. If
#     `QTMODULES` is defined (even if it's empty), the `HEADERS` will be
#     added to the sources of `TARGET`, thus allowing the interfaces
#     that are Q_OBJECT to be mocked. Default value is undefined.
#
# INCLUDE
#     A list of include path's that are required to parse the `HEADERS`.
#     The include paths of Qt5 modules passed in the `QTMODULES`
#     parameter are automatically added to this list.
#
#     The default value contains ${CMAKE_CURRENT_SOURCE_DIR} (the
#     directory that `DrMockModule` is called from) and the current
#     directory's include path.
#
# FRAMEWORKS
#     A list of macOS framework path's that are required to parse the
#     `HEADERS`. Default value is undefined (treated as empty list).
#
# OPTIONS
#     A list of additional options passed to `drmock-generator`.

function(drmock_library)
    cmake_parse_arguments(
        ARGS
        ""
        "TARGET;IFILE;MOCKFILE;ICLASS;MOCKCLASS"
        "HEADERS;LIBS;QTMODULES;INCLUDE;FRAMEWORKS;OPTIONS;FLAGS"
        ${ARGN}
    )
    _drmock_required_param(ARGS_TARGET
        "drmock_library: TARGET parameter missing")
    _drmock_required_param(ARGS_HEADERS
        "drmock_library: HEADER parameter missing or empty")
    _drmock_optional_param(ARGS_ICLASS ${_DRMOCK_FILE_REGEX_DEFAULT_INPUT})
    _drmock_optional_param(ARGS_MOCKCLASS ${_DRMOCK_FILE_REGEX_DEFAULT_OUTPUT})
    _drmock_optional_param(ARGS_IFILE "${ARGS_ICLASS}")
    _drmock_optional_param(ARGS_MOCKFILE "${ARGS_MOCKCLASS}")

    set(mock_header_paths)
    set(mock_source_paths)
    set(input_classes)
    set(output_classes)
    foreach (header ${ARGS_HEADERS})
        _drmock_capture_filenames(
            IFILE ${ARGS_IFILE}
            MOCKFILE ${ARGS_MOCKFILE}
            HEADER ${header}
            OUTPUT_HEADER mock_header_file
            OUTPUT_SOURCE mock_source_file)
        _drmock_compute_path_from_filename(
            HEADER ${header}
            FILENAME ${mock_header_file}
            PATH mock_header_path)
        _drmock_compute_path_from_filename(
            HEADER ${header}
            FILENAME ${mock_source_file}
            PATH mock_source_path)
        list(APPEND mock_header_paths ${mock_header_path})
        list(APPEND mock_source_paths ${mock_source_path})
        # We just put copies of the regex's into the input/output class
        # lists. This is slightly awkward.
        list(APPEND input_classes ${ARGS_ICLASS})
        list(APPEND output_classes ${ARGS_MOCKCLASS})
    endforeach()

    drmock_library2(TARGET ${ARGS_TARGET}
                    HEADERS ${ARGS_HEADERS}
                    MOCK_HEADER_PATHS ${mock_header_paths}
                    MOCK_SOURCE_PATHS ${mock_source_paths}
                    INPUT_CLASSES ${input_classes}
                    OUTPUT_CLASSES ${output_classes}
                    LIBS ${ARGS_LIBS}
                    QTMODULES ${ARGS_QTMODULES}
                    INCLUDE ${ARGS_INCLUDE}
                    FRAMEWORKS ${ARGS_FRAMEWORKS}
                    OPTIONS ${ARGS_OPTIONS}
                    FLAGS ${ARGS_FLAGS})
endfunction()


# drmock_library2(TARGET <target>
#                 HEADERS header1 [header2 ...]
#                 MOCK_HEADER_PATHS header_path1 [header_path2 ...]
#                 MOCK_SOURCE_PATHS source_path1 [source_path2 ...]
#                 [INPUT_CLASSES input_class1 [input_class2 ...]]
#                 [OUTPUT_CLASSES output_class1 [output_class2 ...]]
#                 [LIBS lib1 [lib2 ...]]
#                 [QTMODULES module1 [module2 ...]]
#                 [INCLUDE include1 [include2 ...]]
#                 [FRAMEWORKS framework1 [framework2 ...]]
#                 [OPTIONS option1 [option2 ...]]
#
# Create a library `target` that contains mock objects for the
# specified header files.
#
# Unlinke `drmock_library`, we use concrete paths here. The lists
# `MOCK_HEADER_PATHS` and `MOCK_SOURCE_PATHS` hold the paths to where
# mock class data is written. They must both have the same length as
# HEADERS and the data from these three lists are matched against each
# other by index, i.e. the first entry of HEADER is the path to the
# first input class, the first entry of MOCK_HEADER_PATH and
# MOCK_SOURCE_PATH is the path to the header resp. source of the
# corresponding mock class.
#
# The lists `INPUT_CLASSES` and `OUTPUT_CLASSES` are lists of regex's or
# strings against which the input classes (mocked classes) and output
# classes (mock classes) are matched, just like in `drmock_library`. The
# lists must be either empty, in which case the default values specified
# in the documentation of `drmock_library` are used, or have the same
# length, in which case they are matched against the list HEADER,
# MOCK_HEADER_PATH and MOCK_SOURCE_PATH by index.
#
# See the documentation of `drmock_library` for details regarding the
# other parameters.
function(drmock_library2)
    cmake_parse_arguments(
        ARGS
        ""
        "TARGET"
        "HEADERS;MOCK_HEADER_PATHS;MOCK_SOURCE_PATHS;INPUT_CLASSES;OUTPUT_CLASSES;LIBS;QTMODULES;INCLUDE;FRAMEWORKS;OPTIONS;FLAGS"
        ${ARGN}
    )
    _drmock_required_param(ARGS_TARGET
        "drmock_library2: TARGET parameter missing")

    # Verify that lengths are correct.
    list(LENGTH ARGS_HEADERS len_headers)
    list(LENGTH ARGS_MOCK_HEADER_PATHS len_mock_header_paths)
    list(LENGTH ARGS_MOCK_SOURCE_PATHS len_mock_source_paths)
    if ((NOT "${len_headers}" EQUAL "${len_mock_header_paths}")
     OR (NOT "${len_headers}" EQUAL "${len_mock_source_paths}"))
        message(FATAL_ERROR "drmock_library2: HEADERS, MOCK_HEADER_PATHS, MOCK_SOURCE_PATHS lists are not the same length")
    endif()

    # Default values on empty input/output class lists.
    list(LENGTH ARGS_INPUT_CLASSES len_input_classes)
    if (${len_input_classes} EQUAL 0)
        foreach (_ RANGE 1 ${len_headers})
            list(APPEND ARGS_INPUT_CLASSES ${_DRMOCK_FILE_REGEX_DEFAULT_INPUT})
        endforeach()
    endif()
    list(LENGTH ARGS_OUTPUT_CLASSES len_output_classes)
    if (${len_output_classes} EQUAL 0)
        foreach (_ RANGE 1 ${len_headers})
            list(APPEND ARGS_OUTPUT_CLASSES ${_DRMOCK_FILE_REGEX_DEFAULT_OUTPUT})
        endforeach()
    endif()

    # Define a list to hold the paths of the source files.
    set(sources)

    # If Qt is enabled, add the Qt framework and include paths.
    foreach (module ${ARGS_QTMODULES})
        if (APPLE)
            _drmock_get_qt5_module_framework_path(
                MODULE module
                FRAMEWORK_PATH qt_framework_path
            )
            list(APPEND ARGS_FRAMEWORKS ${qt_framework_path})
        endif()

        _drmock_get_qt5_module_include_dirs(MODULE ${module} INCLUDE_DIRS moduleVar)
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

    # Compute some options for the generator call (--iframework,
    # --clang-library-path)
    _drmock_options_from_list(
        OPTION "-I"
        INPUT ${ARGS_INCLUDE}
        RESULT generator_option_include_directory
    )
    if (ARGS_FRAMEWORKS)
        _drmock_options_from_list(
            OPTION "-iframework"
            INPUT ${ARGS_FRAMEWORKS}
            RESULT generator_option_iframework
        )
    endif()
    if (DEFINED ENV{CLANG_LIBRARY_FILE})
        set(DRMOCK_LIBCLANG_PATH $ENV{CLANG_LIBRARY_FILE})
    else()
        # If no environment variable is available, try to find
        # libclang via CMake.
        find_library(
            DRMOCK_LIBCLANG_PATH
            NAMES clang clang-6.0 clang-7.0 clang-8.0
            PATH_SUFFIXES lib
            HINTS
                /Library/Developer/CommandLineTools/usr
                /usr/lib/llvm-7/lib
        )
    endif()

    foreach (header mock_header_path mock_source_path input_class output_class
             IN ZIP_LISTS ARGS_HEADERS ARGS_MOCK_HEADER_PATHS ARGS_MOCK_SOURCE_PATHS
             ARGS_INPUT_CLASSES ARGS_OUTPUT_CLASSES)
        get_filename_component(absolute_path_to_header ${header} ABSOLUTE)

        # Compute the path to the mock object's header and sourcfiles
        # relative to the working directory of the mock script (at the
        # moment, this is cmake's current binary source directory.
        _drmock_join_paths(
            RESULT path_from_working_dir_to_output_header
            PATHS ${CMAKE_CURRENT_BINARY_DIR} ${mock_header_path})

        # Setup drmock-generator arguments.
        set(command)
        list(APPEND command drmock-generator)
        list(APPEND command ${absolute_path_to_header})
        list(APPEND command ${path_from_working_dir_to_output_header})
        list(APPEND command --input-class \"${input_class}\")
        list(APPEND command --output-class \"${output_class}\")
        list(APPEND command --clang-library-file ${DRMOCK_LIBCLANG_PATH})
        list(APPEND command ${ARGS_OPTIONS})
        # Note: Compiler flags _must_ follow the ``--flags`` argument!
        list(APPEND command --flags)
        list(APPEND command "--std=c++${CMAKE_CXX_STANDARD}")
        list(APPEND command ${ARGS_FLAGS})
        list(APPEND command ${generator_option_include_directory})
        list(APPEND command ${generator_option_iframework})
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
    _drmock_join_paths(RESULT drmock_directory
                       PATHS ${CMAKE_CURRENT_BINARY_DIR} DrMock)  # Required later.
    target_include_directories(${ARGS_TARGET} PUBLIC ${drmock_directory})
    target_link_libraries(${ARGS_TARGET} DrMock::DrMock ${ARGS_LIBS})
endfunction()


# _drmock_get_qt5_module_include_dirs(MODULE <module> INCLUDE_DIRS <include_dirs>)
#
# Write list of include dirs of Qt5 module <module> to <include_dirs>.
#
# Example:
#     _drmock_get_qt5_module_include_dirs(Qt5::Core core_include_dirs)
function(_drmock_get_qt5_module_include_dirs)
    cmake_parse_arguments(ARGS
        ""
        "MODULE;INCLUDE_DIRS"
        ""
        ${ARGN}
    )
    _drmock_required_param(ARGS_MODULE
        "_drmock_get_qt5_module_include_dirs: MODULE parameter missing or empty")
    _drmock_required_param(ARGS_INCLUDE_DIRS
        "_drmock_get_qt5_module_include_dirs: INCLUDE_DIRS parameter missing or empty")

    STRING(REGEX REPLACE
        "::"
        ""
        module_name
        ${ARGS_MODULE}
    )
    set(${ARGS_INCLUDE_DIRS} ${${module_name}_INCLUDE_DIRS} PARENT_SCOPE)
endfunction()


# _drmock_get_qt5_module_framework_path(MODULE <module>
#                                       FRAMEWORK_PATH <framework_path>
#                                       [QT_PATH <qt_path>])
#
# Write the framework path of the Qt5 module <module> to
# <framework_path>.
#
# Requires the path to the Qt5 lib/ dir. Specify the path by setting
# <qt_path> or the environment variable DRMOCK_QT_PATH.
#
# Raises: FATAL_ERROR If path to Qt5 lib/ dir is not specified.
function(_drmock_get_qt5_module_framework_path)
    cmake_parse_arguments(ARGS
        ""
        "MODULE;FRAMEWORK_PATH;QT_PATH"
        ""
        ${ARGN}
    )
    _drmock_required_param(ARGS_MODULE
        "_drmock_get_qt5_module_framework_path: MODULE parameter missing or empty")
    _drmock_required_param(ARGS_FRAMEWORK_PATH
        "_drmock_get_qt5_module_framework_path: FRAMEWORK_PATH parameter missing or empty")

    if (NOT ARGS_QT_PATH)
        if (NOT DEFINED ENV{DRMOCK_QT_PATH})
            message(FATAL_ERROR "_drmock_get_qt5_module_framework_path: error: no Qt5 path defined")
        endif()
        set(ARGS_QT_PATH $ENV{DRMOCK_QT_PATH})
    endif()

    string(REGEX REPLACE "\\\\" "" ARGS_QT_PATH ${ARGS_QT_PATH})
    file(TO_CMAKE_PATH ARGS_QT_PATH ${ARGS_QT_PATH})
    file(TO_NATIVE_PATH "${ARGS_QT_PATH}/lib" framework_path)

    set(${ARGS_FRAMEWORK_PATH} ${framework_path} PARENT_SCOPE)
endfunction()


# _drmock_remove_file_extension(RESULT <result> STRING <string>)
#
# Remove the file extension from <string> and write the result into
# <result>.
function(_drmock_remove_file_extension)
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


# _drmock_join_paths(RESULT <result> PATHS <path1>;<path2>;...)
#
# Join native/UNIX paths <path1>... and write the result into <result>
# (paths may be mixed).
function(_drmock_join_paths)
    cmake_parse_arguments(
        ARGS
        ""
        "RESULT"
        "PATHS"
        ${ARGN}
    )
    _drmock_required_param(ARGS_RESULT
        "_drmock_join_paths: RESULT parameter missing or empty")
    _drmock_required_param(ARGS_PATHS
        "_drmock_join_paths: PATHS parameter missing or empty")

    foreach (path ${ARGS_PATHS})
        file(TO_CMAKE_PATH path ${path})
        if (NOT _result)
            set(_result ${path})
            continue()
        endif()
        set(_result "${_result}/${path}")
    endforeach()
    # file(TO_NATIVE_PATH ${_result} _result)

    set(${ARGS_RESULT} ${_result} PARENT_SCOPE)
endfunction()


# _drmock_capture_filenames(HEADER <header>
#                           IFILE <ifile>
#                           MOCKFILE <mockfile>
#                           OUTPUT_HEADER <output_header>
#                           OUTPUT_SOURCE <output_source>)
#
# Compute output header and output source filenames; write them to
# <output_header> and <output_source>, resp.
function(_drmock_capture_filenames)
    cmake_parse_arguments(
        ARGS
        ""
        "HEADER;IFILE;MOCKFILE;OUTPUT_HEADER;OUTPUT_SOURCE"
        ""
        ${ARGN}
    )
    _drmock_required_param(ARGS_HEADER
        "_drmock_capture_filenames: HEADER parameter missing or empty")
    _drmock_required_param(ARGS_IFILE
        "_drmock_capture_filenames: IFILE parameter missing or empty")
    _drmock_required_param(ARGS_MOCKFILE
        "_drmock_capture_filenames: MOCKFILE parameter missing or empty")
    _drmock_required_param(ARGS_OUTPUT_HEADER
        "_drmock_capture_filenames: OUTPUT_HEADER parameter missing or empty")
    _drmock_required_param(ARGS_OUTPUT_SOURCE
        "_drmock_capture_filenames: OUTPUT_SOURCE parameter missing or empty")

    # Get the filename of `header`.
    get_filename_component(header_filename ${ARGS_HEADER} NAME)  # IExample.h
    # Remove the file extension.
    _drmock_remove_file_extension(STRING ${header_filename}
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

    set(${ARGS_OUTPUT_HEADER} ${mock_header_filename} PARENT_SCOPE)
    set(${ARGS_OUTPUT_SOURCE} ${mock_source_filename} PARENT_SCOPE)
endfunction()

# _drmock_compute_path_from_filename(HEADER <header>
#                                    FILENAME <filename>
#                                    PATH <path>)
#
# Compute mock output path from filename and create a directory for the
# mock files; write the path to <path>.
function(_drmock_compute_path_from_filename)
    cmake_parse_arguments(
        ARGS
        ""
        "HEADER;FILENAME;PATH"
        ""
        ${ARGN}
    )
    _drmock_required_param(ARGS_HEADER
        "_drmock_compute_path_from_filename: HEADER parameter missing")
    _drmock_required_param(ARGS_FILENAME
        "_drmock_compute_path_from_filename: FILENAME parameter missing")
    _drmock_required_param(ARGS_PATH
        "_drmock_compute_path_from_filename: PATH parameter missing")

    get_filename_component(absolute_path_to_header ${ARGS_HEADER} ABSOLUTE)  # /[...]/project/[DIRS]/IExample.h
    get_filename_component(absolute_directory_of_header ${absolute_path_to_header} DIRECTORY)  #/[...]/project/[DIRS]
    file(RELATIVE_PATH relative_path_from_source_dir_to_header ${CMAKE_CURRENT_SOURCE_DIR} ${absolute_directory_of_header})  # [DIRS]

    # Create a directory for the mock object's header and source files.
    _drmock_join_paths(RESULT path_to_mock_subdirectory
                       PATHS ${CMAKE_CURRENT_BINARY_DIR}
                             DrMock/mock
                             ${relative_path_from_source_dir_to_header})
    file(MAKE_DIRECTORY ${path_to_mock_subdirectory})
    # Compute the path to the mock object's header and source files
    # relative to cmake's current binary source directory.
    _drmock_join_paths(
        RESULT _path
        PATHS DrMock/mock ${relative_path_from_source_dir_to_header} ${ARGS_FILENAME})

    set(${ARGS_PATH} ${_path} PARENT_SCOPE)
endfunction()


# _drmock_options_from_list(OPTION <option>
#                           INPUT <input1>;<input2>;... 
#                           RESULT <result>)
#
# Set `result` to "<option><input1>;<option><input2>;..."
function(_drmock_options_from_list)
    cmake_parse_arguments(
        ARGS
        ""
        "OPTION;RESULT"
        "INPUT"
        ${ARGN}
    )
    _drmock_required_param(ARGS_OPTION
        "_drmock_options_from_list: OPTION parameter missing")
    _drmock_required_param(ARGS_RESULT
        "_drmock_options_from_list: RESULT parameter missing")

    set(_result)
    foreach (each ${ARGS_INPUT})
        list(APPEND _result "${ARGS_OPTION}\"${each}\"")
    endforeach()
    set(${ARGS_RESULT} ${_result} PARENT_SCOPE)
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
