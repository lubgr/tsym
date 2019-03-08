#!/usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 [test-executable]"
    exit 1
fi

TESTEXEC="$1"

logfile=`mktemp`

trap removeLogfile INT

function removeLogfile() {
    rm -f "${logfile}"
    exit 1
}

"${TESTEXEC}" --log_level=all --log_sink="${logfile}" --color=no 2>/dev/null

totalTime=`sed -n '$s@.*testing time: *\(.*\)us@\1@p' "${logfile}"`

sed -i '/Entering test case/s@^.*\/\(.*\/.*\.cpp\)(\(.*\)): *Entering test case.*$@\1 +\2@p' "${logfile}"
sed -i '/Leaving test case/s@^.*: \([0-9]\+\)us@\1@' "${logfile}"

grep '^[0-9]\+$\|^.*\.cpp *+[0-9]\+$' "${logfile}" | uniq | paste -d' ' - - |\
    awk -v totalTime="${totalTime}" '{if ($3 >= 0.015*totalTime) printf("%s %s (%.2f)\n", $1, $2, 100*$3/totalTime) }' |\
    while read line; do
        file=`echo "${line}" | cut -d' ' -f 1`
        lineNumber=`echo "${line}" | cut -d' ' -f 2 | tr -d '+'`
        lineInFile=`sed -n "${lineNumber}p" "${file}"`

        echo "${line} ${lineInFile}"
    done

removeLogfile
