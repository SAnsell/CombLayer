#!/usr/bin/env bash

function procExit
{
    if [ $? != 0 ]
    then
       exit
    fi
}

# --validCheck - use only one single point (default: origin (0,0,0))
# use --validPoint to specify the point
# --validAll - use all link points of all fixed components and the origin

# number of angles to check
NA=1000

./ess -validAll --validCheck $NA AA; procExit

# Don't forget to use procExit after the 'parallel' calls!

parallel --halt now,fail=1 "./{} -validAll --validCheck $NA AA" ::: \
	 fullBuild t1Real reactor
procExit

#segments=$(for i in {40..49}; do echo -n "Segment$i "; done)
segments=All
./maxiv --noLengthCheck --defaultConfig Linac ${segments} -validAll --validCheck $NA AA; procExit

# These beamlines currently fail: SPECIES MAXPEEM
parallel --halt now,fail=1 "./maxiv --defaultConfig Single {} --validAll --validCheck $NA AA" ::: \
	 BALDER COSAXS DANMAX FORMAX FLEXPES MICROMAX SOFTIMAX SPECIES MAXPEEM
procExit

# No need to use --validAll for singleItem since these models are very simple:

parallel --halt now,fail=1  "./singleItem --singleItem {} --validCheck $NA AA" ::: \
	 BeamDivider BeamScrapper Bellow BlankTube BoxJaws BremBlock BremTube ButtonBPM \
	 CRLTube CeramicGap CleaningMagnet CollTube ConnectorTube CooledScreen CooledUnit \
	 CorrectorMag CrossBlank CrossWay CylGateValve DiffPumpXIADP03 DipoleDIBMag \
	 DipoleExtract DipoleSndBend EArrivalMon EBeamStop EPSeparator ExperimentalHutch \
	 FMask FlangeDome FlatPipe FourPort GateValveCube GateValveCylinder GaugeTube \
	 HPCombine HPJaws HalfElectronPipe IonGauge IonPTube Jaws LQuadF LQuadH LSexupole \
	 LocalShield MagTube MagnetBlock MagnetM1 MagnetU1 MonoShutter MultiPipe NBeamStop \
	 Octupole PipeTube PortTube PrismaChamber Quadrupole R3ChokeChamber \
	 RoundMonoShutter Scrapper Sexupole SixPort StriplineBPM TWCavity TargetShield \
	 TriGroup TriPipe TriggerTube UndVac UndulatorVacuum VacuumPipe ViewTube YAG \
	 YagScreen YagUnit YagUnitBig default uVac
procExit
