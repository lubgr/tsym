#!/usr/bin/env bash

if [[ $# -ne 1 || ! -f $1 ]]; then
    echo "Usage: $0 [compile command database (json)]"
    exit 1
fi

CDB=$1
CPPCHECK=cppcheck
CLANGTIDY=clang-tidy
CPPCLEAN=cppclean
FMTSCRIPT=./bin/format.sh
CONFIGDIR=misc

INCLUDE="-I src -I include/tsym -I unit-tests"
SRC="src/ unit-tests/"

EXIT=0

if ! which $CLANGTIDY &> /dev/null; then
    echo "$CLANGTIDY not found"
    exit 1
fi

if which $CPPCHECK &> /dev/null; then
    $CPPCHECK --template='{file} +{line} ({severity}/{id}): {message}' \
    -j 4 \
    --enable=all --suppressions-list=$CONFIGDIR/cppcheckrc \
    -q --project="${CDB}"
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

if which $CLANGTIDY &> /dev/null; then
    for file in `find ${SRC} -iname '*.cpp'`; do
        $CLANGTIDY -quiet -header-filter='tsym' -p "${CDB}" "${file}"
    done
else
    echo "$CLANGTIDY not found"
    EXIT=1
fi

echo ''

echo "Formatting diff with `clang-format --version`"
${FMTSCRIPT}

exit ${EXIT}
