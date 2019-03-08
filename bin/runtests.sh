#!/usr/bin/env bash

executables=`ls -1t build*/unit-tests/tests 2>/dev/null`

if [ -z "${executables}" ]; then
    echo "No unit test executable found"
    exit 1
fi

latest=`echo "${executables}" | sed '1q'`

echo "${executables}"
echo ""

"$latest" $@
