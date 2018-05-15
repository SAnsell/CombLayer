~/CombLayerGit/Master/ess -r \
    -defaultConfig Single BIFROST \
    -angle objAxis bifrostAxis 0 \
    -v bifrostStopPoint 3 \
    --sdefVoid \
    --sdefType activation \
    --activation box 'Vec3D(600,-30,-15)' 'Vec3D(630,30,50)' \
    --activation timeStep 3 \
    --activation out Data3.ssw \
    --activation cell Cell \
    --activation nVol 10000 \
    -T tmesh DOSEPHOTON free 'Vec3D(550,-100,-80)' 'Vec3D(750,100,100)' 400 100 100 \
    -n 360000 \
    -m 1  \
    WinA


