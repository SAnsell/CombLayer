#!/usr/bin/env bash

exec >/dev/null

nValid=1000
opts="--validAll --validCheck $nValid"
parallel="parallel -u --bar --halt now,fail=1"
inp="/tmp/AA"

# FLUKA estimators
# the uq is a perl replacement string to unquote the argument
$parallel ./maxiv -fluka infn -defaultConfig Linac -T '{=1 uq(); =}' $inp ::: \
 "myname resnuclei InjectionHall:Floor Concrete" \
 "myname resnuclei InjectionHall:Floor" \
 "myname surface electron InjectionHall back" \
 "myname surface 'e+&e-'  InjectionHall back 1e-11 3000 100 0 6.28318 3" \
 "myname surface electron InjectionHall back  -TMod energy myname 1e-11 3000 100" \
 "myname mesh dose-eq free 'Vec3D(-15.0,-400.0,-55.0)' 'Vec3D(15.0, 40.0,40.0)' 10 20 30 " \
 "myname mesh dose-eq free 'Vec3D(-15.0,-400.0,-55.0)' 'Vec3D(15.0, 40.0,40.0)' 10 20 30 -TMod doseType myname  all-part EWT74" \
 "help" "help resnuclei" "help surface"  || exit

# MCNP tallies
$parallel ./maxiv -defaultConfig Linac -T myname '{=1 uq(); =}' $inp ::: \
	 " surface e object     InjectionHall back" \
	 " surface e surfMap    InjectionHall InnerBack  1" \
	 " surface e surfMap    InjectionHall InnerBack -1" \
	 " surface e viewObject InjectionHall front BackWallFront \#BackWallBack '#SPFMazeIn'" \
	 " flux e InjectionHall:Floor Concrete" \
	 " flux e InjectionHall:Floor All" || exit

# GEOMETRY
$parallel {} $inp ::: "./maxiv --noLengthCheck --defaultConfig Linac All $opts" \
	  "./ess --bunkerPillars ABunker $opts" \
	  "./ess --topModType Butterfly $opts" \
	  "./ess --topModType Pancake   $opts" || exit

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
 FMask FlangeDome FlatPipe FourPort GateValveCube GateValveCylinder GaugeTube GTFGateValve \
 HPCombine HPJaws HalfElectronPipe IonGauge IonPTube Jaws LeadPipe LQuadF LQuadH   \
 LSexupole LocalShield M1detail MagTube MagnetBlock MagnetM1 \
 MagnetU1 MonoShutter MultiPipe NBeamStop Octupole OffsetFlangePipe PipeTube \
 PortTube PrismaChamber Quadrupole  \
 R3ChokeChamber RoundMonoShutter Scrapper Sexupole SlitsMask SixPort StriplineBPM \
 TDCBeamDump TWCavity TargetShield Torus TriGroup TriPipe TriggerTube UndVac UndulatorVacuum \
 UTubePipe VacuumPipe ViewTube YAG YagScreen YagUnit default uVac RFGun || exit

exit

## Need to fix the cooling pads on the reflector
./fullBuild $opts $inp || exit
exit
