#!/bin/bash

./ess \
 -defaultConfig Single VESPA \
 -r \
 -mcnp 10 \
 -matDB shielding \
 -n 1000 \
 -angle objAxis vespaAxis 0 \
 -T tmesh FLUX free 'Vec3D(100, -136, -90.3)' 'Vec3D(6300, 136, 165.7)' 310 17 16 \
 -w -wWWG \
  --weightEnergyType energy 10 1.0 \
  --weightSource 'Vec3D(201.01, 0.0, 13.7)' \
  --weightSource 'Vec3D(5840, 0, 13.7)' \
  --wwgRPtMesh 'Vec3D(201.01, 0.0, 13.7)' \
  --wwgCalc SS0 0.0 1.0 0.9 1.0 1.5 \
  --wwgCalc TS1 0.0 1.0 0.9 1.0 1.5 \
  --wwgCADIS SS0 TS1 \
  --wwgXMesh -100 300 6300 \
  --wwgYMesh -100 50 100 \
  --wwgZMesh 13 1 14 \
  --wwgNorm 40 \
 VESPA
 
sed -i "s/wwp:n 8 1.4 5 -1 -2 0/wwp:n 8 1.4 5 -1 -1 0/" VESPA1.x



# STUART:
#
#./ess \
# -defaultConfig Single VESPA \
# -r \
# -mcnp 10 \
# -matDB shielding \
# -n 1000 \
# -angle objAxis vespaAxis 0 \
# -T tmesh FLUX free 'Vec3D(100, -136, -90.3)' 'Vec3D(6300, 136, 165.7)' 310 17 16 \
# -w -wWWG \
#  --weightEnergyType energy 10 1.0 \
#  --weightSource 'Vec3D(201.01, 0.0, 13.7)' \
#  --weightSource 'Vec3D(5840, 0, 13.7)' \
#  --wwgRPtMesh 'Vec3D(201.01, 0.0, 13.7)' \
#  --wwgCalc SS0 0.0 1.0 0.9 1.0 1.5 \
#  --wwgCalc TS1 0.0 1.0 0.9 1.0 1.5 \
#  --wwgCADIS SS0 TS1 \
#  --wwgXMesh -100 300 6300 \
#  --wwgYMesh -100 50 100 \
#  --wwgZMesh 13 1 14 \
#  --wwgNorm 40 \
# VESPA




#  --weightSource 'Vec3D(5840, 0, 13.7)' \

# --validCheck 1000

#  --wwgCalc TS1 0.0 1.0 0.9 1.0 1.5 \  # TS1 or TS0?
#  --wwgCADIS SS0 \                     # SS0 or TS1? how many times do I have to write that? in BeamLine/Vespa/short è chiamato due volte con TP0 TP0


# -w -wWWG \
#  --weightEnergyType high \
#  --weightSource 'Vec3D(201.01, 0.0, 13.7)' \
#  --weightSource 'Vec3D(5840, 0, 30)' \
#  --wwgCalc SS0 0.0 1.0 0.9 1.0 1.5 \
#  --wwgCalc TS1 0.0 1.0 0.9 1.0 1.5 \
#  --wwgCADIS SS0 \
#  --wwgXMesh -2810 100 6300 \
#  --wwgYMesh -2810 60 2500 \
#  --wwgZMesh -770 25 1010 \
#  --wwgNorm 4 \ 


#/Users/valentinasantoro/CombLayer7June2017/CombLayer/ess -r \
#            -defaultConfig Single NMX G1BLineTop11 \
#            -angle object nmxAxis -1 1 \
#            -va nmxMainShutterLiftZStep 0 \
#            -v nmxMainShutterYStep 37 \
#            -va nmxMainShutterWidth 17 \
#            -va nmxMainShutterHeight 14 \
#            -va nmxPipeCWindowMat Cadmium \
#            -va nmxBInsertWallMat Stainless304 \
#            -wDXT objOffset nmxFWallGuide0 back 'Vec3D(0,4,0)' 20.0 1.0 \
#            -wDD -2.03333E-05 100 \
#            -TAdd plate object nmxBInsert -1 'Vec3D(0.,-10.,17.6)' 30 20.0 20 Stainless304 \
#            -TAdd plate object nmxBInsert -1 'Vec3D(0.,-10,-17.6)' 30 20.0 20 Stainless304  \
#            -TAdd plate object nmxBInsert -1 'Vec3D(24.5,-10.,0)' 20 30.0 20 Stainless304  \
#            -TAdd plate object nmxBInsert -1 'Vec3D(-24.5,-10,0)' 20 30.0 20 Stainless304   \
#            -T mesh free DOSE 'Vec3D(200,-60,-40)' 'Vec3D(1700,60,80.0)' 150 24 24 \
#            -n 10000000 \
#TESTBEAMLINE


 
# -T tmesh DOSE free 'Vec3D(-285, 4000, -150)' 'Vec3D(315, 9900, 200)' 10 59 10  \
# -T tmesh DOSEPHOTON free 'Vec3D(-285, 4000, -150)' 'Vec3D(315, 9900, 200)' 10 59 10  \
# -T flux n -1 insertPlate0 \
# -TMod single -4 \
# -TMod energy 4 "0 75" \

# -w -wWWG \
#  --weightSource 'Vec3D(201, 0.0, 13.7)' \
#  --weightPlane 'Vec3D(1500.0, 0.0, 13.7)' 'Vec3D(-1, 0, 0)' \
#  --wwgCalc SS0 1.0 \
#  --wwgCalc TP0 -2 1 2 -3 \
#  --wwgCADIS SS1 \     ?2 parameters. 
#  --wwgXMesh 0 600 6000 \      /? 600 is too much
#  --wwgYMesh -250 50 250 \
#  --wwgZMesh -250 50 250 \
#  --weightEnergyType 1.0 \
 
# -T tmesh DOSE free 'Vec3D(0, -100, -100)' 'Vec3D(6000, 100, 100)' 100 10 10 \
# -T tmesh FLUX free 'Vec3D(194, -12, 2)' 'Vec3D(5761, 12, 26)' 2784 24 24 \
# -wDXT object vespaFWall 0 25 10.0 \
# -wDD 0.5 100 0.5 100 \
# -offset object vespaFA 0 \
# -wDXT object TopFlyMidWater 0 [Raggio] 1.0 \
# -wDXT objOffset G4BLine1:Beam front 'Vec3D(0,4,0)' 4.0 1.0 \

