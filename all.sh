
nValid=1000

#segments=$(for i in {40..49}; do echo -n "Segment$i "; done)

./ess -validAll --validCheck ${nValid} AA || exit 
./ess --defaultConfig Single  ESTIA --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  CSPEC --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  ODIN --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  MAGIC --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  BIFROST --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  HEIMDAL --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  LOKI --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  NMX --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  NNBAR --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  DREAM --validAll --validCheck ${nValid} AA  || exit
./ess --defaultConfig Single  BEER --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  FREIA --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  SKADI --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  MIRACLES --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  TESTBEAM --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  TREX --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  VESPA --validAll --validCheck ${nValid} AA || exit
./ess --defaultConfig Single  VOR --validAll --validCheck ${nValid} AA || exit

segments=All
./maxiv --noLengthCheck --defaultConfig Linac ${segments} AA

./maxiv --noLengthCheck --defaultConfig Linac ${segments} -validAll --validCheck ${nValid} AA || exit
./fullBuild -validAll --validCheck ${nValid} AA || exit

./t1Real -validAll --validCheck ${nValid} AA || exit
./reactor -validAll --validCheck 100 AA || exit
./maxiv --defaultConfig Single  BALDER --validAll --validCheck ${nValid} AA || exit
./maxiv --defaultConfig Single  COSAXS --validAll --validCheck ${nValid} AA || exit
./maxiv --defaultConfig Single  DANMAX --validAll --validCheck ${nValid} AA || exit
./maxiv --defaultConfig Single  FLEXPES --validAll --validCheck ${nValid} AA || exit
./maxiv --defaultConfig Single  FORMAX --validAll --validCheck ${nValid} AA || exit
./maxiv --defaultConfig Single  MAXPEEM --validAll --validCheck ${nValid} AA || exit
./maxiv --defaultConfig Single  MICROMAX --validAll --validCheck ${nValid} AA || exit
./maxiv --defaultConfig Single  SPECIES --validAll --validCheck ${nValid} AA || exit
./maxiv --defaultConfig Single  SOFTIMAX --validAll --validCheck ${nValid} AA || exit


./singleItem --singleItem BeamDivider       --validCheck ${nValid} AA || exit
./singleItem --singleItem BeamScrapper      --validCheck ${nValid} AA || exit
./singleItem --singleItem Bellow            --validCheck ${nValid} AA || exit
./singleItem --singleItem BlankTube         --validCheck ${nValid} AA || exit
./singleItem --singleItem BoxJaws           --validCheck ${nValid} AA || exit
./singleItem --singleItem BremBlock         --validCheck ${nValid} AA || exit
./singleItem --singleItem BremTube          --validCheck ${nValid} AA || exit
./singleItem --singleItem ButtonBPM         --validCheck ${nValid} AA || exit
./singleItem --singleItem CRLTube           --validCheck ${nValid} AA || exit
./singleItem --singleItem CeramicGap        --validCheck ${nValid} AA || exit
./singleItem --singleItem CleaningMagnet    --validCheck ${nValid} AA || exit
./singleItem --singleItem CollTube          --validCheck ${nValid} AA || exit
./singleItem --singleItem ConnectorTube     --validCheck ${nValid} AA || exit
./singleItem --singleItem CooledScreen      --validCheck ${nValid} AA || exit
./singleItem --singleItem CooledUnit        --validCheck ${nValid} AA || exit
./singleItem --singleItem CorrectorMag      --validCheck ${nValid} AA || exit
./singleItem --singleItem CrossBlank        --validCheck ${nValid} AA || exit
./singleItem --singleItem CrossWay          --validCheck ${nValid} AA || exit
./singleItem --singleItem CylGateValve      --validCheck ${nValid} AA || exit
./singleItem --singleItem DiffPumpXIADP03   --validCheck ${nValid} AA || exit
./singleItem --singleItem DipoleDIBMag      --validCheck ${nValid} AA || exit
./singleItem --singleItem DipoleExtract     --validCheck ${nValid} AA || exit
./singleItem --singleItem DipoleSndBend     --validCheck ${nValid} AA || exit
./singleItem --singleItem EArrivalMon       --validCheck ${nValid} AA || exit
./singleItem --singleItem EBeamStop         --validCheck ${nValid} AA || exit
./singleItem --singleItem EPSeparator       --validCheck ${nValid} AA || exit
./singleItem --singleItem ExperimentalHutch --validCheck ${nValid} AA || exit
./singleItem --singleItem FMask             --validCheck ${nValid} AA || exit

./singleItem --singleItem FlangeDome        --validCheck ${nValid} AA || exit
./singleItem --singleItem FlatPipe          --validCheck ${nValid} AA || exit
./singleItem --singleItem FourPort          --validCheck ${nValid} AA || exit
./singleItem --singleItem GateValveCube     --validCheck ${nValid} AA || exit
./singleItem --singleItem GateValveCylinder --validCheck ${nValid} AA || exit
./singleItem --singleItem GaugeTube         --validCheck ${nValid} AA || exit
./singleItem --singleItem HPCombine         --validCheck ${nValid} AA || exit
./singleItem --singleItem HPJaws            --validCheck ${nValid} AA || exit
./singleItem --singleItem HalfElectronPipe  --validCheck ${nValid} AA || exit
./singleItem --singleItem IonGauge          --validCheck ${nValid} AA || exit
./singleItem --singleItem IonPTube          --validCheck ${nValid} AA || exit
./singleItem --singleItem Jaws              --validCheck ${nValid} AA || exit
./singleItem --singleItem LQuadF            --validCheck ${nValid} AA || exit
./singleItem --singleItem LQuadH            --validCheck ${nValid} AA || exit


./singleItem --singleItem LSexupole       --validCheck ${nValid} AA || exit      
./singleItem --singleItem LocalShield     --validCheck ${nValid} AA || exit      
./singleItem --singleItem MagTube         --validCheck ${nValid} AA || exit

./singleItem --singleItem MagnetBlock     --validCheck ${nValid} AA || exit      
./singleItem --singleItem MagnetM1        --validCheck ${nValid} AA || exit
./singleItem --singleItem MagnetU1        --validCheck ${nValid} AA || exit      
./singleItem --singleItem MonoShutter     --validCheck ${nValid} AA || exit      
./singleItem --singleItem MultiPipe       --validCheck ${nValid} AA || exit
./singleItem --singleItem NBeamStop       --validCheck ${nValid} AA || exit      
./singleItem --singleItem Octupole        --validCheck ${nValid} AA || exit      
./singleItem --singleItem PipeTube        --validCheck ${nValid} AA || exit 
./singleItem --singleItem PortTube        --validCheck ${nValid} AA || exit 
./singleItem --singleItem PrismaChamber   --validCheck ${nValid} AA || exit

./singleItem --singleItem Quadrupole      --validCheck ${nValid} AA || exit

./singleItem --singleItem R3ChokeChamber     --validCheck ${nValid} AA || exit
./singleItem --singleItem RoundMonoShutter   --validCheck ${nValid} AA || exit      
./singleItem --singleItem Scrapper 	     --validCheck ${nValid} AA || exit
./singleItem --singleItem Sexupole 	     --validCheck ${nValid} AA || exit                              
./singleItem --singleItem SixPort  	     --validCheck ${nValid} AA || exit
./singleItem --singleItem StriplineBPM       --validCheck ${nValid} AA || exit      

./singleItem --singleItem TWCavity 	     --validCheck ${nValid} AA || exit

./singleItem --singleItem TargetShield        --validCheck ${nValid} AA || exit    
./singleItem --singleItem TriGroup 	      --validCheck ${nValid} AA || exit
./singleItem --singleItem TriPipe  	      --validCheck ${nValid} AA || exit
./singleItem --singleItem TriggerTube         --validCheck ${nValid} AA || exit     
./singleItem --singleItem UndVac   	      --validCheck ${nValid} AA || exit
./singleItem --singleItem UndulatorVacuum     --validCheck ${nValid} AA || exit     
./singleItem --singleItem VacuumPipe          --validCheck ${nValid} AA || exit     
./singleItem --singleItem ViewTube 	      --validCheck ${nValid} AA || exit                             
./singleItem --singleItem YAG      	      --validCheck ${nValid} AA || exit
./singleItem --singleItem YagScreen           --validCheck ${nValid} AA || exit 
./singleItem --singleItem YagUnit             --validCheck ${nValid} AA || exit
./singleItem --singleItem YagUnitBig          --validCheck ${nValid} AA || exit    
./singleItem --singleItem default             --validCheck ${nValid} AA || exit 
./singleItem --singleItem uVac                --validCheck ${nValid} AA || exit
