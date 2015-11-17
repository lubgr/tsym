#!/usr/bin/env bash

CC=g++
CFLAGS="-pedantic -Wall -I include -I src"

headers=`ls include/*.h src/*.h`

if [ -z "$headers" ]; then
    echo "No header files found!"
    exit 1
fi

tmpdir=`mktemp -d`
trap "rm -rf $tmpdir" EXIT

echo "Test compilation in ${tmpdir}"
cp -r * $tmpdir
cd $tmpdir

tmpobj=`mktemp -p $tmpdir`
origh=`mktemp -p $tmpdir`

echo "Compiling with $CC $CFLAGS"
echo "Implementation files compile without these lines:"
echo "----------------------------------------------------"

for header in $headers; do
    search=`echo $header | sed 's/.*\/\(.*\)\.h/\1\.cpp/'`
    cppfile=`find . -iname "$search"`

    if [ -z $cppfile ]; then
        continue
    fi

    cp $header $origh

    nstart=`awk '/#define +.*_H$/ {print NR + 1}' $header`
    nend=`awk '/#endif *$/ {print NR - 1}' $header`

    for n in `seq $nstart $nend`; do
        if sed -n ${n}p $header | grep '^ *$' >/dev/null; then
            continue
        elif  sed -n ${n}p $header | grep 'public\|private\|protected *:' >/dev/null; then
           continue
        elif  sed -n ${n}p $header | grep '^ */[*].*[*]/' >/dev/null; then
           continue
        elif  sed -n ${n}p $header | grep '^ *[*]' >/dev/null; then
            continue
        fi

        sed -i "${n}d" $header
        $CC $CFLAGS -c $cppfile -o $tmpobj &>/dev/null
        success=$?

        if [ $success -eq 0 ]; then
            line=`sed -n "${n}p" $origh`
            printf "%-30s%s\n" "$header +${n}" "$line"
        fi

        cp $origh $header
    done

    mv $origh $header
done
echo "----------------------------------------------------"

cd -

echo "Clean up temporary directory"
rm -rf $tmpdir

