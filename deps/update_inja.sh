#!/usr/bin/env sh

MYDIR="$(dirname ${0})"
cd "${MYDIR}/.."

git subtree pull --squash --prefix=deps/inja https://github.com/pantor/inja.git master
