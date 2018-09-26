#!/bin/bash

./ess \
 -defaultConfig Single VESPA \
 -r \
 -mcnp 10 \
 -matDB shielding -n 5 \
 -angle objAxis vespaAxis 0 \
 -T tmesh FLUX free 'Vec3D(-1600, -1600, -100)' 'Vec3D(2600, 1600, 100)' 100 40 5 \
 -w -wWWG \
  --weightEnergyType basic \
  --weightSource 'Vec3D(201.01, 0.0, 13.7)' \
  --weightSource 'Vec3D(5840, 0, 30)' \
  --wwgCalc SS0 0.0 1.0 0.9 1.0 1.5 \
  --wwgCalc TS1 0.0 1.0 0.9 1.0 1.5 \
  --wwgCADIS SS0 \
  --wwgXMesh -3000 10 6500 \
  --wwgYMesh -3000 10 1700 \
  --wwgZMesh -450 10 450 \
  --wwgNorm 6 \
  VESPA
  
#./essBeamline \
# -r \
# -n 2000000000 \
# -void \
# -sdefFile sdef_isotropic \
# -matDB shielding \
# -defaultConfig Single VESPA \
# -angle objAxis vespaAxis 0 \
# -T tmesh FLUX free 'Vec3D(0, -280, -230.0)' 'Vec3D(5665, 280, 320.0)' 300 20 20 \
# VESPA
