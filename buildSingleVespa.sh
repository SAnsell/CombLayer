#!/bin/bash

./ess \
 -defaultConfig Single VESPA \
 -r \
 -mcnp 10 \
 -angle objAxis vespaAxis 0 \
 -T tmesh FLUX free 'Vec3D(1150.0, -300.0, -186.3)' 'Vec3D(6300, 300.0, 213.7)' 100 40 5 \
 -w -wWWG \
  --weightEnergyType basic \
  --weightSource 'Vec3D(1500.0, 0.0, 13.7)' \
  --weightSource 'Vec3D(5726.0, 0.0, 13.7)' \
  --wwgCalc SS0 0.0 1.0 0.9 1.0 1.5 \
  --wwgCalc TS1 0.0 1.0 0.9 1.0 1.5 \
  --wwgCADIS SS0 TS1 \
  --wwgXMesh 1150.0 256 6300.0 \
  --wwgYMesh -200.0  40  200.0 \
  --wwgZMesh -186.3  40  213.7 \
  --wwgNorm 6 \
  VESPA
  
  
# -T tmesh FLUX free 'Vec3D(1150.0, -400.0, -386.3)' 'Vec3D(6300.0, 400.0, 413.7)' 100 40 5 \
  
#./essBeamline \
# -r \
# -n 2000000000 \
# -void \
# -sdefFile sdef_isotropic \
# -matDB shielding \
# -matDB shielding -n 5 \
# -defaultConfig Single VESPA \
# -angle objAxis vespaAxis 0 \
# -T tmesh FLUX free 'Vec3D(0, -280, -230.0)' 'Vec3D(5665, 280, 320.0)' 300 20 20 \
# VESPA
