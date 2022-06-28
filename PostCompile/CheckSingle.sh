#!/bin/bash

function checkValid
{
  if [ $? -ne 0 ] 
  then
     exit
  fi
  return
}

./singleItem --singleItem BeamDivider          --validCheck 1000 AA ; checkValid
./singleItem --singleItem BeamScrapper         --validCheck 1000 AA ; checkValid

./singleItem --singleItem Bellow               --validCheck 1000 AA ; checkValid;
./singleItem --singleItem BlankTube            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem BoxJaws              --validCheck 1000 AA ; checkValid;
./singleItem --singleItem BremBlock            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem BremTube             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem ButtonBPM            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem CRLTube              --validCheck 1000 AA ; checkValid;
./singleItem --singleItem CeramicGap           --validCheck 1000 AA ; checkValid;
./singleItem --singleItem CleaningMagnet       --validCheck 1000 AA ; checkValid;
./singleItem --singleItem CollTube             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem ConnectorTube        --validCheck 1000 AA ; checkValid;
./singleItem --singleItem CooledScreen         --validCheck 1000 AA ; checkValid;
./singleItem --singleItem CooledUnit           --validCheck 1000 AA ; checkValid;
./singleItem --singleItem CorrectorMag         --validCheck 1000 AA ; checkValid;
./singleItem --singleItem CrossBlank           --validCheck 1000 AA ; checkValid;
./singleItem --singleItem CrossWay             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem CylGateValve         --validCheck 1000 AA ; checkValid;
./singleItem --singleItem DiffPumpXIADP03      --validCheck 1000 AA ; checkValid;
./singleItem --singleItem DipoleDIBMag         --validCheck 1000 AA ; checkValid;
./singleItem --singleItem DipoleExtract        --validCheck 1000 AA ; checkValid;
./singleItem --singleItem DipoleSndBend        --validCheck 1000 AA ; checkValid;
./singleItem --singleItem EArrivalMon          --validCheck 1000 AA ; checkValid;
./singleItem --singleItem EBeamStop            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem EPCombine            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem EPContinue           --validCheck 1000 AA ; checkValid;
./singleItem --singleItem EPSeparator          --validCheck 1000 AA ; checkValid;
./singleItem --singleItem ExperimentalHutch    --validCheck 1000 AA ; checkValid;
./singleItem --singleItem FMask                --validCheck 1000 AA ; checkValid;
./singleItem --singleItem FlangeDome           --validCheck 1000 AA ; checkValid;
./singleItem --singleItem FlatPipe             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem FourPort             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem GateValveCube        --validCheck 1000 AA ; checkValid;
./singleItem --singleItem GateValveCylinder    --validCheck 1000 AA ; checkValid;
./singleItem --singleItem GaugeTube            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem HPCombine            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem HPJaws               --validCheck 1000 AA ; checkValid;
./singleItem --singleItem HalfElectronPipe     --validCheck 1000 AA ; checkValid;
./singleItem --singleItem Help                 --validCheck 1000 AA ; checkValid;
./singleItem --singleItem IonGauge             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem IonPTube             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem Jaws                 --validCheck 1000 AA ; checkValid;
./singleItem --singleItem LQuadF               --validCheck 1000 AA ; checkValid;
./singleItem --singleItem LQuadH               --validCheck 1000 AA ; checkValid;
./singleItem --singleItem LSexupole            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem LocalShield          --validCheck 1000 AA ; checkValid;
./singleItem --singleItem MagTube              --validCheck 1000 AA ; checkValid;
./singleItem --singleItem MagnetBlock          --validCheck 1000 AA ; checkValid;
./singleItem --singleItem MagnetM1             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem MagnetU1             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem MonoShutter          --validCheck 1000 AA ; checkValid;
./singleItem --singleItem MultiPipe            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem NBeamStop            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem Octupole             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem PipeTube             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem PortTube             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem PrismaChamber        --validCheck 1000 AA ; checkValid;
./singleItem --singleItem Quadrupole           --validCheck 1000 AA ; checkValid;
./singleItem --singleItem R3ChokeChamber       --validCheck 1000 AA ; checkValid;
./singleItem --singleItem RoundMonoShutter     --validCheck 1000 AA ; checkValid;
./singleItem --singleItem Scrapper             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem Sexupole             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem SixPort              --validCheck 1000 AA ; checkValid;
./singleItem --singleItem StriplineBPM         --validCheck 1000 AA ; checkValid;
./singleItem --singleItem TWCavity             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem TargetShield         --validCheck 1000 AA ; checkValid;
./singleItem --singleItem TriGroup             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem TriPipe              --validCheck 1000 AA ; checkValid;
./singleItem --singleItem TriggerTube          --validCheck 1000 AA ; checkValid;
./singleItem --singleItem UndVac               --validCheck 1000 AA ; checkValid;
./singleItem --singleItem UndulatorVacuum      --validCheck 1000 AA ; checkValid;
./singleItem --singleItem VacuumPipe           --validCheck 1000 AA ; checkValid;
./singleItem --singleItem ViewTube             --validCheck 1000 AA ; checkValid;
./singleItem --singleItem YAG                  --validCheck 1000 AA ; checkValid;
./singleItem --singleItem YagScreen            --validCheck 1000 AA ; checkValid;
./singleItem --singleItem YagUnit              --validCheck 1000 AA ; checkValid;
./singleItem --singleItem YagUnitBig           --validCheck 1000 AA ; checkValid;
./singleItem --singleItem default              --validCheck 1000 AA ; checkValid;
./singleItem --singleItem uVac                 --validCheck 1000 AA ; checkValid;       
