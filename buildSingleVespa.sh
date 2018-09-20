#!/bin/bash

./essBeamline \
 -r \
 -n 2000000000 \
 -void \
 -sdefFile sdef_isotropic \
 -matDB shielding \
 -defaultConfig Single VESPA \
 -angle objAxis vespaAxis 0 \
 -T tmesh FLUX free 'Vec3D(0, -280, -230.0)' 'Vec3D(5665, 280, 320.0)' 300 20 20 \
 VESPA
