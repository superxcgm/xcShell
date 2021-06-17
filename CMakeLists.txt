cmake_minimum_required(VERSION 3.17)
project(cpp-foo)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
aux_source_directory(src EXE_SOURCES)
list(FILTER EXE_SOURCES INCLUDE REGEX "^.*.cc$")
add_executable(${PROJECT_NAME} ${EXE_SOURCES})

aux_source_directory(test TEST_SOURCES)
list(FILTER TEST_SOURCES INCLUDE REGEX "^.*_test.cc$")

if (NOT "${TEST_SOURCES}" STREQUAL "")
    list(FILTER EXE_SOURCES EXCLUDE REGEX "main.cc")
    add_executable(${PROJECT_NAME}_test ${EXE_SOURCES} ${TEST_SOURCES} test/test_main.cc)
    find_package(GTest REQUIRED)
    target_link_libraries(${PROJECT_NAME}_test GTest::GTest)
endif ()

add_custom_target(lint COMMAND cpplint --recursive ../)

add_custom_target(format COMMAND find ../ -iname *.h -o -iname *.cpp | xargs clang-format -i -style=file)

add_custom_target(test COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_test)