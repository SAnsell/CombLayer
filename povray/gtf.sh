#! /usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 [ITEM]"
    echo "       ITEM is the value of ITEM in povray/tdc.pov"
    echo "       Example: povray/gtf.sh Gate"
    exit 1
fi

make -j$(nproc) maxiv

ITEM=${BASH_ARGV} # last argument is the view in povray/tdc.pov

params=" +A +W800 +H600 "
void=" -v BldBConcreteDoorDoorMat Mercury"

echo $ITEM
echo $void

trap "rm -f /tmp/gtf.txt" EXIT

./maxiv -defaultConfig Single GunTestFacility -povray $void a \
    && echo \"$ITEM\" > /tmp/gtf.txt \
    && povray ${params} povray/gtf.pov && exit 0
