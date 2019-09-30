
string(TIMESTAMP tsym_CONFIGURE_TIME "%Y-%B-%d")

configure_file(version.cpp.in version.cpp)

set_source_files_properties(version.cpp
    PROPERTIES
    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

add_library(tsym
    base.cpp
    basefct.cpp
    baseptr.cpp
    baseptrlist.cpp
    baseptrlistfct.cpp
    basetypestr.cpp
    cache.cpp
    constant.cpp
    constants.cpp
    directsolve.cpp
    fraction.cpp
    function.cpp
    functions.cpp
    gcd.cpp
    int.cpp
    logarithm.cpp
    logger.cpp
    name.cpp
    namefct.cpp
    number.cpp
    numberfct.cpp
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
    solve.cpp
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
    cxx_std_17)

target_include_directories(tsym
    INTERFACE
    $<BUILD_INTERFACE:${tsym_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${tsym_installIncludeDir}>)

add_library(tsym::tsym ALIAS tsym)

install(TARGETS tsym
    EXPORT tsym-targets
    LIBRARY DESTINATION ${tsym_installLibDir} COMPONENT lib
    ARCHIVE DESTINATION ${tsym_installLibDir} COMPONENT lib)
