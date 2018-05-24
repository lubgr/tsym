
cmake_minimum_required(VERSION 3.8)
project(tsym VERSION 0.2.0 DESCRIPTION "Tiny symbolic library" LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build as shared library" ON)
option(BUILD_TESTING "Compile unit tests" ON)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.65 REQUIRED OPTIONAL_COMPONENTS unit_test_framework)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
set(tsym_installIncludeDir ${CMAKE_INSTALL_INCLUDEDIR})
set(tsym_installLibDir ${CMAKE_INSTALL_LIBDIR})

add_library(tsym-internal-config INTERFACE)

target_compile_definitions(tsym-internal-config
    INTERFACE
    $<$<CONFIG:Debug>:TSYM_WITH_DEBUG_STRINGS>)

target_include_directories(tsym-internal-config
    SYSTEM
    INTERFACE
    ${Boost_INCLUDE_DIRS})

target_compile_features(tsym-internal-config
    INTERFACE
    cxx_std_14)

target_compile_options(tsym-internal-config
    INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -pedantic -Wconversion -Wsign-conversion>)

add_subdirectory(include)
add_subdirectory(src)

if(${BUILD_TESTING})
    enable_testing()
    add_subdirectory(test)
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
