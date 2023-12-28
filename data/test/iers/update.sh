#!/usr/bin/env sh

set -e

CWD=`pwd`
SCRIPTPATH=`cd "$(dirname "${0}")"; pwd -L`

cd "${SCRIPTPATH}"

echo "Updating Bulletin A: Earth orientation rapid data and predictions ..."
rm -f finals.all.iau2000.txt
curl -O https://datacenter.iers.org/data/latestVersion/finals.all.iau2000.txt

echo "Updating Bulletin B: Monthly earth orientation data ..."
rm -f eopc04_IAU2000.62-now
curl -O https://hpiers.obspm.fr/iers/eop/eopc04/eopc04_IAU2000.62-now

echo "Updating Bulletin C: Leap Seconds ..."
rm -f Leap_Second.dat
curl -O https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat

date >> version.txt

cd "${CWD}"
