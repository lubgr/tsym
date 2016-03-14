#!/usr/bin/env bash

cd include

libheader=tsym.h
headers=`ls -1 *.h | grep -v $libheader`

for header in $headers; do
    if ! grep "^ *#include *\"${header} *\"" $libheader &> /dev/null; then
        echo "$header isn't included in ${libheader}!"
    fi
done
