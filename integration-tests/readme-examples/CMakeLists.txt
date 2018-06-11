
cmake_minimum_required(VERSION $ENV{CMAKE_MIN_VERSION})
get_filename_component(dirname ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${dirname} LANGUAGES CXX)

find_package(tsym REQUIRED)

add_executable(test
    ${CMAKE_BINARY_DIR}/main.cpp)

target_link_libraries(test
    PRIVATE
    tsym::tsym)
