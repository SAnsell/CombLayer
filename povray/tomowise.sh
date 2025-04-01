#! /usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 [ITEM]"
    echo "       ITEM is the value of ITEM in povray/tdc.pov"
    echo "       Example: povray/tomowise.sh Gate"
    exit 1
fi

ITEM=${BASH_ARGV} # last argument is the view in povray/tomowise.pov

#params=" +A +W800 +H600 "
params=" +A +Q11 +W1000 +H400 "
#params=" +A +W1800 +H1000 " # ~ max resolution
#void=" -v BldBConcreteDoorDoorMat Mercury "

echo $ITEM
echo $void

trap "rm -f /tmp/tomowise.txt" EXIT

make -j$(nproc) maxiv && ./maxiv -defaultConfig Single TOMOWISE  -povray $void a \
    && echo \"$ITEM\" > /tmp/tomowise.txt \
    && povray ${params} povray/tomowise.pov && exit 0
