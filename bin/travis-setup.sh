#!/usr/bin/env bash

set -e

if [ "${MODE}" != "ANALYSIS" ]; then
    cd ${BOOSTDIR}
    ./bootstrap.sh debug-symbols=off cflags="${CPPFLAGS} ${CFLAGS} -std=c99 -O2" cxxflags="${CPPFLAGS} ${CXXFLAGS} -std=c++14 -O2"
    ./b2 --with-test link=static install --prefix=/usr/local
    cd ..
fi

if [ "${MODE}" = "PROFILING" ]; then
    gem install coveralls-lcov
elif [ "${MODE}" = "ANALYSIS" ]; then
    sudo pip install --upgrade cppclean

    CPPCHECKVER=1.82
    wget -qO- https://github.com/danmar/cppcheck/archive/${CPPCHECKVER}.tar.gz | tar xz
    sudo make -C ./cppcheck-${CPPCHECKVER} SRCDIR=build CFGDIR=/usr/share/cppcheck/cfg HAVE_RULES=yes install

    wget -qO- https://github.com/oclint/oclint/releases/download/v${OCLINTVER}/oclint-${OCLINTVER}-x86_64-linux-3.13.0-125-generic.tar.gz | tar xz
fi
