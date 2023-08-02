for item in beamline global instrument scatMat transport src
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
	 work world xml
do
    echo $item;
    ./CMakeIndividual.pl System/$item > System/$item/CMakeLists.txt
done;


for item in balder commonBeam commonGenerator cosaxs \
    danmax flexpes formax Linac maxpeem micromax R1Common \
    R3Common softimax species;
do
    echo $item;
    ./CMakeIndividual.pl Model/MaxIV/$item maxiv > Model/MaxIV/$item/CMakeLists.txt
done;


for item in cuBlock d4cModel delft  essConstruct \
    essLinac exampleBuild   photon  \
    pipeBuild ralBuild saxs  \
    xrayHutch
do
    echo $item;
    ./CMakeIndividual.pl Model/$item base > Model/$item/CMakeLists.txt
done;

./CMakeIndividual.pl Model/essBuild base ess > Model/essBuild/CMakeLists.txt
./CMakeIndividual.pl Model/t1Build base ral > Model/t1Build/CMakeLists.txt
./CMakeIndividual.pl Model/t2Build base ral > Model/t2Build/CMakeLists.txt
./CMakeIndividual.pl Model/maxivBuild base maxiv > Model/maxivBuild/CMakeLists.txt
./CMakeIndividual.pl Model/singleItemBuild ess base maxiv ral > Model/singleItemBuild/CMakeLists.txt
./CMakeIndividual.pl test base > test/CMakeLists.txt

./CMakeIndividual.pl Model/filter photon base > Model/filter/CMakeLists.txt
./CMakeIndividual.pl Model/snsBuild ral base > Model/snsBuild/CMakeLists.txt

exit	    

for item in beer bifrost common commonVar cspec dream \
    estia freia heimdal loki magic miracles nmx nnbar \
    odin simpleItem skadi testBeam trex vespa vor
do
    echo $item;
    ./CMakeIndividual.pl Model/ESSBeam/$item ess > Model/ESSBeam/$item/CMakeLists.txt
done;

	    
	    
for item in ralVar
do
    echo $item;
    ./CMakeIndividual.pl Model/ralBuild/$item > Model/ralBuild/$item/CMakeLists.txt
done;
	    


#./CMakeIndividual.pl System/constructVar > System/constructVar/CMakeLists.txt
