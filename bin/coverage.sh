#!/usr/bin/env bash

TARGETDIR=build/coverage
INDEX=${TARGETDIR}/index.html

if [ -z "`find . -iname '*.gcda' -or -iname '*.gcno' 2> /dev/null`" ]; then
    echo "No gcov files found (*.gcda, *.gcno)!"
    exit 1
fi

if ! which gcovr &> /dev/null; then
    echo "gcovr not found in \$PATH!"
    exit 1
fi

test -d $TARGETDIR || mkdir -p ${TARGETDIR}

rm -f ${TARGETDIR}/*

gcovr -r . --exclude='.*sgfy\.(h|cpp)' --sort-percentage --html --html-details -o ${INDEX}

echo "Test coverage statistics written to file://${PWD}/${INDEX}"
