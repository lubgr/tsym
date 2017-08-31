#!/usr/bin/env bash

AWK_RULES=(
    'line with more than 100 chars' '{if (length > 100) print NR}'
    'empty line with whitespace' '/^ +$/ {print NR}'
    'trailing whitespace' '/[^ ]+ +$/ {print NR}'
    'empty line at bottom' 'END {if ($0 ~ /^ *$/) print NR}'
    'multiple empty lines' '/^ *$/ {if (++n > 1) print NR} /[^ ]/ {n = 0}'
    'empty line before end of block' '/^ *$/ {++n} /^ *}/ {if (n >= 1) print NR - 1} /[^ ]/ {n = 0}'
    'empty line after start of block' '/^ *{ *$/ {n = NR} /^ *$/ {if (n != 0 && NR == n + 1) print NR}'
    'multiple spaces between tokens' '/^ *[^ /*][^"]*  +[^ ]/ {print NR}'
    'indentation not a multiple of 4 spaces' '/^ *[^ /*]/ {n = match($0, /[^ ]/) - 1; if (n % 4 != 0) print NR}'
)

for file in {test,src}/{*.h,*.cpp}; do
    echo $file

    for i in `seq 0 2 ${#AWK_RULES[@]}`; do
        description=${AWK_RULES[$i]}
        awkCmd=${AWK_RULES[$(( i + 1 ))]}

        for line in `gawk "$awkCmd" $file`; do
            printf "%-40s%s\n" "$file +$line" "$description"
        done
    done
done
