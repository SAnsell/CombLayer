#!/usr/bin/env bash

exec >/dev/null

nValid=1000
opts="--validAll --validCheck $nValid"
# parallel options
popts="-u --bar --halt now,fail=1"

parallel $popts ::: \
	 "./maxiv --noLengthCheck --defaultConfig Linac All $opts AA" \
	 "./ess --bunkerPillars ABunker $opts AA" \
	 "./ess --topModType Butterfly $opts AA" \
	 "./ess --topModType Pancake   $opts AA" \
	 "./maxiv -defaultConfig Single GunTestFacility -offset object Gun $opts AA" || exit

parallel $popts "./maxiv --defaultConfig Single {} $opts AA " ::: \
   SOFTIMAX BALDER COSAXS DANMAX FORMAX MICROMAX SPECIES MAXPEEM || exit

parallel $popts "./{} $opts AA" ::: t1Real reactor saxsSim || exit

parallel --bar $popts "./ess --defaultConfig Single {} $opts AA" ::: \
 VESPA ESTIA CSPEC  ODIN MAGIC BIFROST LOKI NMX  NNBAR  DREAM  BEER   \
 FREIA SKADI MIRACLES TESTBEAM TREX VOR    || exit
# HEIMDAL :: Not currently correct -- update underway

parallel $popts "./singleItem --singleItem {} $opts AA" ::: \
 BeamDivider BeamScrapper Bellow BlankTube BoxJaws         \
 BremBlock BremTube  ButtonBPM CRLTube  CeramicGap CleaningMagnet  \
 CollTube ConnectorTube CooledScreen CooledUnit CornerPipe \
 CorrectorMag CrossBlank   \
 CrossWay CylGateValve DiffPumpXIADP03 DipoleDIBMag DipoleExtract       \
 DipoleSndBend EArrivalMon EBeamStop EPSeparator ExperimentalHutch         \
 FMask FlangeDome FlatPipe FourPort GateValveCube GateValveCylinder GaugeTube \
 HPCombine HPJaws HalfElectronPipe IonGauge IonPTube Jaws LeadPipe LQuadF LQuadH   \
 LSexupole LocalShield M1detail MagTube MagnetBlock MagnetM1 \
 MagnetU1 MonoShutter MultiPipe NBeamStop Octupole OffsetFlangePipe PipeTube \
 PortTube PrismaChamber Quadrupole  \
 R3ChokeChamber RoundMonoShutter Scrapper Sexupole SixPort StriplineBPM \
 TWCavity TargetShield TriGroup TriPipe TriggerTube UndVac UndulatorVacuum \
 UTubePipe VacuumPipe ViewTube YAG YagScreen YagUnit default uVac || exit

exit

## Need to fix the cooling pads on the reflector
./fullBuild $opts AA || exit
exit
