#!/usr/bin/env sh

MYDIR="$(dirname ${0})"
cd "${MYDIR}/.."

git subtree pull --squash --prefix=deps/cpp-httplib https://github.com/yhirose/cpp-httplib.git master
