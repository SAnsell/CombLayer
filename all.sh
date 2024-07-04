#!/usr/bin/env bash

exec >/dev/null

nValid=1000
opts="--validAll --validCheck $nValid"
parallel="parallel -u --bar --halt now,fail=1"
inp="/tmp/AA"

# FLUKA estimators
# the uq is a perl replacement string to unquote the argument
$parallel ./maxiv -fluka infn -defaultConfig Linac -T '{=1 uq(); =}' ::: \
 "myname resnuclei InjectionHall:Floor Concrete $inp" \
 "myname resnuclei InjectionHall:Floor $inp" \
 "myname surface electron InjectionHall back $inp" \
 "myname surface 'e+&e-'  InjectionHall back 1e-11 3000 100 0 6.28318 3 $inp" \
 "help $inp" "help resnuclei $inp" "help surface $inp"  || exit

# MCNP tallies
$parallel ./maxiv -defaultConfig Linac -T myname '{=1 uq(); =}' ::: \
	 " surface e object     InjectionHall back $inp" \
	 " surface e surfMap    InjectionHall InnerBack  1 $inp" \
	 " surface e surfMap    InjectionHall InnerBack -1 $inp" \
	 " surface e viewObject InjectionHall front BackWallFront \#BackWallBack '#SPFMazeIn' $inp" \
	 " flux e InjectionHall:Floor Concrete $inp" \
	 " flux e InjectionHall:Floor All $inp" || exit

# GEOMETRY
$parallel ::: "./maxiv --noLengthCheck --defaultConfig Linac All $opts $inp" \
	  "./ess --bunkerPillars ABunker $opts $inp" \
	  "./ess --topModType Butterfly $opts $inp" \
	  "./ess --topModType Pancake   $opts $inp" || exit

$parallel "./maxiv --defaultConfig Single {} $opts $inp " ::: \
   SOFTIMAX BALDER COSAXS DANMAX FORMAX MICROMAX SPECIES MAXPEEM || exit

$parallel "./{} $opts $inp" ::: t1Real reactor saxsSim || exit

$parallel "./ess --defaultConfig Single {} $opts $inp" ::: \
 VESPA ESTIA CSPEC  ODIN MAGIC BIFROST LOKI NMX  NNBAR  DREAM  BEER   \
 FREIA SKADI MIRACLES TESTBEAM TREX VOR    || exit
# HEIMDAL :: Not currently correct -- update underway

$parallel "./singleItem --singleItem {} $opts $inp" ::: \
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
./fullBuild $opts $inp || exit
exit
