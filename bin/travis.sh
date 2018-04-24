#!/usr/bin/env bash

set -e

TESTEXEC="./build/runtests"
export CPPFLAGS="-isystem ${BOOSTDIR} -I src"
export LDFLAGS="-L ${BOOSTDIR}"

build() {
    CXX=$1
    make -j 4 CXX="${CXX}" lib staticlib tests
    cat build/buildinfo.h
}

buildAndTest() {
    build $1
    "${TESTEXEC}"
}

clean() {
    make clean
}

if [ "${MODE}" = "RELEASE" ]; then
    for compiler in ${COMPILER}; do
        buildAndTest "${compiler}"
        clean
    done
elif [ "${MODE}" = "PROFILING" ]; then
    export PROFILE="--coverage"

    buildAndTest "${COMPILER}"
    lcov -c -d . -o build/coverage.info
    lcov --remove build/coverage.info '*/include/*' '*/boost/*' -o build/filtered.info
    test -f build/filtered.info && coveralls-lcov build/filtered.info
    clean

    export PROFILE="-pg"
    buildAndTest "${COMPILER}"
    gprof "${TESTEXEC}" gmon.out | head -n 100
    clean
elif [ "${MODE}" = "DEBUG" ]; then
    commonCxxFLags="-O0 -g -std=c++14 -fPIC"

    sanitizerFlags="-fsanitize=address,undefined,integer-divide-by-zero,float-divide-by-zero,float-cast-overflow,return"
    export CXXFLAGS="${sanitizerFlags} -fno-omit-frame-pointer ${commonCxxFLags}"
    export LIBS="-lasan -lubsan"
    buildAndTest "${COMPILER}"
    clean

    export CXXFLAGS="${commonCxxFLags}"
    unset LIBS
    build "${COMPILER}"
    valgrind --error-exitcode=1 --leak-check=full "${TESTEXEC}"
    clean
elif [ "${MODE}" = "ANALYSIS" ]; then
    ./bin/staticcheck.sh
fi
