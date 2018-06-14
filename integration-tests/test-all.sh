#!/usr/bin/env bash

set -x

if [ $# -ne 1 ]; then
    echo "Usage $0 [output-directory]"
    exit 1
fi

CURRENT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

DESTDIR=$1
export CMAKE_MIN_VERSION="3.8"
export TSYM_INSTALLATION_PATH="${DESTDIR}/setup-tsym-installation"
export TSYM_REMOTE="${TSYM_REMOTE:-$(dirname "${CURRENT_DIR}")}"
export TSYM_BUILD_DIR="${DESTDIR}/setup-tsym-build"
TSYM_CLONE_DIR="${DESTDIR}/setup-tsym-clone"

if [ -d "${TSYM_CLONE_DIR}" ]; then
    pushd "${TSYM_CLONE_DIR}"
    git pull
    popd
else
    git clone "${TSYM_REMOTE}" "${TSYM_CLONE_DIR}"
fi

mkdir -p "${TSYM_BUILD_DIR}"
pushd "${TSYM_BUILD_DIR}"

cmake --debug-output -D BUILD_TESTING=OFF -D CMAKE_INSTALL_PREFIX="${TSYM_INSTALLATION_PATH}" "${TSYM_CLONE_DIR}"

# This option doesn't work out earlier due to an older cmake version
set -e

make install
popd

 # Don't export this one earlier, it changes the cmake install prefix
export DESTDIR

for testscript in */test.sh; do
    "${testscript}"
done
