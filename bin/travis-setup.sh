#!/usr/bin/env bash

set -e

if [ "${MODE}" != "ANALYSIS" ]; then
    cd ${BOOSTDIR}
    ./bootstrap.sh && ./b2 --with-test link=static
    find ./stage -name 'libboost_unit_test*.a' -exec ln -s '{}' . \;
    cd ..
fi

if [ "${MODE}" = "RELEASE" ]; then
elif [ "${MODE}" = "PROFILING" ]; then
    gem install coveralls-lcov
elif [ "${MODE}" = "DEBUG" ]; then
elif [ "${MODE}" = "ANALYSIS" ]; then
    sudo pip install --upgrade cppclean

    CPPCHECKVER=1.82
    wget -qO- https://github.com/danmar/cppcheck/archive/${CPPCHECKVER}.tar.gz | tar xz
    sudo make -C ./cppcheck-${CPPCHECKVER} SRCDIR=build CFGDIR=/usr/share/cppcheck/cfg HAVE_RULES=yes install

    wget -qO- https://github.com/oclint/oclint/releases/download/v${OCLINTVER}/oclint-${OCLINTVER}-x86_64-linux-3.13.0-125-generic.tar.gz | tar xz
fi
