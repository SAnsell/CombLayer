#! /usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 [ITEM]"
    echo "       ITEM is the value of ITEM in povray/gtf.pov"
    echo "       Example: povray/gtf.sh Gate"
    exit 1
fi

ITEM=${BASH_ARGV} # last argument is the view in povray/gtf.pov

#params=" +A +W800 +H600 "
params=" +A +Q11 +W1000 +H400 "
#params=" +A +W1800 +H1000 " # ~ max resolution
#void=" -v BldBConcreteDoorDoorMat Mercury "

echo $ITEM
echo $void

trap "rm -f /tmp/gtf.txt" EXIT

if [ $ITEM == "Slits" ]; then
    void+=" -v LocalShieldingWallMainMat Iron -v SlitsMainMat Void -v YagUnitBMainMat Void "
    void+=" -v BellowBBellowMat Void -v  BellowBFlangeAMat Void -v BellowBFlangeBMat Void -v BellowBPipeMat Void "
    void+=" -v BellowCBellowMat Void -v  BellowCFlangeAMat Void -v BellowCFlangeBMat Void -v BellowCPipeMat Void "
    void+=" -v BellowBLength 20 -v LocalShieldingWallYStep 65 "
fi

if [ $ITEM == "Line" ]; then
    void+=" -v LocalShieldingWallMat Void "
    void+=" -v DumpBrickMat Void "
    void+=" -v BldBDoorSideShieldMat Void -v BldBDuctLaserShieldMat Void "
    void+=" -v BldBDoorBricksMat B4C "
    void+=" -v BellowAEngineeringActive 1 -v BellowBEngineeringActive 1 -v BellowBelowGunEngineeringActive 1 -v BellowCEngineeringActive 1 -v BellowDEngineeringActive 1 "
#    void+=" -v BellowBelowGunEngineeringActive 1 "
fi
make -j$(nproc) maxiv && ./maxiv -defaultConfig Single GunTestFacility -offset object Gun Cathode  -povray $void a \
    && echo \"$ITEM\" > /tmp/gtf.txt \
    && povray ${params} povray/gtf.pov && exit 0
