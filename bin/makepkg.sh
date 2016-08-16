#!/usr/bin/env bash

PKGDIR=build/arch
PKGPATTERN='tsym-git-*-*-*.pkg.tar.xz'

if test -d ${PKGDIR}; then
    rm -f ${PKGDIR}/${PKGPATTERN}
else
    mkdir ${PKGDIR}
fi

cp doc/arch/PKGBUILD ${PKGDIR}

cd ${PKGDIR}

makepkg -fsr

cmd="sudo pacman -U ${PKGDIR}/`ls ${PKGPATTERN}`"

echo -e "Install via:\n${cmd}"
