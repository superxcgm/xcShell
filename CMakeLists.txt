cmake_minimum_required(VERSION 3.10)
project(xcShell)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

add_subdirectory(src)
enable_testing()
add_subdirectory(test)

add_custom_target(lint COMMAND cpplint --recursive ../)
add_custom_target(format COMMAND find ../ -iname *.h -o -iname *.cpp | xargs clang-format -i -style=file)
