#! /usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 [ITEM]"
    echo "       ITEM is the value of ITEM in povray/tdc.pov"
    echo "       Example: povray/gtf.sh Gate"
    exit 1
fi

ITEM=${BASH_ARGV} # last argument is the view in povray/tdc.pov

params=" +A +W800 +H600 "
void=" -v BldBConcreteDoorDoorMat Mercury "

echo $ITEM
echo $void

trap "rm -f /tmp/gtf.txt" EXIT

if [ $ITEM == "Slits" ]; then
    void+=" -v LocalShieldingWallMainMat Iron -v SlitsMainMat Void -v YagUnitBMainMat Void "
    void+=" -v BellowBBellowMat Void -v  BellowBFlangeAMat Void -v BellowBFlangeBMat Void -v BellowBPipeMat Void "
    void+=" -v BellowCBellowMat Void -v  BellowCFlangeAMat Void -v BellowCFlangeBMat Void -v BellowCPipeMat Void "
    void+=" -v BellowBLength 20 -v LocalShieldingWallYStep 65 "
fi
make -j$(nproc) maxiv && ./maxiv -defaultConfig Single GunTestFacility -offset object Gun Cathode  -povray $void a \
    && echo \"$ITEM\" > /tmp/gtf.txt \
    && povray ${params} povray/gtf.pov && exit 0
