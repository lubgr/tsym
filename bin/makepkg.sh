#!/usr/bin/env bash

PKGDIR=build/arch
PKGPATTERN='tsym-git-*-*-*.pkg.tar.xz'

if test -d ${PKGDIR}; then
    rm -rf ${PKGDIR}/{${PKGPATTERN},pkg,src}

else
    mkdir ${PKGDIR}
fi

cp doc/arch/PKGBUILD ${PKGDIR}

cd ${PKGDIR}

makepkg -fsr

echo "Install via:"

for pkg in `ls ${PKGPATTERN}`; do
    echo "sudo pacman -U ${PKGDIR}/${pkg}"
done
