#!/usr/bin/env bash

CPPCHECK=cppcheck
CPPCLEAN=cppclean
OCLINT=oclint
CONFIGDIR=misc

INCLUDE="-I src -I test"
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
    $OCLINT -version
    || echo "$OCLINT not found"
