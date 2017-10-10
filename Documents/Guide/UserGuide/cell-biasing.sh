#! /bin/bash

./ess -r -defaultConfig Single NMX -angle free 60 \
    -w \
    --weightSource 'Vec3D(200,0,0)' \
    --weightPlane  'Vec3D(200,0,0)'  'Vec3D(1,0,0)' \
    --weightPlane  'Vec3D(2800,0,0)' 'Vec3D(1,0,0)' \
    --weightEnergyType energy 0.1 0.95 1.0 0.85 10.0 0.5 100.0 0.4 5000.0 0.3 \
    --weightObject G1BLineTop1          SS0 0.0 1.0 0.9 1.0 2.0 \
    --weightObject ABunkerWallMainWall0 TP1 0.0 1.0 0.9 1.0 2.0 \
    --weightObject ABunkerWallMainWall1 TP1 0.0 1.0 0.9 1.0 2.0 \
    TA
