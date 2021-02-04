#! /usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 segment_name(s) [ITEM]"
    echo "       ITEM is value of ITEM in povray/tdc.pov"
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
    void+="-v InjectionHallPillarMat Void -v InjectionHallBTGMat Void -v InjectionHallSoilMat Void "
    void+="-v InjectionHallBackWallMat Void -v InjectionHallWallIronMat Void "
fi

./maxiv   -defaultConfig LINAC ${segments} -povray $void a \
    && povray +A +W800 +H600 povray/tdc.pov <<< \"$ITEM\" && exit 0
#    && povray +A +W1600 +H1200 povray/tdc.pov <<< \"$ITEM\" && exit 0
