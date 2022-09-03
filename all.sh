function procExit
{
    if [ $? != 0 ]
    then
       exit
    fi
}

#segments=$(for i in {40..49}; do echo -n "Segment$i "; done)

segments=All
#./maxiv --noLengthCheck --defaultConfig Linac ${segments} AA
./maxiv --noLengthCheck --defaultConfig Linac ${segments} -validAll --validCheck 1000 AA; procExit
exit

./ess -validAll --validCheck 1000 AA; procExit 

./fullBuild -validAll --validCheck 1000 AA; procExit 
./t1Real -validAll --validCheck 1000 AA; procExit
./reactor -validAll --validCheck 100 AA; procExit
./maxiv --defaultConfig Single  BALDER --validAll --validCheck 1000 AA; procExit
./maxiv --defaultConfig Single  COSAXS --validAll --validCheck 1000 AA; procExit
./maxiv --defaultConfig Single  DANMAX --validAll --validCheck 1000 AA; procExit
./maxiv --defaultConfig Single  FLEXPES --validAll --validCheck 1000 AA; procExit
./maxiv --defaultConfig Single  FORMAX --validAll --validCheck 1000 AA; procExit
./maxiv --defaultConfig Single  MAXPEEM --validAll --validCheck 1000 AA; procExit
./maxiv --defaultConfig Single  MICROMAX --validAll --validCheck 1000 AA; procExit
./maxiv --defaultConfig Single  SPECIES --validAll --validCheck 10000 AA; procExit
./maxiv --defaultConfig Single  SOFTIMAX --validAll --validCheck 1000 AA; procExit

./singleItem --singleItem BeamDivider       --validCheck 1000 AA; procExit
./singleItem --singleItem BeamScrapper      --validCheck 1000 AA; procExit
./singleItem --singleItem Bellow            --validCheck 1000 AA; procExit
./singleItem --singleItem BlankTube         --validCheck 1000 AA; procExit
./singleItem --singleItem BoxJaws           --validCheck 1000 AA; procExit
./singleItem --singleItem BremBlock         --validCheck 1000 AA; procExit
./singleItem --singleItem BremTube          --validCheck 1000 AA; procExit
./singleItem --singleItem ButtonBPM         --validCheck 1000 AA; procExit
./singleItem --singleItem CRLTube           --validCheck 1000 AA; procExit
./singleItem --singleItem CeramicGap        --validCheck 1000 AA; procExit
./singleItem --singleItem CleaningMagnet    --validCheck 1000 AA; procExit
./singleItem --singleItem CollTube          --validCheck 1000 AA; procExit
./singleItem --singleItem ConnectorTube     --validCheck 1000 AA; procExit
./singleItem --singleItem CooledScreen      --validCheck 1000 AA; procExit
./singleItem --singleItem CooledUnit        --validCheck 1000 AA; procExit
./singleItem --singleItem CorrectorMag      --validCheck 1000 AA; procExit
./singleItem --singleItem CrossBlank        --validCheck 1000 AA; procExit
./singleItem --singleItem CrossWay          --validCheck 1000 AA; procExit
./singleItem --singleItem CylGateValve      --validCheck 1000 AA; procExit
./singleItem --singleItem DiffPumpXIADP03   --validCheck 1000 AA; procExit
./singleItem --singleItem DipoleDIBMag      --validCheck 1000 AA; procExit
./singleItem --singleItem DipoleExtract     --validCheck 1000 AA; procExit
./singleItem --singleItem DipoleSndBend     --validCheck 1000 AA; procExit
./singleItem --singleItem EArrivalMon       --validCheck 1000 AA; procExit
./singleItem --singleItem EBeamStop         --validCheck 1000 AA; procExit
./singleItem --singleItem EPSeparator       --validCheck 1000 AA; procExit
./singleItem --singleItem ExperimentalHutch --validCheck 1000 AA; procExit
./singleItem --singleItem FMask             --validCheck 1000 AA; procExit

./singleItem --singleItem FlangeDome        --validCheck 1000 AA; procExit
./singleItem --singleItem FlatPipe          --validCheck 1000 AA; procExit
./singleItem --singleItem FourPort          --validCheck 1000 AA; procExit
./singleItem --singleItem GateValveCube     --validCheck 1000 AA; procExit
./singleItem --singleItem GateValveCylinder --validCheck 1000 AA; procExit
./singleItem --singleItem GaugeTube         --validCheck 1000 AA; procExit
./singleItem --singleItem HPCombine         --validCheck 1000 AA; procExit
./singleItem --singleItem HPJaws            --validCheck 1000 AA; procExit
./singleItem --singleItem HalfElectronPipe  --validCheck 1000 AA; procExit
./singleItem --singleItem IonGauge          --validCheck 1000 AA; procExit
./singleItem --singleItem IonPTube          --validCheck 1000 AA; procExit
./singleItem --singleItem Jaws              --validCheck 1000 AA; procExit
./singleItem --singleItem LQuadF            --validCheck 1000 AA; procExit
./singleItem --singleItem LQuadH            --validCheck 1000 AA; procExit


./singleItem --singleItem LSexupole       --validCheck 1000 AA; procExit      
./singleItem --singleItem LocalShield     --validCheck 1000 AA; procExit      
./singleItem --singleItem MagTube         --validCheck 1000 AA; procExit

./singleItem --singleItem MagnetBlock     --validCheck 1000 AA; procExit      
./singleItem --singleItem MagnetM1        --validCheck 1000 AA; procExit
./singleItem --singleItem MagnetU1        --validCheck 1000 AA; procExit      
./singleItem --singleItem MonoShutter     --validCheck 1000 AA; procExit      
./singleItem --singleItem MultiPipe       --validCheck 1000 AA; procExit
./singleItem --singleItem NBeamStop       --validCheck 1000 AA; procExit      
./singleItem --singleItem Octupole        --validCheck 1000 AA; procExit      
./singleItem --singleItem PipeTube        --validCheck 1000 AA; procExit 
./singleItem --singleItem PortTube        --validCheck 1000 AA; procExit 
./singleItem --singleItem PrismaChamber   --validCheck 1000 AA; procExit

./singleItem --singleItem Quadrupole      --validCheck 1000 AA; procExit

./singleItem --singleItem R3ChokeChamber     --validCheck 1000 AA; procExit
./singleItem --singleItem RoundMonoShutter   --validCheck 1000 AA; procExit      
./singleItem --singleItem Scrapper 	     --validCheck 1000 AA; procExit
./singleItem --singleItem Sexupole 	     --validCheck 1000 AA; procExit                              
./singleItem --singleItem SixPort  	     --validCheck 1000 AA; procExit
./singleItem --singleItem StriplineBPM       --validCheck 1000 AA; procExit      

./singleItem --singleItem TWCavity 	     --validCheck 1000 AA; procExit

./singleItem --singleItem TargetShield        --validCheck 1000 AA; procExit    
./singleItem --singleItem TriGroup 	      --validCheck 1000 AA; procExit
./singleItem --singleItem TriPipe  	      --validCheck 1000 AA; procExit
./singleItem --singleItem TriggerTube         --validCheck 1000 AA; procExit     
./singleItem --singleItem UndVac   	      --validCheck 1000 AA; procExit
./singleItem --singleItem UndulatorVacuum     --validCheck 1000 AA; procExit     
./singleItem --singleItem VacuumPipe          --validCheck 1000 AA; procExit     
./singleItem --singleItem ViewTube 	      --validCheck 1000 AA; procExit                             
./singleItem --singleItem YAG      	      --validCheck 1000 AA; procExit
./singleItem --singleItem YagScreen           --validCheck 1000 AA; procExit 
./singleItem --singleItem YagUnit             --validCheck 1000 AA; procExit
./singleItem --singleItem YagUnitBig          --validCheck 1000 AA; procExit    
./singleItem --singleItem default             --validCheck 1000 AA; procExit 
./singleItem --singleItem uVac                --validCheck 1000 AA; procExit
