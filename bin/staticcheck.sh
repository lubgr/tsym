#!/usr/bin/env bash

CPPCHECK=cppcheck
CPPCLEAN=cppclean
OCLINT=oclint
CONFIGDIR=misc

INCLUDE="-I src -I test"
SYSTEM_INCLUDE="-isystem ${BOOSTDIR}"
SRC="src/ test/"

which $CPPCHECK &> /dev/null && \
    $CPPCHECK --template='{file} +{line} ({severity}/{id}): {message}' \
    -j 4 \
    --enable=all --suppressions-list=$CONFIGDIR/cppcheckrc \
    -q $INCLUDE --suppress=missingIncludeSystem --std=c++14 --language=c++ $SRC \
    || echo "$CPPCHECK not found"

echo ''

which $CPPCLEAN &> /dev/null && \
    $CPPCLEAN $INCLUDE $SRC \
    | grep -v 'declared but not defined' | grep -v 'not found in expected header' \
    | grep -v 'unable to find' \
    || echo "$CPPCLEAN not found"

echo ''

which $OCLINT &> /dev/null && \
    $OCLINT -version && \
    $OCLINT `find $SRC -iname '*.cpp' -or -iname '*.h'` -max-priority-3=0 -- -std=c++14 $INCLUDE $SYSTEM_INCLUDE\
    || echo "$OCLINT not found"
