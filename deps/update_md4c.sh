#!/usr/bin/env sh

MYDIR="$(dirname ${0})"
cd "${MYDIR}/.."

git subtree pull --squash --prefix=deps/md4c https://github.com/mity/md4c.git master
