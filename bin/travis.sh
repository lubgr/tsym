#!/usr/bin/env bash

TESTEXEC="${PWD}/tests"
CMAKE_BUILD_TYPE="Release"
EXIT=0

build() {
    CXX=$1
    buildDir="build-${MODE}-${CXX}"

    mkdir -p "${buildDir}"
    cd "${buildDir}"
    cmake \
        -D CMAKE_CXX_COMPILER="${CXX}"\
        -D CMAKE_CXX_FLAGS="${CXXFLAGS}"\
        -D CMAKE_EXE_LINKER_FLAGS="${LDFLAGS}"\
        -D CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"\
        -D BOOST_ROOT="${BOOSTDIR}"\
        ..
    make tsym tests
    SUCCESS=$?

    cp "test/tests" ${TESTEXEC}
    cd -

    return ${SUCCESS}
}

buildAndTest() {
    build $1
    SUCCESS=$?

    "${TESTEXEC}" || SUCCESS=1

    return ${SUCCESS}
}

if [ "${MODE}" = "RELEASE" ]; then
    for compiler in ${COMPILER}; do
        buildAndTest "${compiler}" || EXIT=1
    done
elif [ "${MODE}" = "PROFILING" ]; then
    export PROFILE="--coverage"

    buildAndTest "${COMPILER}" || EXIT=1
    lcov -c -d . -o coverage.info || EXIT=1
    lcov --remove coverage.info '*/include/*' '*/boost/*' -o filtered.info
    test -f filtered.info && coveralls-lcov filtered.info

    CXXFLAGS="-pg"
    LDFLAGS="-pg"
    buildAndTest "${COMPILER}" || EXIT=1
    gprof "${TESTEXEC}" gmon.out | head -n 100
elif [ "${MODE}" = "DEBUG" ]; then
    CMAKE_BUILD_TYPE="Debug"

    CXXFLAGS="-fsanitize=address,undefined,integer-divide-by-zero,float-divide-by-zero,float-cast-overflow,return -fno-omit-frame-pointer"
    LDFLAGS="-lasan -lubsan"
    build "${COMPILER}"
    logFile=sanitizerLog
    ${TESTEXEC} -l all --color=no &> ${logFile} || EXIT=1
    grep --color=auto -E -C 5 'runtime *error|LeakSanitizer' ${logFile} && EXIT=1

    unset CXXFLAGS
    unset LIBS
    build "${COMPILER}"
    valgrind --error-exitcode=1 --leak-check=full "${TESTEXEC}" || EXIT=1
elif [ "${MODE}" = "ANALYSIS" ]; then
    ./bin/staticcheck.sh || EXIT=1
fi

exit ${EXIT}
