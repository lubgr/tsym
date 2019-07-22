#!/usr/bin/env bash

set -e

if [ "${MODE}" = "PROFILING" ]; then
    gem install coveralls-lcov
elif [ "${MODE}" = "ANALYSIS" ]; then
    sudo pip install --upgrade cppclean

    CPPCHECKVER=1.88
    wget -qO- https://github.com/danmar/cppcheck/archive/${CPPCHECKVER}.tar.gz | tar xz
    sudo make -C ./cppcheck-${CPPCHECKVER} SRCDIR=build CFGDIR=/usr/share/cppcheck/cfg HAVE_RULES=yes install
fi
