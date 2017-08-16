#! /bin/bash

./ess -r -photon 0.5 -C 1e-3 -sdefVoid -n 30000 -w \
    --weightEnergyType energy 0.1 0.95 1.0 0.85 10.0 0.5 100.0 0.4 5000.0 0.3 \
    --weightPlane 'Vec3D(600,0,0)' 'Vec3D(0,1,0)' \
    --weightPlane 'Vec3D(1600,0,0)' 'Vec3D(1,1,0)' \
    --weightObject BBunkerWallMainWall1 TP1 1.0 0.15 1e-20 \
    -T mesh free DOSE 'Vec3D(1465,-600,10)' 'Vec3D(3660,360,20)' 30 60 50 \
    --voidUnMask TA
