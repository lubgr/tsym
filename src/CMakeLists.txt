
string(TIMESTAMP tsym_CONFIGURE_TIME "%Y-%B-%d %H:%M")

configure_file(version.cpp.in version.cpp)

set_source_files_properties(version.cpp
    PROPERTIES
    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

add_library(tsym
    base.cpp
    baseptr.cpp
    baseptrlist.cpp
    bplist.cpp
    cache.cpp
    constant.cpp
    fraction.cpp
    function.cpp
    gcd.cpp
    globals.cpp
    integer.cpp
    logarithm.cpp
    logger.cpp
    name.cpp
    number.cpp
    numeric.cpp
    numpowersimpl.cpp
    numtrigosimpl.cpp
    options.cpp
    order.cpp
    parser.cpp
    plaintextprintengine.cpp
    poly.cpp
    polyinfo.cpp
    power.cpp
    powernormal.cpp
    powersimpl.cpp
    primefac.cpp
    primitivegcd.cpp
    printer.cpp
    product.cpp
    productsimpl.cpp
    subresultantgcd.cpp
    sum.cpp
    sumsimpl.cpp
    symbol.cpp
    symbolmap.cpp
    trigonometric.cpp
    undefined.cpp
    var.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

target_link_libraries(tsym PRIVATE tsym-internal-config)

target_compile_features(tsym
    PUBLIC
    cxx_std_14)

target_include_directories(tsym
    INTERFACE
    $<BUILD_INTERFACE:${tsym_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${tsym_installIncludeDir}>)

add_library(tsym::tsym ALIAS tsym)

install(TARGETS tsym
    EXPORT tsym-targets
    LIBRARY DESTINATION ${tsym_installLibDir} COMPONENT lib
    ARCHIVE DESTINATION ${tsym_installLibDir} COMPONENT lib)
