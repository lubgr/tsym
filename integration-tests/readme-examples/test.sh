#!/usr/bin/env bash

set -e

: "${TSYM_INSTALLATION_PATH?}"
: "${DESTDIR?}"
: "${TSYM_REMOTE?}"

CURRENT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="${DESTDIR}/$(basename "${CURRENT_DIR}")"
TEST_BUILD_DIR="${TEST_DIR}/build"
EXAMPLE_CODE="${TEST_BUILD_DIR}/main.cpp"
EXAMPLE_OUTPUT="${TEST_BUILD_DIR}/readme-output"
EXPECTED_OUTPUT="${CURRENT_DIR}/expected-output"
README="${TSYM_REMOTE}/README.md"

mkdir -p "${TEST_BUILD_DIR}"
touch "${EXAMPLE_CODE}"
rm -f "${EXAMPLE_OUTPUT}"

pushd "${TEST_BUILD_DIR}"
cmake -D CMAKE_PREFIX_PATH="${TSYM_INSTALLATION_PATH}" "${CURRENT_DIR}"
popd

awk \
    ' BEGIN { printSnippet = 0 }
    /^``` *$/ { if (printSnippet) { printSnippet = 0; printf "\n" }}
    { if (printSnippet) printf "%s ", $0 }
    /^```c++/ { printSnippet = 1 }' "${README}" |\
        sed 's/int *main *([^)]*)//' |\
        grep -v 'boost::numeric\|std::array' |\
        sed 's/#include *[<\"][^>\"]*[>\"]//g' |\
        while read -r line; do
            echo '#include "tsym/tsym.h"' > "${EXAMPLE_CODE}"
            echo '#include <iostream>' >> "${EXAMPLE_CODE}"
            echo 'int main(int, char**) {' >> "${EXAMPLE_CODE}"
            echo "${line}" >> "${EXAMPLE_CODE}"
            echo 'return 0; }' >> "${EXAMPLE_CODE}"

            make -C "${TEST_BUILD_DIR}" clean
            make -C "${TEST_BUILD_DIR}"
            "${TEST_BUILD_DIR}/tsym-test" >> "${EXAMPLE_OUTPUT}"
        done

diff "${EXAMPLE_OUTPUT}" "${EXPECTED_OUTPUT}"

exit $?
