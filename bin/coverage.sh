#!/usr/bin/env bash

TARGETDIR=build/coverage
INDEX=${TARGETDIR}/index.html
SRCDIR=build/src

if [ -z "`find ${SRCDIR} -iname '*.gcda' -or -iname '*.gcno' 2> /dev/null`" ]; then
    echo "No gcov files found (*.gcda, *.gcno) in ${SRCDIR}!"
    exit 1
fi

if ! which gcovr &> /dev/null; then
    echo "gcovr not found in \$PATH!"
    exit 1
fi

test -d $TARGETDIR || mkdir -p ${TARGETDIR}

rm -f ${TARGETDIR}/*

gcovr -r  ${SRCDIR} --sort-percentage --html --html-details -o ${INDEX}

echo "Test coverage statistics written to file://${PWD}/${INDEX}"
