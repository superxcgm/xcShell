SET(LIB_NAME xcShell_lib)
add_library(${LIB_NAME}
        command_executor.cc
        error_handling.cc
        parser.cc
        shell.cc
        utils.cc
        build_in/alias.cc
        build_in/build_in.cc
        build_in/cd.cc
        build_in/j.cc)
find_package(readline REQUIRED)
find_package(spdlog REQUIRED)
find_package(CLI11 REQUIRED)
target_link_libraries(${LIB_NAME} PUBLIC readline::readline spdlog::spdlog CLI11::CLI11)
target_include_directories(${LIB_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} main.cc)
target_link_libraries(${PROJECT_NAME} ${LIB_NAME})

install(TARGETS ${PROJECT_NAME}
        DESTINATION bin)

install (FILES ${PROJECT_SOURCE_DIR}/tools/global_config_default.txt
        DESTINATION etc RENAME xcshrc)

# todo install config