./ess -r \
    -mcnp 10 \
    -C 0.1 \
    -defaultConfig Single BIFROST \
    -angle objAxis bifrostAxis 0 \
    -v bifrostStopPoint 3 \
    -T flux n allNonVoid bifrostChopperAIPortA \
    -T flux n allNonVoid bifrostChopperAIPortB \
    -cinder -TMod single -4 \
    -n 80000 \
   ChopHA 
