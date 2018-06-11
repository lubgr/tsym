#!/usr/bin/env bash

set -x

checkInstalledFile(){
    file="$1"

    test -f "${file}"
    test -s "${file}"
    test "`ls -l "${file}" | cut -d' ' -f1`" = "-rw-r--r--"
}

set -e

: "${TSYM_INSTALLATION_PATH?}"

pushd "${TSYM_INSTALLATION_PATH}"

for header in globals logger plu tsym var version; do
    checkInstalledFile "include/tsym/${header}".h
done

for middleFilenamePart in -config-version -config Targets-noconfig Targets; do
    checkInstalledFile lib*"/cmake/tsym/tsym${middleFilenamePart}.cmake"
done

test -f lib*/libtsym.*

popd
