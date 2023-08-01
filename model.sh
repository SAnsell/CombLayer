for item in cuBlock d4cModel delft essBuild essConstruct \
    essLinac exampleBuild filter maxivBuild photon  \
    pipeBuild ralBuild saxs singleItemBuild snsBuild \
    t1Build t2Build xrayHutch
do
    echo $item;
    ./CMakeIndividual.pl Model/$item base > Model/$item/CMakeLists.txt
done;
exit	    

for item in beer bifrost common commonVar cspec dream \
    estia freia heimdal loki magic miracles nmx nnbar \
    odin simpleItem skadi testBeam trex vespa vor;
do
    echo $item;
    ./CMakeIndividual.pl Model/ESSBeam/$item ess > Model/ESSBeam/$item/CMakeLists.txt
done;

exit

for item in beamline global instrument scatMat transport test src
do
    echo $item
   ./CMakeIndividual.pl $item > $item/CMakeLists.txt
done


for item in attachComp compWeights construct constructVar crystal \
         endf flukaPhysics flukaProcess  flukaTally funcBase  \
	 generalProcess geometry input insertUnit log magnetic mcnpProcess \
	 md5 modelSupport monte objectMod phitsPhysics \
	 phitsProcess phitsSupport phitsTally physics \
	 poly simMC source support tally visit weights \
	 work world xml;
do
    echo $item;
    ./CMakeIndividual.pl System/$item > System/$item/CMakeLists.txt
done;
exit	    



for item in balder commonBeam commonGenerator cosaxs \
    danmax flexpes formax Linac maxpeem micromax R1Common \
    R3Common softimax species;
do
    echo $item;
#    ./CMakeIndividual.pl Model/MaxIV/$item > Model/MaxIV/$item/CMakeLists.txt
done;
	    
	    
for item in ralVar
do
    echo $item;
    ./CMakeIndividual.pl Model/ralBuild/$item > Model/ralBuild/$item/CMakeLists.txt
done;
	    


#./CMakeIndividual.pl System/constructVar > System/constructVar/CMakeLists.txt
