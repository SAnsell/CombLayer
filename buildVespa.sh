#!/bin/bash

./ess \
 -defaultConfig Single VESPA \
 -r \
 -X \
 -mcnp 10 \
 -matDB shielding \
 -n 1000 \
 -angle objAxis vespaAxis 0 \
 VESPA


sed -i "s/wwp:n 8 1.4 5 -1 -2 0/wwp:n 8 1.4 5 -1 -1 0/" VESPA1.x


# -wDXT object TopFlyMidWater 0 5 1.0 \
# -wDXT objOffset G4BLine1:Beam front 'Vec3D(0,4,0)' 4.0 1.0 \
# -wDD 0.5 100 0.5 100\

# -X :: produce il foglio xml
# -x :: carica il foglio xml
# -T tmesh FLUX free 'Vec3D(100, -136, -90.3)' 'Vec3D(6300, 136, 165.7)' 310 17 16 \
# -T tmesh DOSE free 'Vec3D(-285, 4000, -150)' 'Vec3D(315, 9900, 200)' 10 59 10  \
# -w -wWWG \
#  --weightEnergyType high \
#  --weightSource 'Vec3D(201.01, 0.0, 13.7)' \
#  --weightSource 'Vec3D(5840, 0, 13.7)' \
#  --wwgRPtMesh 'Vec3D(201.01, 0.0, 13.7)' \
#  --wwgCalc SS0 0.0 1.0 0.9 1.0 1.5 \
#  --wwgCalc TS1 0.0 1.0 0.9 1.0 1.5 \
#  --wwgCADIS SS0 TS1 \
#  --wwgXMesh 100 310 6300 \
#  --wwgYMesh -136 17 136 \
#  --wwgZMesh -90.3 16 165.7 \
#  --wwgNorm 40 \
# --validCheck 1000000 \
