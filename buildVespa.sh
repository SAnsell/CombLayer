#!/bin/bash

~/CombLayer-VESPA/ess \
 -defaultConfig Single VESPA \
 -r \
 -mcnp 10 \
 -matDB shielding \
 -n 1000000000 \
 -angle objAxis vespaAxis 0 \
 -T tmesh FLUX free 'Vec3D(100, -136, -90.3)' 'Vec3D(6300, 136, 165.7)' 310 17 16 \
 -w -wWWG \
  --weightEnergyType high \
  --weightSource 'Vec3D(201.01, 0.0, 13.7)' \
  --weightSource 'Vec3D(5840, 0, 13.7)' \
  --wwgRPtMesh 'Vec3D(201.01, 0.0, 13.7)' \
  --wwgCalc SS0 0.0 1.0 0.9 1.0 1.5 \
  --wwgCalc TS1 0.0 1.0 0.9 1.0 1.5 \
  --wwgCADIS SS0 TS1 \
  --wwgXMesh 100 310 6300 \
  --wwgYMesh -136 17 136 \
  --wwgZMesh -90.3 16 165.7 \
  --wwgNorm 40 \
 VESPA
 
sed -i "s/wwp:n 8 1.4 5 -1 -2 0/wwp:n 8 1.4 5 -1 -1 0/" VESPA1.x
