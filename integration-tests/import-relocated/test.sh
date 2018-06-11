#!/usr/bin/env bash

set -e

EXPECTED_OUTPUT="-3/4 + a"
: "${DESTDIR?}"
: "${TSYM_INSTALLATION_PATH?}"

CURRENT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="${DESTDIR}/$(basename "${CURRENT_DIR}")"
TEST_BUILD_DIR="${TEST_DIR}/build"
RELOCATED_INSTALL_PATH="${TEST_DIR}/tsym-installation"

mkdir -p "${TEST_BUILD_DIR}"
rm -rf "${RELOCATED_INSTALL_PATH}"
cp -a "${TSYM_INSTALLATION_PATH}" "${RELOCATED_INSTALL_PATH}"
pushd "${TEST_BUILD_DIR}"
cmake -D CMAKE_PREFIX_PATH="${RELOCATED_INSTALL_PATH}" "${CURRENT_DIR}"
make
popd

output="$("${TEST_BUILD_DIR}"/test)"

if [ "${output}" != "${EXPECTED_OUTPUT}" ]; then
    echo "${output} doesn't match ${EXPECTED_OUTPUT}"
    exit 1
fi
