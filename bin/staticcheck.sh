#!/usr/bin/env bash

CPPCHECK=cppcheck
CPPCLEAN=cppclean
OCLINT=oclint
FMTSCRIPT=./bin/format.sh
CONFIGDIR=misc

INCLUDE="-I src -I test"
SRC="src/ test/"

EXIT=0

if env | grep BOOSTDIR; then
    SYSTEM_INCLUDE="-isystem ${BOOSTDIR}"
fi

if which $CPPCHECK &> /dev/null; then
    $CPPCHECK --template='{file} +{line} ({severity}/{id}): {message}' \
    -j 4 \
    --enable=all --suppressions-list=$CONFIGDIR/cppcheckrc \
    -q $INCLUDE --suppress=missingIncludeSystem --std=c++14 --language=c++ $SRC
else
    echo "$CPPCHECK not found"
    EXIT=1
fi

echo ''

if which $CPPCLEAN &> /dev/null; then
    $CPPCLEAN $INCLUDE $SRC \
    | grep -v 'declared but not defined' | grep -v 'not found in expected header' \
    | grep -v 'unable to find'
else
    echo "$CPPCLEAN not found"
    EXIT=1
fi

echo ''

if which $OCLINT &> /dev/null; then
    $OCLINT -version && \
    $OCLINT `find $SRC -iname '*.cpp' -or -iname '*.h'` -max-priority-3=0 -- -std=c++14 -x c++ $INCLUDE $SYSTEM_INCLUDE \
    | grep -v '|P3\]'
else
    echo "$OCLINT not found"
    EXIT=1
fi

echo ''

fmtDiff=`mktemp`
${FMTSCRIPT} 2>&1 | tee ${fmtDiff}
test -s ${fmtDiff} && EXIT=1
rm -f ${fmtDiff}

exit ${EXIT}
