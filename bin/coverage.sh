#!/usr/bin/env bash

if [[ $# -ne 2 || ! -d $1 ]]; then
    echo "Usage: $0 [coverage-build-directory] [gcov-executable]"
    exit 1
fi

DIR="$1"
GCOV="$2"

if [ -z "`find "${DIR}" -iname '*.gcda' -or -iname '*.gcno' 2> /dev/null`" ]; then
    echo "No gcov files found (*.gcda, *.gcno) in ${DIR}!"
    exit 1
fi

if ! which lcov &> /dev/null; then
    echo "lcov not found in \$PATH!"
    exit 1
fi

pushd "${DIR}"

HTMLDIR=coverage-html
mkdir -p "${HTMLDIR}"

rm -rf "${HTMLDIR}"/*

lcov --gcov-tool "${GCOV}" -c -d src -o coverage.info || exit 1
lcov --remove coverage.info '*/c++*' '*/include/*' '*boost/*' -o filtered.info
genhtml -o "${HTMLDIR}" filtered.info

popd

echo "Test coverage statistics written to file://${PWD}/${DIR}/${HTMLDIR}/index.html"

