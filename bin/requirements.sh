#!/bin/sh

NECESSARY_EXEC="git make g++ valgrind gdb"
SUPPLEMENT_EXEC="vim python gcovr ginsh"

NECESSARY_FILES="/usr/local/share/gdb/cpputest.py /usr/local/share/gdb/hlgdb.py"

NECESSARY_LDFLAGS="-lCppUTest -lstdc++"
NECESSARY_HEADERS="CppUTest/TestHarness.h CppUTest/SimpleString.h"

error=0

for executable in $NECESSARY_EXEC; do
    if ! which $executable &> /dev/null; then
        echo "Executable $executable not found!"
        error=1
    fi
done

for flag in $NECESSARY_LDFLAGS; do
    if ! echo "int main() { return 0; }" | gcc $flag -o /dev/null -xc++ - &>/dev/null; then
        echo "Couldn't link against ${flag}!"
        error=1
    fi
done

for header in $NECESSARY_HEADERS; do
    if ! echo -e "#include \"$header\"\n int main() { return 0; }" | \
        gcc $flag -o /dev/null -xc++ - ; then
        echo "Couldn't include header ${header}!"
        error=1
    fi
done

for file in $NECESSARY_FILES; do
    if ! [ -f $file ]; then
        echo "File $file not found!"
        error=1
    fi
done

if [ $error -eq 1 ]; then
    exit 1
fi

for executable in $SUPPLEMENT_EXEC; do
    if ! which $executable &> /dev/null; then
        echo "Executable $executable not found!"
    fi
done
