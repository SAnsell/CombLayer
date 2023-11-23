


nValid=1000

#./maxiv --defaultConfig Single SOFTIMAX AA 
#./maxiv --defaultConfig Single SOFTIMAX --validRandom 190  --validCheck $nValid AA || exit
# exit
#segments=$(for i in {40..49}; do echo -n "Segment$i "; done)
segments=All
./singleItem --singleItem M1detail --validAll --validCheck ${nValid} AA 
exit

./singleItem --singleItem UTubePipe --validAll --validCheck $nValid AA 
#exit


# ./ess --defaultConfig Single VESPA --validAll --validCheck $nValid AA  || exit
# ./ess --defaultConfig Single HEIMDAL --validAll --validCheck $nValid AA  || exit

## SOFTIMAX removed because making the new M1 mirror
parallel --halt now,fail=1 "./maxiv --defaultConfig Single {} --validAll --validCheck $nValid AA" ::: \
   BALDER COSAXS DANMAX FORMAX MICROMAX SPECIES MAXPEEM || exit


./maxiv --noLengthCheck --defaultConfig Linac ${segments} --validAll --validCheck $nValid AA || exit 


./t1Real -validAll --validCheck ${nValid} AA || exit
./reactor -validAll --validCheck ${nValid} AA || exit
./saxs -validAll --validCheck ${nValid} AA || exit

parallel --halt now,fail=1 "./ess --topModType {} --validAll --validCheck $nValid AA" ::: \
     Butterfly Pancake 
./ess --bunkerPillars ABunker --validAll --validCheck $nValid AA  || exit

parallel --halt now,fail=1 "./ess --defaultConfig Single {} --validAll --validCheck $nValid AA" ::: \
 ESTIA CSPEC  ODIN MAGIC BIFROST LOKI NMX  NNBAR  DREAM  BEER   \
 FREIA SKADI MIRACLES TESTBEAM TREX VESPA VOR      || exit
# HEIMDAL :: Not currently correct -- update underway

parallel --halt now,fail=1 "./singleItem --singleItem {} --validAll --validCheck $nValid AA" ::: \
 BeamDivider BeamScrapper Bellow BlankTube BoxJaws         \
 BremBlock BremTube  ButtonBPM CRLTube  CeramicGap CleaningMagnet  \
 CollTube ConnectorTube CooledScreen CooledUnit CornerPipe \
 CorrectorMag CrossBlank   \
 CrossWay CylGateValve DiffPumpXIADP03 DipoleDIBMag DipoleExtract       \
 DipoleSndBend EArrivalMon EBeamStop EPSeparator ExperimentalHutch         \
 FMask FlangeDome FlatPipe FourPort GateValveCube GateValveCylinder GaugeTube \
 HPCombine HPJaws HalfElectronPipe IonGauge IonPTube Jaws LeadPipe LQuadF LQuadH   \
 LSexupole LegoBrick LocalShield M1detail MagTube MagnetBlock MagnetM1 \
 MagnetU1 MonoShutter MultiPipe NBeamStop Octupole OffsetFlangePipe PipeTube \
 PortTube PrismaChamber Quadrupole  \
 R3ChokeChamber RoundMonoShutter Scrapper Sexupole SixPort StriplineBPM \
 TWCavity TargetShield TriGroup TriPipe TriggerTube UndVac UndulatorVacuum \
 UTubePipe VacuumPipe ViewTube YAG YagScreen YagUnit default uVac  || exit

exit

## Need to fix the cooling pads on the reflector
./fullBuild -validAll --validCheck ${nValid} AA || exit
exit
