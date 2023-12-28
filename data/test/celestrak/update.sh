#!/usr/bin/env sh

set -e

CWD=`pwd`
SCRIPTPATH=`cd "$(dirname "${0}")"; pwd -L`

cd "${SCRIPTPATH}"

echo "Updating active satellite elements ..."
rm -f active.txt
curl -O https://celestrak.org/NORAD/elements/active.txt

cd "${CWD}"
