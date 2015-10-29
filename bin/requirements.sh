#!/bin/sh

NECESSARY_EXEC="git make g++ grep lex yacc"
NECESSARY_LDFLAGS="-lCppUTest -lstdc++ -lm"
NECESSARY_HEADERS="CppUTest/TestHarness.h CppUTest/SimpleString.h"

SUPPLEMENT_EXEC="clang vim python gdb gcovr valgrind ginsh"
SUPPLEMENT_LDFLAGS="-ltrlog"
SUPPLEMENT_HEADERS="trlog/trlog.h"
SUPPLEMENT_FILES="/usr/local/share/gdb/cpputest.py /usr/local/share/gdb/hlgdb.py"

function checkExecutable {
    for executable in $1; do
        if ! which $executable &> /dev/null; then
            echo "$2 executable $executable not found"
        fi
    done
}

function checkLibraryLink {
    for flag in $1; do
        if ! echo "int main() { return 0; }" | cc $flag -o /dev/null -xc++ - &>/dev/null; then
            echo "Couldn't link against $2 ${flag}"
        fi
    done
}

function checkHeaderFiles {
    for header in $1; do
        if ! echo -e "#include \"$header\"\n int main() { return 0; }" | \
            cc $flag -o /dev/null -xc++ - ; then
            echo "Couldn't include $2 header ${header}"
        fi
    done
}

function checkFiles {
    for file in $1; do
        if ! [ -f $file ]; then
            echo "$2 file $file not found"
        fi
    done
}

checkExecutable "$NECESSARY_EXEC" "Mandatory"
checkLibraryLink "$NECESSARY_LDFLAGS" "mandatory"
checkHeaderFiles "$NECESSARY_HEADERS" "mandatory"

checkExecutable "$SUPPLEMENT_EXEC" "Supplementary"
checkLibraryLink "$SUPPLEMENT_LDFLAGS" "supplementary"
checkHeaderFiles "$SUPPLEMENT_HEADERS" "supplementary"
checkFiles "$SUPPLEMENT_FILES" "Supplementary"

