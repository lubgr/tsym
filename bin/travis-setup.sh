#!/usr/bin/env bash

set -e

pushd ${BOOSTDIR}
if [ "${MODE}" = "ANALYSIS" ]; then
    targetDir="/usr/local/include"
    sudo mkdir -p "${targetDir}"
    sudo cp -r boost "${targetDir}"
else
    ./bootstrap.sh debug-symbols=off cflags="${CPPFLAGS} ${CFLAGS} -std=c99 -O2" cxxflags="${CPPFLAGS} ${CXXFLAGS} -std=c++14 -O2"
    sudo ./b2 -d0 --with-test link=static install --prefix=/usr/local
fi
popd

if [ "${MODE}" = "PROFILING" ]; then
    gem install coveralls-lcov
elif [ "${MODE}" = "ANALYSIS" ]; then
    sudo pip install --upgrade cppclean

    CPPCHECKVER=1.82
    wget -qO- https://github.com/danmar/cppcheck/archive/${CPPCHECKVER}.tar.gz | tar xz
    sudo make -C ./cppcheck-${CPPCHECKVER} SRCDIR=build CFGDIR=/usr/share/cppcheck/cfg HAVE_RULES=yes install
fi
