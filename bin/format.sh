#!/usr/bin/env bash

formatstr="%-40s%s\n"

for file in {test,src}/{*.h,*.cpp}; do
    echo $file
    for line in `gawk '{if (length > 100) print NR}' $file`; do
        printf $formatstr "$file +$line" "line > 100 chars"
    done

    for line in `gawk '/^ +$/ {print NR}' $file`; do
        printf $formatstr "$file +$line" "empty line with whitespace"
    done

    for line in `gawk '/[^ ]+ +$/ {print NR}' $file`; do
        printf $formatstr "$file +$line" "trailing whitespace"
    done

    for line in `gawk 'END {if ($0 ~ /^ *$/) print NR}' $file`; do
        printf $formatstr "$file +$line" "empty line at bottom"
    done

    for line in `gawk '/^ *$/ {if (++n > 1) print NR} /[^ ]/ {n = 0}' $file`; do
        printf $formatstr "$file +$line" "multiple empty lines"
    done

    for line in `gawk '/^ *$/ {++n} /^ *}/ {if (n >= 1) print NR - 1} /[^ ]/ {n = 0}' $file`; do
        printf $formatstr "$file +$line" "empty line before end of block"
    done

    for line in `gawk '/^ *[^ /*][^"]*  +[^ ]/ {print NR}' $file`; do
        printf $formatstr "$file +$line" "multiple spaces between tokens"
    done

    for line in `gawk '/^ *[^ /*]/ {n = match($0, /[^ ]/) - 1; if (n % 4 != 0) print NR}' $file`; do
        printf $formatstr "$file +$line" "indentation not a multiple of 4 spaces"
    done
done
