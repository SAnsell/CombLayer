#!/usr/bin/env bash

exec >/dev/null

nValid=1000
opts="" #--validAll --validCheck $nValid"
parallel="parallel -u --bar --halt now,fail=1"

# FLUKA estimators
# the uq is a perl replacement string to unquote the argument
$parallel ./maxiv -fluka infn -defaultConfig Linac -T '{=1 uq(); =}' ::: \
 "myname resnuclei InjectionHall:Floor Concrete ihf1" \
 "myname resnuclei InjectionHall:Floor ihf2" \
 "myname surface electron InjectionHall back ihf3" \
 "myname surface 'e+&e-'  InjectionHall back 1e-11 3000 100 0 6.28318 3 ihf4" \
 "myname surface electron InjectionHall back  -TMod energy myname 1e-11 3000 100 ihf5" \
 "myname mesh dose-eq free 'Vec3D(-15.0,-400.0,-55.0)' 'Vec3D(15.0, 40.0,40.0)' 10 20 30 ihf6" \
 "myname mesh dose-eq free 'Vec3D(-15.0,-400.0,-55.0)' 'Vec3D(15.0, 40.0,40.0)' 10 20 30 -TMod doseType myname  all-part EWT74 ihf7" \
 "help ihf8" "help resnuclei ihf9" "help surface ihf10"  || exit

# MCNP tallies
$parallel ./maxiv -defaultConfig Linac -T myname '{=1 uq(); =}' ::: \
	 " surface e object     InjectionHall back ihm1" \
	 " surface e surfMap    InjectionHall InnerBack  1 ihm2" \
	 " surface e surfMap    InjectionHall InnerBack -1 ihm3" \
	 " surface e viewObject InjectionHall front BackWallFront \#BackWallBack '#SPFMazeIn' ihm4" \
	 " flux e InjectionHall:Floor Concrete ihm5" \
	 " flux e InjectionHall:Floor All ihm6" || exit

# GEOMETRY
$parallel {} ::: "./maxiv --noLengthCheck --defaultConfig Linac All $opts linac" \
	  "./ess --bunkerPillars ABunker $opts ess" \
	  "./ess --topModType Butterfly  $opts butterfly" \
	  "./ess --topModType Pancake    $opts pancake" || exit

$parallel "./maxiv --defaultConfig Single {} $opts {} " ::: \
	  DANMAX || exit
#	  SOFTIMAX BALDER COSAXS DANMAX FORMAX MICROMAX SPECIES MAXPEEM || exit

$parallel "./{} $opts {}" ::: t1Real reactor saxsSim || exit

$parallel "./ess --defaultConfig Single {} $opts {}" ::: \
 VESPA ESTIA CSPEC  ODIN MAGIC BIFROST LOKI NMX  NNBAR  DREAM  BEER   \
 FREIA SKADI MIRACLES TESTBEAM TREX VOR    || exit
# HEIMDAL :: Not currently correct -- update underway

$parallel "./singleItem --singleItem {} $opts {}" ::: \
 BeamDivider BeamScrapper Bellow BlankTube BoxJaws         \
 BremBlock BremTube  ButtonBPM CRLTube  CeramicGap CleaningMagnet  \
 CollTube ConnectorTube CooledScreen CooledUnit CornerPipe \
 CorrectorMag CrossBlank   \
 CrossWay CylGateValve DiffPumpXIADP03 DipoleDIBMag DipoleExtract       \
 DipoleSndBend EArrivalMon EBeamStop EPSeparator ExperimentalHutch        \
 FlangeDome FlatPipe FMask FourPort GateValveCube GateValveCylinder GaugeTube GTFGateValve \
 HPCombine HPJaws HalfElectronPipe IonGauge IonPTube Jaws LeadPipe LQuadF LQuadH   \
 LSexupole LocalShield M1detail MagTube MagnetBlock MagnetM1 \
 MagnetU1 MonoShutter MultiPipe NBeamStop Octupole OffsetFlangePipe PipeTube \
 PortTube PrismaChamber Quadrupole  \
 R3ChokeChamber RoundMonoShutter Scrapper Sexupole SlitsMask SixPort SqrShield StriplineBPM \
 TDCBeamDump TWCavity TargetShield Torus TriGroup TriPipe TriggerTube UndVac UndulatorVacuum \
 UTubePipe VacuumPipe ViewTube YAG YagScreen YagUnit default uVac RFGun HeatAbsorberToyama \
 HeatAbsorberR3Toyama MLMono || exit

# Test FLUKA USRBIN estimators in the MAX IV DanMAX model
./maxiv --defaultConfig Single DANMAX -angle objAxis DanMAXFrontBeam 0  -offset object DanMAXFrontBeamUndulator 0 -fluka -T fineD mesh dose-eq object DanMAXFrontBeamPrePipe \#back 'Vec3D(-10.5,-10.0,-10.5)' 'Vec3D(10.5,5650.0,10.5)' 21 11320 1 -TMod doseType fineD all-part EWT74  danmax || exit
./maxiv --defaultConfig Single DANMAX -angle objAxis DanMAXFrontBeam 0  -offset object DanMAXFrontBeamUndulator 0 -fluka -T fineD mesh dose-eq object DanMAXFrontBeamPrePipe \#back 'Vec3D(-10.5,-10.0,-10.5)' 'Vec3D(10.5,5650.0,10.5)' 21 11320 1 -TMod doseType fineD all-part EWT74 -T fineS mesh dose-eq object DanMAXOpticsLineValveS2 back 'Vec3D(-10.5,-50.0,-10.5)' 'Vec3D(10.5,2550.0,10.5)' 21 5200 1 -TMod doseType fineS all-part EWT74 danmax || exit

# Object addition
./maxiv --defaultConfig Single DANMAX -angle objAxis DanMAXFrontBeam 0  -offset object DanMAXFrontBeamUndulator 0 -fluka -OAdd sphere object DanMAXFrontBeamUndulator 0 'Vec3D(1,4350,0)' 5 Copper   -v DanMAXExptHut1VoidMat Air oadd_sphere || exit

./maxiv --defaultConfig Single DANMAX -angle objAxis DanMAXFrontBeam 0  -offset object DanMAXFrontBeamUndulator 0 -fluka -OAdd cylinder object DanMAXFrontBeamUndulator 0 'Vec3D(1,4350,0)' 0.5 0.5 Copper oadd_cylinder || exit

./maxiv --defaultConfig Single DANMAX -angle objAxis DanMAXFrontBeam 0  -offset object DanMAXFrontBeamUndulator 0 -fluka -OAdd plate object DanMAXOpticsHut front 'Vec3D(-260,50,80)' 5 10 60 Void oadd_plate || exit

## Need to fix the cooling pads on the reflector
#./fullBuild $opts $inp || exit
