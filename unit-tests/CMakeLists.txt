
add_executable(tests EXCLUDE_FROM_ALL
    boostmatrixvector.cpp
    fixtures.cpp
    main.cpp
    testbaseptrlistfct.cpp
    testcoeff.cpp
    testcomparison.cpp
    testcomplexity.cpp
    testconstant.cpp
    testdegree.cpp
    testdiff.cpp
    testexpansion.cpp
    testfraction.cpp
    testgcd.cpp
    testhas.cpp
    testhash.cpp
    testint.cpp
    testlogarithm.cpp
    testludecomposition.cpp
    testname.cpp
    testnormal.cpp
    testnumber.cpp
    testnumeric.cpp
    testnumpowersimpl.cpp
    testnumtrigosimpl.cpp
    testorder.cpp
    testparser.cpp
    testpivoting.cpp
    testplu.cpp
    testpolycontentunit.cpp
    testpolydivide.cpp
    testpolyinfo.cpp
    testpolymindegree.cpp
    testpower.cpp
    testpowernormal.cpp
    testprimefac.cpp
    testprinter.cpp
    testproduct.cpp
    testsign.cpp
    testsimpleprimepolicy.cpp
    testsubst.cpp
    testsuitelogger.cpp
    testsum.cpp
    testsymbol.cpp
    testsymbolmap.cpp
    testtrigonometric.cpp
    testundefined.cpp
    testvar.cpp
    testvarfct.cpp
    tsymtests.cpp)

target_include_directories(tests
    PRIVATE
    ${tsym_SOURCE_DIR}/src)

target_link_libraries(tests
    PRIVATE
    tsym tsym-internal-config Boost::unit_test_framework)

add_test(NAME tsym.unittests COMMAND tests)
