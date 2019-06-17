#!/usr/bin/env bash

TESTEXEC="./unit-tests/tests"
EXIT=0

buildDir() {
    buildDir="build-$1"
    mkdir -p "${buildDir}"
    pushd "${buildDir}"
}

configure() {
    CXX=$1

    cmake \
        -D CMAKE_CXX_COMPILER="${CXX}"\
        -D BUILD_TESTING=ON\
        -D CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"\
        ..
}

build() {
    configure $1

    make tsym tests
    SUCCESS=$?

    cat -n unit-tests/Makefile

    return ${SUCCESS}
}

buildAndTest() {
    build $1
    SUCCESS=$?

    "${TESTEXEC}" || SUCCESS=1

    return ${SUCCESS}
}


if [ "${MODE}" = "RELEASE" ]; then
    CMAKE_BUILD_TYPE="Release"
    for compiler in ${COMPILER}; do
        buildDir "release-${compiler}"
        buildAndTest "${compiler}" || EXIT=1
        popd
    done
elif [ "${MODE}" = "PROFILING" ]; then
    CMAKE_BUILD_TYPE="Coverage"
    buildDir "coverage-${COMPILER}"
    buildAndTest "${COMPILER}" || EXIT=1
    lcov -c -d src -o coverage.info || EXIT=1
    lcov --remove coverage.info '*usr/include/*' '*/boost/*' -o filtered.info
    test -f filtered.info && coveralls-lcov filtered.info
    popd

    CMAKE_BUILD_TYPE="Profile"
    buildDir "profile-${COMPILER}"
    buildAndTest "${COMPILER}" || EXIT=1
    gprof "${TESTEXEC}" gmon.out | head -n 100
    popd
elif [ "${MODE}" = "DEBUG" ]; then
    CMAKE_BUILD_TYPE=Sanitizer

    buildDir "debug-sanitizer-${COMPILIER}"
    build "${COMPILER}"
    logFile=sanitizerLog
    ${TESTEXEC} -l all --color=no &> "${logFile}" || EXIT=1
    cat "${logFile}"
    grep --color=auto -E -C 5 'runtime *error|LeakSanitizer' ${logFile} && EXIT=1
    popd

    CMAKE_BUILD_TYPE=Debug
    buildDir "debug-valgrind-${COMPILIER}"
    build "${COMPILER}"
    valgrind --error-exitcode=1 --leak-check=full "${TESTEXEC}" -t '!@expensive' || EXIT=1
    popd
elif [ "${MODE}" = "INTEGRATION" ]; then
    pushd integration-tests
    buildDirRoot="$(pwd)/output"
    mkdir -p "${buildDirRoot}"
    CXX="${COMPILER}" ./test-all.sh "${buildDirRoot}" || EXIT=1
    popd
elif [ "${MODE}" = "ANALYSIS" ]; then
    buildDir "compile-database-${COMPILER}"
    configure "${COMPILER}"
    popd

    ./bin/staticcheck.sh "${buildDir}/compile_commands.json" || EXIT=1
fi

exit ${EXIT}
