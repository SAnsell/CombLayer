#! /bin/bash

./ess -r -defaultConfig Single ODIN -angle objAxis odinAxis 0 \
    -w \
    --weightEnergyType energy 0.1 0.95 1.0 0.85 10.0 0.5 100.0 0.4 5000.0 0.3 \
    --weightSource 'Vec3D(200.0,4.0,13.7)' \
    --weightPlane  'Vec3D(2800,0,0)' 'Vec3D(1,0,0)' \
    --weightObject G2BLineTop20         SS0 0.0 1.0 0.9 1.0 2.0 \
    --weightObject CBunkerWallMainWall1 TP0 0.0 1.0 0.9 1.0 2.0 \
    --weightObject CBunkerWallMainWall2 TP0 0.0 1.0 0.9 1.0 2.0 \
    --voidUnMask \
    TA
