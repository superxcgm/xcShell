set(TEST_NAME ${PROJECT_NAME}_test)

add_executable(${TEST_NAME} test_main.cc parser_test.cc utils_test.cc build_in/alias_test.cc)

find_package(GTest REQUIRED)

target_link_libraries(${TEST_NAME} GTest::GTest xcShell_lib)
target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
add_test(${TEST_NAME} ${TEST_NAME} --gtest_output=xml:${CMAKE_SOURCE_DIR}/unit_test.xml)