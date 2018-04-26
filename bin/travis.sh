#!/usr/bin/env bash

TESTEXEC="./build/runtests"
export CPPFLAGS="-isystem ${BOOSTDIR} -I src"
export LDFLAGS="-L ${BOOSTDIR}"
EXIT=0

build() {
    CXX=$1

    make -j 4 CXX="${CXX}" lib staticlib tests
    SUCCESS=$?

    cat build/buildinfo.h

    return ${SUCCESS}
}

buildAndTest() {
    build $1
    SUCCESS=$?

    "${TESTEXEC}" || SUCCESS=1

    return ${SUCCESS}
}

clean() {
    make clean
}

if [ "${MODE}" = "RELEASE" ]; then
    for compiler in ${COMPILER}; do
        buildAndTest "${compiler}" || EXIT=1
        clean
    done
elif [ "${MODE}" = "PROFILING" ]; then
    export PROFILE="--coverage"

    buildAndTest "${COMPILER}" || EXIT=1
    lcov -c -d . -o build/coverage.info || EXIT=1
    lcov --remove build/coverage.info '*/include/*' '*/boost/*' -o build/filtered.info
    test -f build/filtered.info && coveralls-lcov build/filtered.info
    clean

    export PROFILE="-pg"
    buildAndTest "${COMPILER}" || EXIT=1
    gprof "${TESTEXEC}" gmon.out | head -n 100
    clean
elif [ "${MODE}" = "DEBUG" ]; then
    commonCxxFLags="-O0 -g -std=c++14 -fPIC"

    sanitizerFlags="-fsanitize=address,undefined,integer-divide-by-zero,float-divide-by-zero,float-cast-overflow,return"
    export CXXFLAGS="${sanitizerFlags} -fno-omit-frame-pointer ${commonCxxFLags}"
    export LIBS="-lasan -lubsan"
    build "${COMPILER}"
    logFile=sanitizerLog
    ${TESTEXEC} -l all --color=no > ${logFile} || EXIT=1
    grep -i -C 20 error ${logFile} && EXIT=1
    clean

    export CXXFLAGS="${commonCxxFLags}"
    unset LIBS
    build "${COMPILER}"
    valgrind --error-exitcode=1 --leak-check=full "${TESTEXEC}" || EXIT=1
    clean
elif [ "${MODE}" = "ANALYSIS" ]; then
    ./bin/staticcheck.sh || EXIT=1
fi

exit ${EXIT}
