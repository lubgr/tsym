
cmake_minimum_required(VERSION 3.8)
project(tsym VERSION 0.2.0 DESCRIPTION "Tiny symbolic library" LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build as shared library" ON)
option(BUILD_TESTING "Compile unit tests" OFF)

SET(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Coverage Profile Sanitizer." FORCE)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.65 REQUIRED OPTIONAL_COMPONENTS unit_test_framework)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
set(tsym_installIncludeDir ${CMAKE_INSTALL_INCLUDEDIR})
set(tsym_installLibDir ${CMAKE_INSTALL_LIBDIR})

add_library(tsym-internal-config INTERFACE)

target_compile_definitions(tsym-internal-config
    INTERFACE
    $<$<OR:$<PLATFORM_ID:Windows>,$<PLATFORM_ID:Cygwin>>:_USE_MATH_DEFINES>
    $<$<PLATFORM_ID:Windows>:TSYM_ASCII_ONLY>
    $<$<CONFIG:Debug>:TSYM_WITH_DEBUG_STRINGS>)

target_include_directories(tsym-internal-config
    SYSTEM
    INTERFACE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_compile_options(tsym-internal-config
    INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -pedantic -Wconversion -Wsign-conversion -Wsign-compare -Wold-style-cast>
    $<$<CONFIG:PROFILE>:-pg>
    $<$<CONFIG:COVERAGE>:--coverage>
    $<$<CONFIG:SANITIZER>:-fsanitize=address,undefined,integer-divide-by-zero,float-divide-by-zero,float-cast-overflow,return -fno-omit-frame-pointer>)

target_link_libraries(tsym-internal-config
    INTERFACE
    $<$<CONFIG:COVERAGE>:--coverage>
    $<$<CONFIG:PROFILE>:-pg>
    $<$<CONFIG:SANITIZER>:-fsanitize=address,undefined>)
add_subdirectory(include)
add_subdirectory(src)

if(${BUILD_TESTING})
    enable_testing()
    add_subdirectory(unit-tests)
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(tsym-config-version.cmake
    VERSION ${tsym_VERSION}
    COMPATIBILITY ExactVersion)

install(FILES tsym-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/tsym-config-version.cmake
    DESTINATION ${tsym_installLibDir}/cmake/tsym)

install(EXPORT tsym-targets
    FILE tsymTargets.cmake
    NAMESPACE tsym::
    DESTINATION ${tsym_installLibDir}/cmake/tsym)

if(NOT ${BUILD_SHARED_LIBS})
    install(TARGETS tsym-internal-config
        EXPORT tsym-targets)
endif()
