#!/usr/bin/env bash

TARGETDIR=doc/coverage

if [ -z "`find . -iname '*.gcna' -or -iname '*.gcno' 2> /dev/null`" ]; then
    echo "No gcov files found (*.gcna, *.gcno)!"
    exit 1
fi

if ! which gcovr &> /dev/null; then
    echo "Gcovr script not found in \$PATH!"
    exit 1
fi

if [ ! -d $TARGETDIR ]; then
    "Directory $TARGETDIR for HTML output not found."
    exit 1
fi

rm -f ${TARGETDIR}/*

gcovr -r build/debug/src/ --sort-percentage --html --html-details -o ${TARGETDIR}/coverage.html

echo "Generated test coverage statistics in ${TARGETDIR}"
