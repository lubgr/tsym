#!/usr/bin/env bash

CPPCHECK=cppcheck
CONFIGDIR=misc

INCLUDE="-I src -I test"
SRC="src/ test/"

which $CPPCHECK &> /dev/null && \
    $CPPCHECK --template='{file} +{line} ({severity}/{id}): {message}' \
    -j 4 \
    --enable=all --suppressions-list=$CONFIGDIR/cppcheckrc \
    -q $INCLUDE --suppress=missingIncludeSystem --std=c++14 --language=c++ $SRC || \
    echo "$CPPCHECK not found"

echo ''
