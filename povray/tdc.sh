#! /usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 segment_name(s) [ITEM]"
    echo "       ITEM is the value of ITEM in povray/tdc.pov"
    echo "            if not specified, last segment name is used as ITEM"
    echo "       Example: povray/tdc.sh  Segment47 Segment48 Segment49 SPF48ShieldA"
    exit 1
fi

test -e maxiv || make -j$(nproc) maxiv

ITEM=${BASH_ARGV} # last argument is the view in povray/tdc.pov

# check if last argument is a segment name
if echo  $ITEM | egrep "Segment[[:digit:]][[:digit:]]?$"; then
    segments=$@
else # last argument is not a segment name but the ITEM, so remove
     # it from the list of segments
    segments="${@:1:$(($#-1))}"
fi

void=""
if [ $1 != " " ]; then # we build the beam line, therefore remove the InjectionHall walls
    void=" -v InjectionHallFloorMat Void -v InjectionHallRoofMat Void -v InjectionHallWallMat Void "
    void+=" -v InjectionHallPillarMat Void -v InjectionHallBTGMat Void -v InjectionHallSoilMat Void "
    void+=" -v InjectionHallBackWallMat Void -v InjectionHallWallIronMat Void "
    void+=" -v InjectionHallTHzMat Void -v InjectionHallMidTFrontLShieldMat Void "
fi

if [ "$segments" == "All" -a $ITEM == "SPF32DipoleA" ]; then
    void=""
elif [ "$segments" == "Segment16 Segment30 Segment31" -a $ITEM == "TDC16CMagH" ]; then
    void=""
elif [ $ITEM == "MidTFrontLShield" -o $ITEM == "L2SPF3ShieldA" -o $ITEM == "L2SPF10ShieldA" -o $ITEM == "BTG" ]; then
    void=""
elif [ $ITEM == "L2SPF2YagUnit" ]; then
    void+="-v L2SPF2ShieldAMainMat Void "
elif [ $ITEM == "L2SPF2YagScreenInBeam" ]; then
    void+="-v L2SPF2ShieldAMainMat Void -v L2SPF2YagUnitMainMat Void -v L2SPF2YagScreenScreenMat Wax -v L2SPF2YagScreenMirrorMat Copper -v L2SPF2YagScreenInBeam 1 "
elif [ $ITEM == "L2SPF2YagScreen" ]; then
    void+="-v L2SPF2ShieldAMainMat Void -v L2SPF2YagUnitMainMat Void "
fi


 echo $ITEM
 echo ${segments}
 echo $void

 ./maxiv   -defaultConfig LINAC ${segments} -povray $void a \
    && povray +A +W800 +H600 povray/tdc.pov <<< \"$ITEM\" && exit 0
#    && povray +Q0 +W400 +H300 povray/tdc.pov <<< \"$ITEM\" && exit 0
#    && povray +A +W1600 +H1200 povray/tdc.pov <<< \"$ITEM\" && exit 0
