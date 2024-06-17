#! /usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 [ITEM]"
    echo "       ITEM is the value of ITEM in povray/singleItem.pov"
    echo "       Example: povray/singleItem.sh RFGun"
    exit 1
fi

make -j$(nproc) singleItem

ITEM=${BASH_ARGV} # last argument is the view in povray/singleItem.pov

params=" +A +W800 +H600 "
void=""

echo $ITEM
echo $void

trap "rm -f /tmp/povray.txt" EXIT

./singleItem -povray -singleItem $ITEM a \
    && echo \"$ITEM\" > /tmp/povray.txt \
    && povray ${params} povray/singleItem.pov && exit 0
