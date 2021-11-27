cmake_minimum_required(VERSION 3.10)
project(xcShell)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
aux_source_directory(src EXE_SOURCES)
aux_source_directory(src/build_in BUILD_IN_SOURCES)
list(APPEND EXE_SOURCES ${BUILD_IN_SOURCES})

list(FILTER EXE_SOURCES INCLUDE REGEX "^.*.cc$")
add_executable(${PROJECT_NAME} ${EXE_SOURCES})

aux_source_directory(test TEST_SOURCES)
aux_source_directory(test/build_in BUILD_IN_TEST_SOURCES)
list(APPEND TEST_SOURCES ${BUILD_IN_TEST_SOURCES})
list(FILTER TEST_SOURCES INCLUDE REGEX "^.*_test.cc$")

find_package(readline REQUIRED)
find_package(spdlog REQUIRED)
find_package(CLI11 REQUIRED)
target_link_libraries(${PROJECT_NAME} readline::readline spdlog::spdlog CLI11::CLI11)

if (NOT "${TEST_SOURCES}" STREQUAL "")
    enable_testing()
    list(FILTER EXE_SOURCES EXCLUDE REGEX "main.cc")
    add_executable(${PROJECT_NAME}_test ${EXE_SOURCES} ${TEST_SOURCES} test/test_main.cc)
    find_package(GTest REQUIRED)
    target_link_libraries(${PROJECT_NAME}_test GTest::GTest readline::readline spdlog::spdlog CLI11::CLI11)
    add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test --gtest_output=xml:${CMAKE_SOURCE_DIR}/unit_test.xml)
endif ()

add_custom_target(lint COMMAND cpplint --recursive ../)

add_custom_target(format COMMAND find ../ -iname *.h -o -iname *.cpp | xargs clang-format -i -style=file)
