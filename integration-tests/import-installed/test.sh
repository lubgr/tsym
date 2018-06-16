#!/usr/bin/env bash

set -e

: "${DESTDIR?}"
: "${TSYM_INSTALLATION_PATH?}"

EXPECTED_OUTPUT="atan(3/7)"
CURRENT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="${DESTDIR}/$(basename "${CURRENT_DIR}")"
TEST_BUILD_DIR="${TEST_DIR}/build"

mkdir -p "${TEST_BUILD_DIR}"
pushd "${TEST_BUILD_DIR}"
cmake -D CMAKE_PREFIX_PATH="${TSYM_INSTALLATION_PATH}" "${CURRENT_DIR}"
make
popd

output="$("${TEST_BUILD_DIR}"/tsym-test)"

if [ "${output}" != "${EXPECTED_OUTPUT}" ]; then
   echo "${output} doesn't match ${EXPECTED_OUTPUT}"
   exit 1
fi
