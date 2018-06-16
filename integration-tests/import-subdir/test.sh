#!/usr/bin/env bash

set -e

EXPECTED_OUTPUT="3/4 + cos(a + b)"
: "${DESTDIR?}"
: "${TSYM_INSTALLATION_PATH?}"

CURRENT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="${DESTDIR}/$(basename "${CURRENT_DIR}")"
TSYM_SUBDIR="${TEST_DIR}/tsym-local"
TSYM_REMOTE="${TSYM_REMOTE:-"${CURRENT_DIR}/../.."}"

if [ -d "${TSYM_SUBDIR}" ]; then
    pushd "${TSYM_SUBDIR}"
    git pull
    popd
else
    mkdir -p "${TSYM_SUBDIR}"
    git clone "${TSYM_REMOTE}" "${TSYM_SUBDIR}"
fi

cp "${CURRENT_DIR}/"{CMakeLists.txt,main.cpp} "${TEST_DIR}"

buildAndTest() {
    variant=$1

    if [ "${variant}" = "static" ]; then
        buildShared=OFF
    else
        buildShared=ON
    fi

    TEST_BUILD_DIR="${TEST_DIR}/build-${variant}"
    mkdir -p "${TEST_BUILD_DIR}"

    pushd "${TEST_BUILD_DIR}"
    cmake -D BUILD_TESTING=OFF -D TSYM_SUBDIR="${TSYM_SUBDIR}" -D BUILD_SHARED_LIBS="${buildShared}" "${TEST_DIR}"
    make
    popd

    output="$("${TEST_BUILD_DIR}"/tsym-test)"

    if [ "${output}" != "${EXPECTED_OUTPUT}" ]; then
        echo "${output} doesn't match ${EXPECTED_OUTPUT}"
        exit 1
    fi
}

buildAndTest "shared"
buildAndTest "static"
