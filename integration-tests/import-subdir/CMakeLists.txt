
cmake_minimum_required(VERSION $ENV{CMAKE_MIN_VERSION})
get_filename_component(dirname ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${dirname} LANGUAGES CXX)

add_subdirectory(${TSYM_SUBDIR})

add_executable(tsym-test
    main.cpp)

target_link_libraries(tsym-test
    PRIVATE
    tsym::tsym)
