#! /usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 [ITEM]"
    echo "       ITEM is one of the items in povray/danmax.pov"
    echo "       Example: povray/danmax.sh FloorShine"
    exit 1
fi

ITEM=${BASH_ARGV} # last argument is the view in povray/danmax.pov

#params=" +A +W800 +H600 "
params=" +A +Q11 +W1000 +H400 "
#params=" +A +W1800 +H1000 " # ~ max resolution

opts=" -va DanMAXOpticsHutFloorShineMat B4C"
opts+=" -v DanMAXOpticsHutChicane0PlateMat Mercury -v DanMAXOpticsHutChicane0SkinMat Air -v DanMAXOpticsHutChicane0WallMat B4C "
opts+=" -v DanMAXOpticsHutChicane1PlateMat Mercury -v DanMAXOpticsHutChicane1SkinMat Air -v DanMAXOpticsHutChicane1WallMat H2O "

echo $ITEM
echo $opts

trap "rm -f /tmp/danmax.txt" EXIT

make -j$(nproc) maxiv && ./maxiv -defaultConfig Single DANMAX  -angle objYAxis DanMAXFrontBeam 0 -offset object DanMAXFrontBeamUndulator 0 $opts -povray a \
    && echo \"$ITEM\" > /tmp/danmax.txt \
    && povray ${params} povray/danmax.pov && exit 0
