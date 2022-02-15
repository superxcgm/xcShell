cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(xcShell LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
            "MinSizeRel" "RelWithDebInfo")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions("IS_LINUX")
    execute_process(COMMAND "conan" "install" ".." "-s" "compiler.libcxx=libstdc++11"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_compile_definitions("IS_MACOS")
    execute_process(COMMAND "conan" "install" ".."
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

add_subdirectory(src)
enable_testing()
add_subdirectory(test)

add_custom_target(lint COMMAND cpplint --recursive ../)
add_custom_target(format COMMAND find ../ -iname *.h -o -iname *.cpp | xargs clang-format -i -style=file)
