#! /usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 [ITEM]"
    echo "       ITEM is the value of ITEM in povray/singleItem.pov"
    echo "       Example: povray/singleItem.sh RFGun"
    exit 1
fi

make -j$(nproc) singleItem

ITEM=${BASH_ARGV} # last argument is the view in povray/singleItem.pov

echo $ITEM

params=" +A +W800 +H600 "
out=" +O$ITEM.png"
void=""

echo $ITEM
echo $void

trap "rm -f /tmp/povray.txt" EXIT

./singleItem -povray -singleItem $ITEM a \
    && echo \"$ITEM\" > /tmp/povray.txt \
    && povray ${params} ${out} povray/singleItem.pov && exit 0
