/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBuild/bifrost/BIFROST.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <array>




#include "FileReport.h"
#include "OutputLog.h"






#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "Object.h"


#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "AttachSupport.h"
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "Aperture.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "Bunker.h"
#include "CompBInsert.h"
//#include "SecondTrack.h"

#include "LayerComp.h"
#include "ContainedComp.h"
//#include "ContainedSpace.h"
#include "ContainedGroup.h"

#include "BaseMap.h"
#include "CellMap.h"

#include "CryoMagnetBase.h"


#include "caveDetector.h"
#include "radialCollimator.h"
#include "brickShape.h"
#include "Beamstop.h"
#include "Corner.h"
#include "VacuumPipe.h"
#include "ConicPipe.h"

#include "cryoSample.h"
#include "simpleCave.h"
#include "CaveMicoBlock.h"
#include "CaveMicoRoof.h"

#include "SimpleBoxChopper.h"
#include "ChopperPit.h"
#include "LineShieldPlus.h"
#include "InnerShield.h"
#include "HoleShape.h"
#include "BifrostHut.h"

#include "BoxShutter.h"
#include "D03SupportBlock.h"
#include "D03BaseBlock.h"
#include "D03UpperBlock.h"
#include "D03SteelInsert.h"
#include "D03SteelSpacing.h"
#include "E02BaseBlock.h"
#include "E02PillarCover.h"


#include "BIFROST.h"


namespace essSystem
{

BIFROST::BIFROST(const std::string& keyName) :
  attachSystem::CopiedComp("bifrost",keyName),
  nGuideSection(5),nSndSection(10),nEllSection(6),stopPoint(0),
  //  nShutterPlate(5),nShutterB4CPlate(5),
  bifrostAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  FocusA(new beamlineSystem::GuideLine(newName+"FA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  AppA(new constructSystem::Aperture(newName+"AppA")),

  ChopperA(new constructSystem::SimpleBoxChopper(newName+"ChopperA")),
  DDisk(new constructSystem::DiskChopper(newName+"DBlade")),

  VPipeBA(new constructSystem::VacuumPipe(newName+"PipeBA")),
  BenderA(new beamlineSystem::GuideLine(newName+"BA")),
  
  ChopperB(new constructSystem::SimpleBoxChopper(newName+"ChopperB")),
  FOCDiskB(new constructSystem::DiskChopper(newName+"FOC1Blade")),

  VPipeBB(new constructSystem::VacuumPipe(newName+"PipeBB")),
  //  FocusD(new beamlineSystem::GuideLine(newName+"FD")),
  BenderB(new beamlineSystem::GuideLine(newName+"BB")),

  VPipeBC(new constructSystem::VacuumPipe(newName+"PipeBC")),
  //  FocusE(new beamlineSystem::GuideLine(newName+"FE")),
  BenderC(new beamlineSystem::GuideLine(newName+"BC")),

  ChopperC(new constructSystem::SimpleBoxChopper(newName+"ChopperC")),
  FOCDiskC(new constructSystem::DiskChopper(newName+"FOC2Blade")),

  VPipeBD(new constructSystem::VacuumPipe(newName+"PipeBD")),
  // FocusF(new beamlineSystem::GuideLine(newName+"FF")),
  BenderD(new beamlineSystem::GuideLine(newName+"BD")),

  
  VPipeBE(new constructSystem::VacuumPipe(newName+"PipeBE")),
  // FocusF(new beamlineSystem::GuideLine(newName+"FF")),
  BenderE(new beamlineSystem::GuideLine(newName+"BE")),

  AppB(new constructSystem::Aperture(newName+"AppB")),

  //  BInsert(new BunkerInsert(newName+"BInsert")),
  BInsert(new CompBInsert(newName+"CInsert")),
  VPipeWall(new constructSystem::VacuumPipe(newName+"PipeWall")),
  TPipeWall(new beamlineSystem::GuideLine(newName+"TPipeWall")),
  TPipeWall1(new beamlineSystem::GuideLine(newName+"TPipeWall1")),
  TPipeWall2(new beamlineSystem::GuideLine(newName+"TPipeWall2")),
  TPipeWall3(new beamlineSystem::GuideLine(newName+"TPipeWall3")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),
  FocusWall1(new beamlineSystem::GuideLine(newName+"FWall1")),
  FocusWall2(new beamlineSystem::GuideLine(newName+"FWall2")),


  //  WallCut(new constructSystem::Aperture(newName+"WallCut")),
  WallCut(new beamlineSystem::GuideLine(newName+"WallCut")),  



  D03FirstSupport(new constructSystem::D03SupportBlock(newName+"D03FirstSupport")),
  D03FirstBase(new constructSystem::D03BaseBlock(newName+"D03FirstBase")),
  D03FirstUpper(new constructSystem::D03UpperBlock(newName+"D03FirstUpperBlock")),
  D03SecondUpper(new constructSystem::D03UpperBlock(newName+"D03SecondUpperBlock")),

  D03SteelVoid(new constructSystem::D03SteelSpacing(newName+"D03SteelVoid")),
  D03LeadFirstA(new constructSystem::D03SteelInsert(newName+"D03LeadFirstA")),
  D03SteelFirstA(new constructSystem::D03SteelInsert(newName+"D03SteelFirstA")),
  D03B4CFirstA(new constructSystem::D03SteelInsert(newName+"D03B4CFirstA")),
  D03SteelFirstB(new constructSystem::D03SteelInsert(newName+"D03SteelFirstB")),
  D03SteelFirstC(new constructSystem::D03SteelInsert(newName+"D03SteelFirstC")),
  // D03SteelShutter(new constructSystem::D03SteelInsert(newName+"D03SteelFirstA")),
  // D03SteelFirstA(new constructSystem::D03SteelInsert(newName+"D03SteelFirstA")),


  
  //  ShieldGap(new constructSystem::LineShieldPlus(newName+"ShieldGap")),
  //  ShieldGap1(new constructSystem::LineShieldPlus(newName+"ShieldGap1")),
    ShieldGap1(new constructSystem::InnerShield(newName+"ShieldGap1")),
  ShieldGap2(new constructSystem::LineShieldPlus(newName+"ShieldGap2")),
  InnerShieldGap1(new constructSystem::InnerShield(newName+"InnerShieldGap1")),
  InnerShieldGap2(new constructSystem::InnerShield(newName+"InnerShieldGap2")),

  ShieldA(new constructSystem::LineShieldPlus(newName+"ShieldA")),
  InnerShieldA(new constructSystem::InnerShield(newName+"InnerShieldA")),
  InnerShieldB(new constructSystem::InnerShield(newName+"InnerShieldB")),
  ShutterFrontWall(new constructSystem::LineShieldPlus(newName+"ShutterFrontWall")),
  ShutterRearWall(new constructSystem::LineShieldPlus(newName+"ShutterRearWall")),
  ShutterExpansion(new constructSystem::LineShieldPlus(newName+"ShutterExpansion")),
  ShutterInnerShield(new constructSystem::InnerShield(newName+"ShutterInnerShield")),
  ShutterFrontHorseshoe
  (new constructSystem::InnerShield(newName+"ShutterFrontHorseshoe")),
  ShutterRearHorseshoe
  (new constructSystem::InnerShield(newName+"ShutterRearHorseshoe")),
  ShutterFrontB4C(new constructSystem::InnerShield(newName+"ShutterFrontB4C")),
  ShutterRearB4C(new constructSystem::InnerShield(newName+"ShutterRearB4C")),
 
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"FOutA")),

  //  ShutterA(new shutterSystem::GeneralShutter(1,newName+"ShutterA")),
  ShutterAFrame(new constructSystem::Aperture(newName+"ShutterAFrame")),
  ShutterARails(new constructSystem::Aperture(newName+"ShutterARails")),
  ShutterAB4C(new essSystem::BoxShutter(newName+"ShutterAB4C")),
  ShutterALead(new essSystem::BoxShutter(newName+"ShutterALead")),

  ShutterAPedestal(new constructSystem::D03SteelInsert(newName+"ShutterAPedestal")),
  
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::GuideLine(newName+"FOutB")),

  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  FocusOutC(new beamlineSystem::GuideLine(newName+"FOutC")),
  /*
  VPipeOutD(new constructSystem::VacuumPipe(newName+"PipeOutD")),
  FocusOutD(new beamlineSystem::GuideLine(newName+"FOutD")),
  */
  OutPitA(new constructSystem::ChopperPit(newName+"OutPitA")),
  OutACutFront(new constructSystem::HoleShape(newName+"OutACutFront")),
  OutACutBack(new constructSystem::HoleShape(newName+"OutACutBack")),

  ChopperOutA(new constructSystem::SimpleBoxChopper(newName+"ChopperOutA")),
  FOCDiskOutA(new constructSystem::DiskChopper(newName+"FOCOutABlade")),

  ShieldB(new constructSystem::LineShieldPlus(newName+"ShieldB")),

  Cave(new BifrostHut(newName+"Cave")),
  CaveCut(new constructSystem::HoleShape(newName+"CaveCut")),

  //  VPipeCave(new constructSystem::VacuumPipe(newName+"PipeCave")),
  // FocusCave(new beamlineSystem::GuideLine(newName+"FCave")),
  //Sample environment
  //  SampleMagnetBase(new cryoSystem::CryoMagnetBase("SampleMagnetBase")),
  //  ASample(new cryoSystem::cryoSample("ASample"))
  /*!
    Constructor
 */
  /**** Cave variables ****/
  ACave(new simpleCaveSystem::simpleCave("BIFROSTCave")),
  CaveLining(new simpleCaveSystem::simpleCave("BIFROSTCaveLining")),
  //  InletCut(new beamlineSystem::GuideLine("BIFROSTCaveInletCut")),
    InletCut(new constructSystem::InnerShield("BIFROSTCaveInletCut")),
    InletCut2(new constructSystem::InnerShield("BIFROSTCaveInletCut2")),
  BeamstopCut(new beamlineSystem::GuideLine("BIFROSTCaveBeamstopCut")),
  //  CaveCut(new constructSystem::HoleShape("CaveCut")),
  AMagnetBase(new cryoSystem::CryoMagnetBase("BMagnetBase")),
  ASample(new cryoSystem::cryoSample("BSample")),
  //  VBeamstopCone(new constructSystem::VacuumPipe("BIFROSTCaveConicPipe")),
  VBeamstopPipe(new constructSystem::VacuumPipe("BIFROSTCaveBeamstopPipe")),
  VGetLostPipe(new constructSystem::ConicPipe("BIFROSTCaveGetLostPipe")),
    VPipeCave(new constructSystem::VacuumPipe("PipeCave")),
  //  VPipeCave2(new constructSystem::VacuumPipe("PipeCave2")),
  FocusCave1(new beamlineSystem::GuideLine(newName+"FCave1")),
  FocusCave2(new beamlineSystem::GuideLine(newName+"FCave2")),
  Detector(new simpleCaveSystem::caveDetector("Detector")),
  Collimator(new simpleCaveSystem::radialCollimator("Collimator")),
  BrickShapeIn(new simpleCaveSystem::brickShape("BrickShapeIn")),
  BrickShapeOut(new simpleCaveSystem::brickShape("BrickShapeOut")),
  SampleLabyrinth(new constructSystem::Corner("BIFROSTCaveSampleLabyrinth")),
  SampleLabyrinthB4C(new constructSystem::Corner("BIFROSTCaveSampleLabyrinthB4C")),
  SampleCorridor(new constructSystem::InnerShield("BIFROSTCaveSampleCorridor")),
  DetectorLabyrinth(new constructSystem::Beamstop("BIFROSTCaveDetectorLabyrinth")),
  DetectorLabyrinthB4C(new constructSystem::Corner("BIFROSTCaveDetectorLabyrinthB4C")),
  DetectorLabyrinthB4Cexit(new constructSystem::Corner("BIFROSTCaveDetectorLabyrinthB4Cexit")),
  UpperBlockWall(new constructSystem::InnerShield("BIFROSTCaveUpperBlockWall")),
  GallerySteel(new constructSystem::InnerShield("BIFROSTCaveGallerySteel")),
  GalleryDoor(new constructSystem::InnerShield("BIFROSTCaveGalleryDoor")),
  UpperBlockRoof1(new constructSystem::InnerShield("BIFROSTCaveUpperBlockRoof1")),
  UpperBlockRoof2(new constructSystem::InnerShield("BIFROSTCaveUpperBlockRoof2")),
  UpperBlockHatch(new constructSystem::InnerShield("BIFROSTCaveUpperBlockHatch")),
  //  UpperBlockHatchB4C(new constructSystem::InnerShield("BIFROSTCaveUpperBlockHatchB4C")),
  UpperBlockHatch2(new constructSystem::InnerShield("BIFROSTCaveUpperBlockHatch2")),
  RoofCableB4C1(new constructSystem::Corner("BIFROSTCaveRoofCableB4C1")),
  RoofCableB4C2(new constructSystem::Corner("BIFROSTCaveRoofCableB4C2")),
  UpperCableRouteR(new constructSystem::Corner("BIFROSTCaveUpperCableRouteR")),
  UpperCableRouteL(new constructSystem::Corner("BIFROSTCaveUpperCableRouteL")),
  BeamstopDoughnut2(new constructSystem::InnerShield("BIFROSTCaveBeamstopDoughnut2")),
  BeamstopDoughnut(new constructSystem::InnerShield("BIFROSTCaveBeamstopDoughnut")),
  //  BeamstopTshape(new beamlineSystem::GuideLine("BIFROSTCaveBeamstopTshape")),
  BeamstopTshape(new essSystem::BoxShutter("BIFROSTCaveBeamstopTshape")),
  BeamstopPlug(new essSystem::BoxShutter("BIFROSTCaveBeamstopPlug")),
  BeamstopInnerDoughnut
       (new beamlineSystem::GuideLine("BIFROSTCaveBeamstopInnerDoughnut")),
  Hatch(new constructSystem::InnerShield("BIFROSTCaveHatch")),
  Hatch2(new constructSystem::InnerShield("BIFROSTHatch2")),
  Hatch3(new constructSystem::InnerShield("BIFROSTCaveHatch3")),
  HatchRails(new constructSystem::InnerShield("BIFROSTCaveHatchRails")),
  HatchLock(new essSystem::BoxShutter("BIFROSTCaveHatchLock")),
  HatchLock1(new essSystem::BoxShutter("BIFROSTCaveHatchLock1")),
  HatchLock2(new essSystem::BoxShutter("BIFROSTCaveHatchLock2")),
  HatchStreamBlocker(new essSystem::BoxShutter("BIFROSTCaveHatchStreamBlocker")),
  LabyrinthPlate(new essSystem::BoxShutter("BIFROSTCaveLabyrinthPlate")),
  CavePlatform(new constructSystem::InnerShield("BIFROSTCavePlatform")),
  CavePlatformFence(new constructSystem::InnerShield("BIFROSTCavePlatformFence")),
  CavePlatformB4C(new constructSystem::InnerShield("BIFROSTCavePlatformB4C")),
  CableLow1(new constructSystem::InnerShield("BIFROSTCaveCableLow1")),
  CableLowBlock1(new constructSystem::Corner("BIFROSTCaveCableLowBlock1")),
  CableLowBlock11(new constructSystem::Corner("BIFROSTCaveCableLowBlock11")),
  CableLow2(new constructSystem::InnerShield("BIFROSTCaveCableLow2")),
  CableLowBlock2(new constructSystem::Corner("BIFROSTCaveCableLowBlock2")),
  CableLowBlock21(new constructSystem::Corner("BIFROSTCaveCableLowBlock21")),
  CableLow3(new constructSystem::InnerShield("BIFROSTCaveCableLow3")),
  CableLowBlock3(new constructSystem::InnerShield("BIFROSTCaveCableLowBlock3")),
  DetectorAccess(new constructSystem::InnerShield("BIFROSTCaveDetAccess")),
  DetectorAccessOuter(new beamlineSystem::GuideLine("BIFROSTCaveDetAccessOuter")),
  Slits(new constructSystem::Aperture(newName+"Slits")),
  B4CChopper(new constructSystem::Aperture(newName+"B4CChopper")),
  BeamMonitorChopper(new constructSystem::Aperture(newName+"BeamMonitorChopper")),
  SampleSheet(new essSystem::BoxShutter("BIFROSTCaveSampleSheet")),
  Beamstop(new essSystem::BoxShutter("BIFROSTCaveBeamstop"))

  //  ShieldGap1(new constructSystem::LineShieldPlus("BIFROSTCaveShieldGap1")),
  // ShieldGap2(new constructSystem::LineShieldPlus("BIFROSTCaveShieldGap2"))


{
  ELog::RegMethod RegA("BIFROST","BIFROST");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for(size_t i=0;i<nGuideSection;i++)
    {
      const std::string strNum(std::to_string(i));
      RecPipe[i]=std::shared_ptr<constructSystem::VacuumPipe>
        (new constructSystem::VacuumPipe(newName+"PipeR"+strNum));
      RecFocus[i]=std::shared_ptr<beamlineSystem::GuideLine>
        (new beamlineSystem::GuideLine(newName+"FOutR"+strNum));
      OR.addObject(RecPipe[i]);
      OR.addObject(RecFocus[i]);
    }

  for(size_t i=0;i<nSndSection;i++)
    {
      const std::string strNum(std::to_string(i));
      SndPipe[i]=std::shared_ptr<constructSystem::VacuumPipe>
        (new constructSystem::VacuumPipe(newName+"PipeS"+strNum));
      SndFocus[i]=std::shared_ptr<beamlineSystem::GuideLine>
        (new beamlineSystem::GuideLine(newName+"FOutS"+strNum));
      OR.addObject(SndPipe[i]);
      OR.addObject(SndFocus[i]);
    }

  for(size_t i=0;i<nEllSection;i++)
    {
      const std::string strNum(std::to_string(i));
      EllPipe[i]=std::shared_ptr<constructSystem::VacuumPipe>
        (new constructSystem::VacuumPipe(newName+"PipeE"+strNum));
      EllFocus[i]=std::shared_ptr<beamlineSystem::GuideLine>
        (new beamlineSystem::GuideLine(newName+"FOutE"+strNum));
      OR.addObject(EllPipe[i]);
      OR.addObject(EllFocus[i]);
    }
  /*
  for (size_t t=0; i<nShutterPlate;i++){
      const std::string strNum(std::to_string(i));
      ShutterALead[i]=std::shared_ptr<constructSystem::Aperture>
        (new constructSystem::Aperture(newName+"ShutterALead"+strNum));
      OR.addObject(ShutterALead[i]);
  }
  for (size_t t=0; i<nShutterB4CPlate;i++){
      const std::string strNum(std::to_string(i));
      ShutterALead[i]=std::shared_ptr<constructSystem::Aperture>
        (new constructSystem::Aperture(newName+"ShutterALead"+strNum));
      OR.addObject(ShutterALead[i]);
      }*/
  
  // This is necessary as not directly constructed:
  //  OR.cell(newName+"Axis");
  OR.addObject(bifrostAxis);

  OR.addObject(FocusA);
  OR.addObject(VPipeB);
  OR.addObject(FocusB);

  OR.addObject(VPipeC);
     OR.addObject(FocusC);
  OR.addObject(AppA);

  OR.addObject(ChopperA);
  OR.addObject(DDisk);  
  
  OR.addObject(VPipeBA);
  OR.addObject(BenderA);
  
  OR.addObject(ChopperB);
  OR.addObject(FOCDiskB);  

  OR.addObject(VPipeBB);
  //  OR.addObject(FocusD);
  OR.addObject(BenderB);

  OR.addObject(VPipeBC);
  //  OR.addObject(FocusE);
    OR.addObject(BenderC);

  OR.addObject(ChopperC);
  OR.addObject(FOCDiskC);  
  
  OR.addObject(VPipeBD);
  //  OR.addObject(FocusF);
  OR.addObject(BenderD);

  OR.addObject(VPipeBE);
  //  OR.addObject(FocusF);
  OR.addObject(BenderE);

  OR.addObject(AppB);

  OR.addObject(BInsert);
  OR.addObject(VPipeWall);
  OR.addObject(TPipeWall);
  OR.addObject(TPipeWall1);
  OR.addObject(TPipeWall2);
  OR.addObject(TPipeWall3);
  OR.addObject(FocusWall);  
  OR.addObject(FocusWall1);  
  OR.addObject(FocusWall2);  


  OR.addObject(WallCut);
  OR.addObject(D03FirstSupport);
  OR.addObject(D03FirstBase);
  OR.addObject(D03FirstUpper);
  OR.addObject(D03SecondUpper);
  OR.addObject(D03SteelVoid);
  OR.addObject(D03LeadFirstA);
  OR.addObject(D03SteelFirstA);
  OR.addObject(D03B4CFirstA);
  OR.addObject(D03SteelFirstB);
  OR.addObject(D03SteelFirstC);
  
  //  OR.addObject(ShieldGap);
  OR.addObject(ShieldGap1);
  OR.addObject(InnerShieldGap1);
  OR.addObject(ShieldGap2);
  OR.addObject(InnerShieldGap2);

  OR.addObject(ShieldA);
  OR.addObject(InnerShieldA);
  OR.addObject(InnerShieldB);

  OR.addObject(ShutterFrontWall);
  OR.addObject(ShutterRearWall);
  OR.addObject(ShutterExpansion);
  OR.addObject(ShutterInnerShield);
  OR.addObject(ShutterFrontHorseshoe);
  OR.addObject(ShutterRearHorseshoe);
  OR.addObject(ShutterFrontB4C);
  OR.addObject(ShutterRearB4C);

  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);
 
  OR.addObject(ShutterAFrame);
  OR.addObject(ShutterARails);
  OR.addObject(ShutterAB4C);
  OR.addObject(ShutterALead);
  OR.addObject(ShutterAPedestal);
  
  OR.addObject(VPipeOutB);
  OR.addObject(FocusOutB);
  
  OR.addObject(VPipeOutC);
  OR.addObject(FocusOutC);
  /*
  OR.addObject(VPipeOutD);
  OR.addObject(FocusOutD);
  */
  OR.addObject(OutPitA);
  OR.addObject(OutACutFront);
  OR.addObject(OutACutBack);

  OR.addObject(ChopperOutA);
  OR.addObject(FOCDiskOutA);  

  OR.addObject(Cave);
  OR.addObject(CaveCut);

  //  OR.addObject(VPipeCave);
  //  OR.addObject(FocusCave);
  //Sample environment
  //  OR.addObject(SampleMagnetBase);
  //  OR.addObject(ASample);

  /**** Cave variables ******/
  OR.addObject(ACave);
  OR.addObject(CaveLining);
  OR.addObject(AMagnetBase);
  OR.addObject(ASample);
  OR.addObject(VPipeCave);
  OR.addObject(Slits);
  OR.addObject(B4CChopper);
  OR.addObject(BeamMonitorChopper);
  //  OR.addObject(VPipeCave2);
  OR.addObject(VBeamstopPipe);
  OR.addObject(VGetLostPipe);
  OR.addObject(FocusCave1);
  OR.addObject(FocusCave2);
  OR.addObject(Detector);
  OR.addObject(Collimator);
  OR.addObject(BrickShapeIn);
  OR.addObject(BrickShapeOut);
  OR.addObject(SampleLabyrinth);
  OR.addObject(SampleLabyrinthB4C);
  OR.addObject(SampleCorridor);
  OR.addObject(DetectorLabyrinth);
  OR.addObject(DetectorLabyrinthB4C);
  OR.addObject(DetectorLabyrinthB4Cexit);
  OR.addObject(UpperBlockWall);
  OR.addObject(GallerySteel);
  OR.addObject(GalleryDoor);
  OR.addObject(UpperBlockRoof1);
  OR.addObject(UpperBlockRoof2);
  OR.addObject(UpperBlockHatch);
  OR.addObject(UpperBlockHatch2);
  OR.addObject(RoofCableB4C1);
  OR.addObject(RoofCableB4C2);
  OR.addObject(UpperCableRouteR);
  OR.addObject(UpperCableRouteL);
  OR.addObject(BeamstopDoughnut2); 
  OR.addObject(BeamstopDoughnut);
  OR.addObject(BeamstopInnerDoughnut);
  OR.addObject(Hatch);
  OR.addObject(Hatch2);
  OR.addObject(Hatch3);
  OR.addObject(HatchRails);
  OR.addObject(HatchLock);
  OR.addObject(HatchLock1);
  OR.addObject(HatchLock2);
  OR.addObject(HatchStreamBlocker);

  OR.addObject(LabyrinthPlate);
  OR.addObject(InletCut);
  OR.addObject(InletCut2);
  OR.addObject(BeamstopCut);
  OR.addObject(Beamstop);
  OR.addObject(BeamstopTshape);
  OR.addObject(BeamstopPlug);
  OR.addObject(CavePlatform);
  OR.addObject(CavePlatformFence);
  OR.addObject(CavePlatformB4C);
  OR.addObject(CableLow1);
  OR.addObject(CableLow2);
  OR.addObject(CableLow3);
  OR.addObject(CableLowBlock1);
  OR.addObject(CableLowBlock2);
  OR.addObject(CableLowBlock3);
  OR.addObject(CableLowBlock11);
  OR.addObject(CableLowBlock21);
  OR.addObject(DetectorAccess);
  OR.addObject(DetectorAccessOuter);


}

BIFROST::~BIFROST()
  /*!
    Destructor
  */
{}
  
void 
BIFROST::build(Simulation& System,
               const GuideItem& GItem,
               const Bunker& bunkerObj,
               const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param bunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // For output stream
  ELog::RegMethod RegA("BIFROST","build");

  ELog::EM<<"\nBuilding BIFROST on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  
  essBeamSystem::setBeamAxis(*bifrostAxis,Control,GItem,1);
  ELog::EM<<"Beam axis == "<<bifrostAxis->getLinkPt(3)<<ELog::endDiag;

  /*  
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*bifrostAxis,-3,*bifrostAxis,-3); 
  */
  // FocusA(new beamlineSystem::GuideLine(newName+"FA")),


    std::array<std::shared_ptr<beamlineSystem::GuideLine>,20> FASplit;
    for (int i=0;i<20;i++){
      int insVoid1 = GItem.getCells("Void")[0];
      int insVoid2 = GItem.getCells("Void")[1];
      int insVoid3 = GItem.getCells("Void")[2];
      
      FASplit[i]=std::shared_ptr<beamlineSystem::GuideLine>
	(new beamlineSystem::GuideLine("bifrostFSplit"+std::to_string(i)));
    OR.addObject ("bifrostFSplit"+std::to_string(i),FASplit[i]);
        
    if (i==0) { FASplit[i]->setFront(GItem.getKey("Beam"),-1);
      FASplit[i]->addInsertCell(insVoid1);
      FASplit[i]->createAll(System,*bifrostAxis,-3,*bifrostAxis,-3);
      insVoid1=System.splitObject(insVoid1, FASplit[i]->getKey("Guide0").getLinkSurf(-2)); }
    else if(i<6) {      FASplit[i]->addInsertCell(insVoid1);
     FASplit[i]->createAll(System,FASplit[i-1]->getKey("Guide0"),2,FASplit[i-1]->getKey("Guide0"),2);
     insVoid1=System.splitObject(insVoid1, FASplit[i]->getKey("Guide0").getLinkSurf(-2));
    }
    else if(i==6) {      FASplit[i]->addInsertCell(insVoid1);
                         FASplit[i]->addInsertCell(insVoid2);
     FASplit[i]->createAll(System,FASplit[i-1]->getKey("Guide0"),2,FASplit[i-1]->getKey("Guide0"),2);
     insVoid2=System.splitObject(insVoid2, FASplit[i]->getKey("Guide0").getLinkSurf(-2));
    }
    else if(i<10) {      FASplit[i]->addInsertCell(insVoid2);
     FASplit[i]->createAll(System,FASplit[i-1]->getKey("Guide0"),2,FASplit[i-1]->getKey("Guide0"),2);
     insVoid2=System.splitObject(insVoid2, FASplit[i]->getKey("Guide0").getLinkSurf(-2));
    }
    else if (i==10)  { FASplit[i]->addInsertCell(insVoid1);
       FASplit[i]->addInsertCell(insVoid2);
       FASplit[i]->addInsertCell(insVoid3);
      FASplit[i]->createAll(System,FASplit[i-1]->getKey("Guide0"),2,FASplit[i-1]->getKey("Guide0"),2);
      insVoid3 = System.splitObject(insVoid3, FASplit[i]->getKey("Guide0").getLinkSurf(-2));
    }
    else if (i<19)  { FASplit[i]->addInsertCell(insVoid1);
       FASplit[i]->addInsertCell(insVoid3);
      FASplit[i]->createAll(System,FASplit[i-1]->getKey("Guide0"),2,FASplit[i-1]->getKey("Guide0"),2);
      insVoid3 = System.splitObject(insVoid3, FASplit[i]->getKey("Guide0").getLinkSurf(-2));
    }
    else if (i==19) {FASplit[i]->setBack(GItem.getKey("Beam"),-2);
      FASplit[i]->addInsertCell(insVoid3);
      FASplit[i]->createAll(System,FASplit[i-1]->getKey("Guide0"),2,FASplit[i-1]->getKey("Guide0"),2);
    }
}


  
  
  if (stopPoint==1) return;                      // STOP At monolith

  
  int lastBunkerVoidPiece =  buildBunkerUnits(System,FASplit[19]->getKey("Guide0"),2,
                   bunkerObj.getCell("MainVoid"));
  
  /*
  int lastBunkerVoidPiece =  buildBunkerUnits(System,FocusA->getKey("Guide0"),2,
                   bunkerObj.getCell("MainVoid"));
  */
  
  if (stopPoint==2) return;                      // STOP At bunker edge
  // IN WALL
  // Make bunker insert
  // BInsert->addInsertCell(bunkerObj.getCell("MainVoid"));


 // TPipeWall->setFront(-sIndex[11]);

 
  /*

  TPipeWall1->setFront(TPipeWall->getKey("Guide0"),2);
  TPipeWall1->setBack(bunkerObj,-2);
 
  TPipeWall1->createAll(System,TPipeWall->getKey("Guide0"),2,TPipeWall->getKey("Guide0"),2);
  */
  //  WallCut->setFront(*BInsert,2);
  
 
  /*  // Example of accessing sectors in the bunker wall doesn't work: getRegister removed from ObjectRegister definition
 ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

 attachSystem::FixedComp* CPtr=OR.getObject<attachSystem::FixedComp>("ABunkerWall");
  const BunkerWall* WllPtr= dynamic_cast<const BunkerWall*>(CPtr);
 if (!WllPtr)
   throw ColErr::InContainerError<std::string>
     (WllPtr->getKeyName(),"Component not found");
     
 std::vector<int> sIndex=WllPtr->getCellsHR(System,"Sector4").getSurfaceNumbers();
 std::vector<int> dIndex=WllPtr->getCellsHR(System,"Sector3").getSurfaceNumbers();

  */



  //   ShieldGap1->addInsertCell(voidCell);

   
      ShieldGap1->setBack(bunkerObj,-2);

      // WallCut->setBack(bunkerObj,-2);
   ShieldGap1->createAll(System, BenderE->getKey("Guide0"),2);
    attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*ShieldGap1);
    attachSystem::addToInsertSurfCtrl(System,bunkerObj,"floor",*ShieldGap1);
    /*
   std::shared_ptr<essSystem::CompBInsert> InsertPhantom=
     std::shared_ptr<essSystem::CompBInsert>
     (new essSystem::CompBInsert("bifrostCPhantomInsert"));
    OR.addObject ("bifrostCPhantomInsert",InsertPhantom);
    InsertPhantom->setFront(bunkerObj,-1);
    InsertPhantom->setBack(*ShieldGap1,1);
    InsertPhantom->createAll(System,BenderE->getKey("Guide0"),2,bunkerObj);
  //  BInsert->createAll(System,*VPipeBE,2,bunkerObj);
    attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*InsertPhantom);
    */
 
   //   BInsert->addInsertCell(lastBunkerVoidPiece);
   BInsert->setFront(bunkerObj,-1);
    BInsert->setBack(*ShieldGap1,1);
 
   //  BInsert->setFront(BenderE->getKey("Guide0"),2);
    BInsert->createAll(System,BenderE->getKey("Guide0"),2,bunkerObj);
        attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);
  //  BInsert->createAll(System,*VPipeBE,2,bunkerObj);
    //  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);
  
     TPipeWall->setFront(bunkerObj,-1);
  //  TPipeWall->setBack(*BInsert,8);
   
  TPipeWall->createAll(System,BenderE->getKey("Guide0"),2,BenderE->getKey("Guide0"),2);


  TPipeWall1->setFront(TPipeWall->getKey("Guide0"),2);
  
  TPipeWall1->createAll(System,TPipeWall->getKey("Guide0"),2,TPipeWall->getKey("Guide0"),2);

  
  TPipeWall2->setFront(TPipeWall1->getKey("Guide0"),2);
  // TPipeWall2->setBack(*BInsert,-2);
  
  TPipeWall2->createAll(System,TPipeWall1->getKey("Guide0"),2,TPipeWall1->getKey("Guide0"),2);

  
  TPipeWall3->setFront(TPipeWall2->getKey("Guide0"),2);
  TPipeWall3->setBack(bunkerObj,-2);
  
  TPipeWall3->createAll(System,TPipeWall2->getKey("Guide0"),2,TPipeWall2->getKey("Guide0"),2);

  attachSystem::addToInsertSurfCtrl(System,ShieldGap1->getCell("Void"),*TPipeWall2);   
  attachSystem::addToInsertSurfCtrl(System,*BInsert,*TPipeWall);  

  attachSystem::addToInsertSurfCtrl(System,*BInsert,*TPipeWall1);  
  attachSystem::addToInsertSurfCtrl(System,*BInsert,*TPipeWall2);  

  
  //  VPipeWall->addInsertCell(BInsert->getCell("Void"));
  //  VPipeWall->createAll(System,*BInsert,-1);

  // using 7 : mid point
  //  FocusWall->addInsertCell(BInsert->getCells("Item"));
  //  FocusWall->createAll(System,*BInsert,7,*BInsert,7);


  //  TPipeWall->addInsertCell(BInsert->getCells("Item"));
  //  TPipeWall->setFront(*BInsert,-1);
  //  TPipeWall->setBack(*BInsert,-2);
  // TPipeWall->createAll(System,*BInsert,7,*BInsert,7);


   
  FocusWall->addInsertCell(TPipeWall->getCells("Guide0Void"));
  FocusWall->addInsertCell(TPipeWall1->getCells("Guide0Void"));
  // FocusWall->addInsertCell(TPipeWall2->getCells("Guide0Void"));
  FocusWall->setFront(TPipeWall->getKey("Guide0"),-1);
  FocusWall->createAll(System,BenderE->getKey("Guide0"),2,BenderE->getKey("Guide0"),2);

  FocusWall1->setFront(FocusWall->getKey("Guide0"),2);
  FocusWall1->addInsertCell(TPipeWall2->getCells("Guide0Void"));
 
  FocusWall1->createAll(System,BenderE->getKey("Guide0"),2,BenderE->getKey("Guide0"),2);

  FocusWall2->setFront(FocusWall1->getKey("Guide0"),2);
 FocusWall2->addInsertCell(TPipeWall2->getCells("Guide0Void"));
  FocusWall2->addInsertCell(TPipeWall3->getCells("Guide0Void"));
 FocusWall2->setBack(TPipeWall3->getKey("Guide0"),-2);
 FocusWall2->createAll(System,BenderE->getKey("Guide0"),2,BenderE->getKey("Guide0"),2);




 //Cuting in pieces for variance reduction
 {
   std::array<std::shared_ptr<beamlineSystem::GuideLine>,20> FSplit;
    for (size_t i=0;i<20;i++){  
      FSplit[i]=std::shared_ptr<beamlineSystem::GuideLine>
	(new beamlineSystem::GuideLine("bifrostSplit"+std::to_string(i)));
       OR.addObject ("bifrostSplit"+std::to_string(i),FSplit[i]);
    
    if (i==0) FSplit[i]->createAll(System,FocusWall->getKey("Guide0"),-1,
				   FocusWall->getKey("Guide0"),-1);
    else  FSplit[i]->createAll(System,FSplit[i-1]->getKey("Guide0"),2,FSplit[i-1]->getKey("Guide0"),2);
    //createAll(System,*bifrostAxis,-3,*bifrostAxis,-3);
    
    //std::cout << "bbb" << std::endl;

}
    //    delete FSplit;


    
    std::vector<int> iFsplit=FocusWall->getCells("Full");
    
    for (size_t j=0;j<iFsplit.size();j++){
      int icell=iFsplit[j];
  for (size_t i=0;i<16;i++){  

  int inew=System.splitObject(icell,
				 FSplit[i]->getKey("Guide0").getLinkSurf(2));
  icell=inew;
	    }
    }

        iFsplit=FocusWall2->getCells("Full");
    
    for (int j=0;j<iFsplit.size();j++){
      int icell=iFsplit[j];
      for (size_t i=18;i<19;i++){  

  int inew=System.splitObject(icell,
				 FSplit[i]->getKey("Guide0").getLinkSurf(2));
  icell=inew;
	    }
}

    

   iFsplit=BInsert->getCells();
    
      int icell=iFsplit[0];
  for (size_t i=0;i<10;i++){  

  int inew=System.splitObject(icell,
				 FSplit[i]->getKey("Guide0").getLinkSurf(2));
  if (i==0) System.findObject(icell)->setMaterial(0);
  icell=inew;
	    }

     icell=iFsplit[2];
    for (size_t i=10;i<18;i++){  

  int inew=System.splitObject(icell,
				 FSplit[i]->getKey("Guide0").getLinkSurf(2));
  icell=inew;
	    }
    

    
    iFsplit=TPipeWall->getCells("Full");
    
    for (size_t j=0;j<iFsplit.size();j++){
      int icell=iFsplit[j];
  for (size_t i=0;i<10;i++){  

  int inew=System.splitObject(icell,
				 FSplit[i]->getKey("Guide0").getLinkSurf(2));
  icell=inew;
	    }
}


    iFsplit=TPipeWall1->getCells("Full");
    
    for (size_t j=0;j<iFsplit.size();j++){
      int icell=iFsplit[j];
  for (size_t i=11;i<17;i++){  

  int inew=System.splitObject(icell,
				 FSplit[i]->getKey("Guide0").getLinkSurf(2));
  icell=inew;
	    }
}

    
    
    iFsplit=TPipeWall2->getCells("Full");
    
    for (size_t j=0;j<iFsplit.size();j++){
      int icell=iFsplit[j];
  for (size_t i=18;i<19;i++){  

  int inew=System.splitObject(icell,
				 FSplit[i]->getKey("Guide0").getLinkSurf(2));
  icell=inew;
	    }
}

   
    

    
    for (size_t i=0;i<20;i++){  
      std::vector<int> iFsplit= FSplit[i]->getCells();
    for (size_t j=0;j<iFsplit.size();j++){
      System.removeCell(iFsplit[j]);
    }      
}
   

 }
 


  if (stopPoint==3)
    {
    attachSystem::addToInsertForced(System,ShieldGap1->getCells("Void"),*TPipeWall3);  
    return;                      // STOP Out of bunker
    }




 
  
  std::cout << "Building outside bunker" << std::endl;
  int newVoid=System.splitObject(voidCell,
				 FocusWall->getKey("Guide0").getLinkSurf(2));
  System.findObject(newVoid)->setMaterial(60); // air around bunker
   System.setMaterialDensity(newVoid); // air around bunker

  
   //  buildCommonShield(System,bunkerObj,voidCell);
     buildCommonShield(System,bunkerObj,newVoid);

 return;
}


  
int
BIFROST::buildCommonShield(Simulation& System,
		       //		       const attachSystem::FixedComp& FTA,
		       //                       const long int thermalIndex,
		       //		       const CompBInsert& BInsert,
		       const Bunker& bunkerObj,
		       const int voidCell)
  /*!
Build all the components between bunker and cave
    \param System :: simulation                    
    \param FTA :: FixedComp for thermal guide      
    \param thermalIndex :: FixedComp link point for thermal
    \param BInsert :: Bunker insert which has to be excluded from shielding volume 
    \param Bunker :: Bunker object to link the gap to
    \param voidCell :: cell to place objects in            

  */
{

 
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  FuncDataBase& Control=System.getDataBase();

  ELog::RegMethod RegA("BIFROST","buildOutGuideCommonShield");

  // ShieldGap1->addInsertCell(voidCell);

   
  //   ShieldGap1->setFront(bunkerObj,2);
      //  ShieldGap2->setFront(bunkerObj,2);
 

  //  ShieldGap1->createAll(System,FocusWall2->getKey("Guide0"),2);

    
 
       D03FirstSupport->addInsertCell(ShieldGap1->getCell("Void"));
   
       //  D03FirstSupport->createAll(System,FocusWall2->getKey("Guide0"),2);
       D03FirstSupport->createAll(System,*ShieldGap1,-1);


       
       attachSystem::addToInsertSurfCtrl(System,ShieldGap1->getCells("Wall"),*D03FirstSupport);

    attachSystem::addToInsertSurfCtrl(System,bunkerObj,"floor",*D03FirstSupport);


    /*

std::shared_ptr<constructSystem::InnerShield> ConcreteLowerPlate;
      ConcreteLowerPlate=std::shared_ptr<constructSystem::InnerShield>
      (new constructSystem::InnerShield("bifrostConcreteLowerPlate"));
    OR.addObject ("bifrostConcreteLowerPlate",B4CLowerPlate);

    ConcreteLowerPlate->createAll(System,*D03FirstSupport,-1);

    attachSystem::addToInsertSurfCtrl(System,*D03FirstSupport,*ConcreteLowerPlate);

    */
    
   std::array<std::shared_ptr<constructSystem::D03SupportBlock>,9> D03SupportSystem;
    for (int i=0;i<9;i++){  
      D03SupportSystem[i]=std::shared_ptr<constructSystem::D03SupportBlock>
	(new constructSystem::D03SupportBlock("D03Support"+std::to_string(i)));
    OR.addObject ("D03Support"+std::to_string(i),D03SupportSystem[i]);
    
    if (i==0) D03SupportSystem[i]->createAll(System,*D03FirstSupport,2);
    else  D03SupportSystem[i]->createAll(System,*D03SupportSystem[i-1],2);
}

   int splitIndex = D03FirstSupport->getLinkSurf(1); 

   int voidCellNew;
      voidCellNew=System.splitObject(ShieldGap1->getCell("Void"),-splitIndex);




      
     //     std::cout << "Shutter" << std::endl;
    std::shared_ptr<constructSystem::D03SupportBlock> D03ShutterSupport1=
      std::shared_ptr<constructSystem::D03SupportBlock>
	(new constructSystem::D03SupportBlock("bifrostD03ShutterSupport1"));
      
      OR.addObject ("bifrostD03ShutterSupport1",D03ShutterSupport1);
      
      //                 D03ShutterSupport1->addInsertCell(voidRear);
		//  D03ShutterSupport1->setFront(*D03SupportSystem[9],2);
	   D03ShutterSupport1->createAll(System,*D03SupportSystem[8],2);
	
      std::shared_ptr<constructSystem::D03SupportBlock> D03ShutterSupport2=
	std::shared_ptr<constructSystem::D03SupportBlock>
	(new constructSystem::D03SupportBlock("bifrostD03ShutterSupport2"));
      OR.addObject ("bifrostD03ShutterSupport2",D03ShutterSupport2);
    
      //    D03ShutterSupport2->addInsertCell(voidRear);
	         D03ShutterSupport2->createAll(System,*D03ShutterSupport1,2);

	  
  
         std::shared_ptr<constructSystem::D03SupportBlock> D03LastSupport=
	std::shared_ptr<constructSystem::D03SupportBlock>
	(new constructSystem::D03SupportBlock("bifrostD03LastSupport"));
      OR.addObject ("bifrostD03LastSupport",D03LastSupport);
    
      //            D03LastSupport->addInsertCell(E02Floor);
       D03LastSupport->createAll(System,*D03ShutterSupport2,2);
      splitIndex = D03LastSupport->getLinkSurf(2);     
     int  E02Void =  System.splitObject(voidCell,splitIndex);

      //      System.findObject(E02Floor)->setMaterial(144);
      //   System.setMaterialDensity(E02Floor);
     	 
     //
     //	int E02Void;
	//	E02Void=    System.splitObject(voidTop,D03ShutterSupport2->getLinkSurf(2));
	//	E02Floor=    System.splitObject(E02Void,D03FirstBase->getLinkSurf(6));
	// System.findObject(E02Floor)->setMaterial(144); // air around bunker
	//System.setMaterialDensity(E02Floor); // air around bunker


      
		 int voidRight, D03Floor, E02Floor, voidShutterSupport, voidBottom,voidLeft, voidFront,voidTop;

      splitIndex = D03FirstSupport->getLinkSurf(6);
     voidTop = System.splitObject(ShieldGap1->getCell("Void"),splitIndex);
     voidTop =  System.splitObject(voidCell,splitIndex);

    
      splitIndex = D03FirstSupport->getLinkSurf(5);
      D03Floor =  System.splitObject(voidCell,splitIndex);

      
      System.findObject(D03Floor)->setMaterial(144);
      System.setMaterialDensity(D03Floor);


          splitIndex = D03ShutterSupport2->getLinkSurf(2);     
         E02Floor =  System.splitObject(voidCell,splitIndex);
	 E02Floor =  System.splitObject(E02Floor,-D03LastSupport->getLinkSurf(3));
	 E02Floor =  System.splitObject(E02Floor,-D03LastSupport->getLinkSurf(4));
	 System.removeCell(E02Floor);



      splitIndex = D03SupportSystem[8]->getLinkSurf(2);     
      voidShutterSupport=System.splitObject(voidCell,splitIndex);
     

    splitIndex = D03FirstSupport->getLinkSurf(4);
    //  voidRight=System.splitObject(ShieldGap1->getCell("Void"),splitIndex);
    voidRight =  System.splitObject(voidCell,splitIndex);
    
      splitIndex = D03FirstSupport->getLinkSurf(3);
      //       voidLeft=System.splitObject(ShieldGap1->getCell("Void"),splitIndex);
      voidLeft =  System.splitObject(voidCell,splitIndex);

      splitIndex = D03ShutterSupport2->getLinkSurf(4);
    //  voidRight=System.splitObject(ShieldGap1->getCell("Void"),splitIndex);
    voidRight =  System.splitObject(voidShutterSupport,splitIndex);
    
      splitIndex = D03ShutterSupport2->getLinkSurf(3);
      //       voidLeft=System.splitObject(ShieldGap1->getCell("Void"),splitIndex);
      voidLeft =  System.splitObject(voidShutterSupport,splitIndex);


         splitIndex = D03FirstSupport->getLinkSurf(1);
      voidFront =  System.splitObject(voidCell,splitIndex);
      
      System.removeCell(voidCell);
      System.removeCell(voidShutterSupport);

     
     

    D03FirstBase->addInsertCell(voidCellNew);
    //   D03FirstBase->addInsertCell(voidTop);
    D03FirstBase->createAll(System,*D03FirstSupport,-1);
    std::array<std::shared_ptr<constructSystem::D03BaseBlock>,4> D03BaseSystem;
    int ireverse(1);
    for (int i=0;i<4;i++){
      if(ireverse){
      	Control.addVariable("bifrostD03Base"+std::to_string(i)+"XYAngle",180);
      	Control.addVariable("bifrostD03Base"+std::to_string(i)+"YStep",400);}
     D03BaseSystem[i]=std::shared_ptr<constructSystem::D03BaseBlock>
	(new constructSystem::D03BaseBlock("bifrostD03Base"+std::to_string(i)));
    OR.addObject ("bifrostD03Base"+std::to_string(i),D03BaseSystem[i]);
    
    if (i==0) D03BaseSystem[i]->createAll(System,*D03FirstBase,2);
    else  D03BaseSystem[i]->createAll(System,*D03BaseSystem[i-1],ireverse+1);
    //    std::cout<<ireverse<<std::endl;
    ireverse=abs(ireverse-1);
    }


    int iRemove;

    std::array<std::shared_ptr<constructSystem::D03BaseBlock>,3> D03BaseSteelStart;
     D03BaseSteelStart[0]= std::shared_ptr<constructSystem::D03BaseBlock>
      (new constructSystem::D03BaseBlock("bifrostD03BaseSteelStart0"));
     D03BaseSteelStart[1]= std::shared_ptr<constructSystem::D03BaseBlock>
      (new constructSystem::D03BaseBlock("bifrostD03BaseSteelStart1"));
     D03BaseSteelStart[2]= std::shared_ptr<constructSystem::D03BaseBlock>
      (new constructSystem::D03BaseBlock("bifrostD03BaseSteelStart2"));

     //     D03BaseSteelStart[0]->addInsertCell(D03FirstBase->getCells("WallFrontL")); 
     //  D03BaseSteelStart[0]->addInsertCell(D03FirstBase->getCells("WallFrontR")); 
         D03BaseSteelStart[0]->setBack(*D03FirstBase,8);
      D03BaseSteelStart[0]->setFront(*D03FirstBase,-1);
    D03BaseSteelStart[0]->createAll(System,*D03FirstSupport,-1);

    //    attachSystem::addToInsertSurfCtrl(System,*D03FirstSupport,*B4CLowerPlate);

    //    System.findObject(B4CLowerPlate->getCell("Roof"))->setMaterial(141);


    
     D03BaseSteelStart[1]->setFront(*D03FirstBase,-1);
     D03BaseSteelStart[1]->setBack(*D03FirstBase,8);
     //   D03BaseSteelShutter->addInsertCell(D03BaseShutter[0]->getCell("VoidLow"));
     //      D03BaseSteelStart[0]->addInsertCell(D03FirstBase->getCell("VoidF"));

     //    D03BaseSteelStart[1]->createAll(System, *D03BaseSteelStart[0],2);
         D03BaseSteelStart[1]->createAll(System, *D03FirstSupport,-1);


 iRemove=D03FirstBase->getCell("VoidF");
     System.removeCell(iRemove);

    iRemove=System.splitObject(D03BaseSteelStart[0]->getCell("VoidF"),
		      -D03BaseSteelStart[1]->getLinkSurf(6));
     System.removeCell(iRemove);
    iRemove=System.splitObject(D03BaseSteelStart[0]->getCell("VoidB"),
		      -D03BaseSteelStart[1]->getLinkSurf(6));
     System.removeCell(iRemove);

     System.removeCell(D03BaseSteelStart[1]->getCell("VoidLow"));
        System.removeCell(D03BaseSteelStart[0]->getCell("VoidLow"));

       
	     D03BaseSteelStart[2]->setFront(*D03FirstBase,7);
         D03BaseSteelStart[2]->setBack(*D03BaseSystem[0],7);
     //   D03BaseSteelShutter->addInsertCell(D03BaseShutter[0]->getCell("VoidLow"));
     //      D03BaseSteelStart[0]->addInsertCell(D03FirstBase->getCell("VoidF"));
     
     D03BaseSteelStart[2]->createAll(System,*D03FirstBase,-7);


    iRemove=System.splitObject(D03FirstBase->getCell("VoidB"),
			      -D03BaseSteelStart[2]->getLinkSurf(6));
    
     System.removeCell(iRemove);
    iRemove=System.splitObject(D03BaseSystem[0]->getCell("VoidB"),
			      -D03BaseSteelStart[2]->getLinkSurf(6));
    
     System.removeCell(iRemove);
        System.removeCell(D03BaseSteelStart[2]->getCell("VoidLow"));


std::shared_ptr<constructSystem::InnerShield> B4CLowerPlate;
      B4CLowerPlate=std::shared_ptr<constructSystem::InnerShield>
      (new constructSystem::InnerShield("bifrostB4CLowerPlate"));
    OR.addObject ("bifrostB4CLowerPlate",B4CLowerPlate);
    B4CLowerPlate->addInsertCell(D03BaseSteelStart[1]->getCell("VoidF"));
    B4CLowerPlate->addInsertCell(D03FirstBase->getCell("VoidLow"));
    B4CLowerPlate->createAll(System,*D03FirstSupport,-1);



    
std::shared_ptr<constructSystem::InnerShield> ExtrusionPlate;
      ExtrusionPlate=std::shared_ptr<constructSystem::InnerShield>
      (new constructSystem::InnerShield("bifrostExtrusionPlate"));
    OR.addObject ("bifrostExtrusionPlate",ExtrusionPlate);
    ExtrusionPlate->addInsertCell(D03BaseSteelStart[1]->getCell("VoidB"));
    ExtrusionPlate->addInsertCell(D03FirstBase->getCell("VoidLow"));
    ExtrusionPlate->createAll(System,*D03FirstSupport,-1);


	


      int voidRight1, voidBottom1,voidLeft1, voidFront1,voidTop1,voidRear1;
       splitIndex = D03FirstBase->getLinkSurf(6);
      voidTop1 =  System.splitObject(voidTop,splitIndex);

    

     splitIndex = D03FirstBase->getLinkSurf(4);
     voidRight1 =  System.splitObject(voidTop,splitIndex);

      splitIndex = D03FirstBase->getLinkSurf(3);
      voidLeft1 =  System.splitObject(voidTop,splitIndex);

           splitIndex = D03BaseSystem[3]->getLinkSurf(2);
           voidRear1 =  System.splitObject(voidTop,splitIndex);


      
      
      splitIndex = D03FirstBase->getLinkSurf(1);
      voidFront1 =  System.splitObject(voidTop,splitIndex);
      
     System.removeCell(voidTop);
     
    
    
     //    D03FirstUpper->addInsertCell(voidTop1);
    D03FirstUpper->addInsertCell(voidCellNew);
    D03FirstUpper->createAll(System,*D03FirstBase,-1);
    // D03SecondUpper->addInsertCell(voidTop1);
    D03SecondUpper->createAll(System,*D03FirstUpper,2);
      
     
  std::array<std::shared_ptr<constructSystem::D03BaseBlock>,3> D03BaseShutter;
  ireverse=0;
    for (int i=0;i<3;i++){
      if((ireverse)&&(i==1)){
      	Control.addVariable("bifrostD03BaseShutter"+std::to_string(i)+"XYAngle",180);
//      	Control.addVariable("bifrostD03BaseShutter"+std::to_string(i)+"YStep",286);}
     	Control.addVariable("bifrostD03BaseShutter"+std::to_string(i)+"YStep",205);}
      if (i==2){
	Control.addVariable("bifrostD03BaseShutter"+std::to_string(i)+"XYAngle",180);
//      	Control.addVariable("bifrostD03BaseShutter"+std::to_string(i)+"YStep",286);}
     	Control.addVariable("bifrostD03BaseShutter"+std::to_string(i)+"YStep",325);}
     D03BaseShutter[i]=std::shared_ptr<constructSystem::D03BaseBlock>
	(new constructSystem::D03BaseBlock("bifrostD03BaseShutter"+std::to_string(i)));
    OR.addObject ("bifrostD03BaseShutter"+std::to_string(i),D03BaseShutter[i]);
    
    D03BaseShutter[i]->addInsertCell(voidRear1);
    if (i==0) D03BaseShutter[i]->createAll(System,*D03BaseSystem[3],2);
    else if (i==1) D03BaseShutter[i]->createAll(System,*D03BaseShutter[i-1],ireverse+1);
    else D03BaseShutter[i]->createAll(System,*D03BaseShutter[i-1],1);
    //    std::cout<<ireverse<<std::endl;
    if( i==0) ireverse=abs(ireverse-1);
    }


  
    
   
    
   int   voidRearShutter =
      System.splitObject(voidRear1,D03BaseShutter[1]->getLinkSurf(1));



   
    std::array<std::shared_ptr<constructSystem::D03UpperBlock>,18> D03UpperSystem;
    ireverse=1;
    for (int i=0;i<18;i++){
      if(ireverse){
      	Control.addVariable("bifrostD03Upper"+std::to_string(i)+"XYAngle",180);
      	Control.addVariable("bifrostD03Upper"+std::to_string(i)+"YStep",100);}
     D03UpperSystem[i]=std::shared_ptr<constructSystem::D03UpperBlock>
	(new constructSystem::D03UpperBlock("bifrostD03Upper"+std::to_string(i)));
    OR.addObject ("bifrostD03Upper"+std::to_string(i),D03UpperSystem[i]);
    
    if (i==0) D03UpperSystem[i]->createAll(System,*D03SecondUpper,2);
    else  D03UpperSystem[i]->createAll(System,*D03UpperSystem[i-1],ireverse+1);
    //    std::cout<<ireverse<<std::endl;
    ireverse=abs(ireverse-1);
    }

    
   
    
   int voidRight2, voidBottom2,voidLeft2, voidFront2,voidTop2,voidRear2;

      splitIndex = D03UpperSystem[17]->getLinkSurf(2);
      voidRear2 =  System.splitObject(voidTop1,splitIndex);

      splitIndex = D03FirstUpper->getLinkSurf(4);
     // voidRight2 =  System.splitObject(voidRear1,splitIndex);
     voidTop1=  System.splitObject(voidTop1,-splitIndex);
    
      splitIndex = D03FirstUpper->getLinkSurf(3);
      //  voidLeft2 =  System.splitObject(voidRear1,splitIndex);
      voidTop1=  System.splitObject(voidTop1,-splitIndex);

        
     splitIndex = D03SecondUpper->getLinkSurf(6);
     //voidTop2 =  System.splitObject(voidRear1,splitIndex);
     voidTop1=  System.splitObject(voidTop1,-splitIndex);
      
    
    System.removeCell(voidTop1);
 
    


   

      /*        
  splitIndex = D03FirstUpper->getLinkSurf(1);
      voidFront2 =  System.splitObject(voidRear1,splitIndex);
      */
      
std::shared_ptr<constructSystem::D03UpperBlock> D03UpperShutterStart =
  std::shared_ptr<constructSystem::D03UpperBlock>
		 (new constructSystem::D03UpperBlock("bifrostD03UpperShutterStart"));
  OR.addObject ("bifrostD03UpperShutterStart",D03UpperShutterStart);
      	Control.addVariable("bifrostD03UpperShutterStartXYAngle",180);
      	Control.addVariable("bifrostD03UpperShutterStartYStep",72.5);

     ireverse=abs(ireverse-1);

 D03UpperShutterStart->addInsertCell(voidRear2);
  D03UpperShutterStart->createAll(System, *D03UpperSystem[17],2);
 voidRear2 = System.splitObject(voidRear2,D03UpperShutterStart->getLinkSurf(1));

 
 //  ireverse=abs(ireverse-1);
  std::array<std::shared_ptr<constructSystem::D03UpperBlock>,2> D03UpperShutter;
    for (int i=0;i<2;i++){
            if(ireverse){
      	Control.addVariable("bifrostD03UpperShutter"+std::to_string(i)+"XYAngle",180);
      	Control.addVariable("bifrostD03UpperShutter"+std::to_string(i)+"YStep",63.5);}
      D03UpperShutter[i]=std::shared_ptr<constructSystem::D03UpperBlock>
      (new constructSystem::D03UpperBlock("bifrostD03UpperShutter"+std::to_string(i)));
    OR.addObject ("bifrostD03UpperShutter"+std::to_string(i),D03UpperShutter[i]);
    
    D03UpperShutter[i]->addInsertCell(voidRear2);
    if (i==0) D03UpperShutter[i]->createAll(System,*D03UpperShutterStart,1);
    else D03UpperShutter[i]->createAll(System,*D03UpperShutter[i-1],ireverse+1);
    //    std::cout<<ireverse<<std::endl;
    ireverse=abs(ireverse-1);
    }

 voidRear2 = System.splitObject(voidRear2,D03UpperShutter[1]->getLinkSurf(1));
    

    D03LeadFirstA->setFront(*D03FirstUpper,-1);
    D03LeadFirstA->addInsertCell(D03BaseSteelStart[0]->getCell("VoidF"));
    D03LeadFirstA->addInsertCell(D03BaseSteelStart[0]->getCell("VoidB"));

    /*
std::shared_ptr<constructSystem::InnerShield> FirstSupportInsertion;
      FirstSupportInsertion=std::shared_ptr<constructSystem::InnerShield>
      (new constructSystem::InnerShield("bifrostFirstSupportInsertion"));
    OR.addObject ("bifrostFirstSupportInsertion",FirstSupportInsertion);
    */

    
std::shared_ptr<constructSystem::InnerShield> LeadUpperPlate;
      LeadUpperPlate=std::shared_ptr<constructSystem::InnerShield>
      (new constructSystem::InnerShield("bifrostLeadUpperPlate"));
    OR.addObject ("bifrostLeadUpperPlate",LeadUpperPlate);
    
     std::vector<int>  iWalls = D03FirstUpper->getCells("WallFrontT");
    for (size_t i=0;i<iWalls.size();i++){
      LeadUpperPlate->addInsertCell(D03FirstUpper->getCells("WallFrontT")[i]);
      
    }
   LeadUpperPlate->createAll(System,*D03FirstUpper,-1);
    
    D03LeadFirstA->createAll(System,*D03FirstUpper,-1);
   System.splitObject(D03BaseSteelStart[0]->getCell("VoidB"),-D03LeadFirstA->getLinkSurf(2));
 
    iWalls = D03LeadFirstA->getCells("Wall");
    for (size_t i =0; i<iWalls.size(); i++){
      System.findObject(iWalls[i])->setMaterial(152) ; // Lead L51121
      System.setMaterialDensity(iWalls[i]);
    }

    
    //    D03SteelFirstA->setFront(*D03FirstUpper,-1);
    // D03SteelFirstA->setBack(*D03FirstUpper,8);
    //   D03SteelFirstA->addInsertCell(D03FirstBase->getCell("VoidF"));
    D03SteelFirstA->createAll(System,*D03LeadFirstA,-2);

    D03B4CFirstA->createAll(System,*D03SteelFirstA,2);

    

    iWalls = D03LeadFirstA->getCells("Void");
    for (size_t i =0; i<iWalls.size(); i++){
     
  iRemove=System.splitObject(iWalls[i], -D03B4CFirstA->getLinkSurf(2));

  System.removeCell(iRemove);

    }



    
    iWalls = D03LeadFirstA->getCells("WallLining");
    for (size_t i =0; i<iWalls.size(); i++){
     
  iRemove=System.splitObject(iWalls[i], -D03B4CFirstA->getLinkSurf(2));

  System.removeCell(iRemove);

    }

    
    iWalls = D03LeadFirstA->getCells("RoofLining");
    for (size_t i =0; i<iWalls.size(); i++){
     
  iRemove=System.splitObject(iWalls[i], -D03B4CFirstA->getLinkSurf(2));

  System.removeCell(iRemove);

    }
    
    iWalls = D03LeadFirstA->getCells("B4CCorner");
    for (size_t i =0; i<iWalls.size(); i++){
     
  iRemove=System.splitObject(iWalls[i], -D03B4CFirstA->getLinkSurf(2));

  System.removeCell(iRemove);

    }

   iWalls = D03SteelFirstA->getCells("Roof");
    for (size_t i =0; i<iWalls.size(); i++){
     
      //  iRemove=System.splitObject(iWalls[i], -D03SteelFirstA->getLinkSurf(2));

  System.removeCell(iWalls[i]);

    }

   iWalls = D03B4CFirstA->getCells("Roof");
    for (size_t i =0; i<iWalls.size(); i++){
     
      //  iRemove=System.splitObject(iWalls[i], -D03SteelFirstA->getLinkSurf(2));

  System.removeCell(iWalls[i]);

    }


   iWalls = D03SteelFirstA->getCells("Corner");
    for (size_t i =0; i<iWalls.size(); i++){
     
      //  iRemove=System.splitObject(iWalls[i], -D03SteelFirstA->getLinkSurf(2));

  System.removeCell(iWalls[i]);

    }

    

   iWalls = D03B4CFirstA->getCells("Corner");
    for (size_t i =0; i<iWalls.size(); i++){
     
      //  iRemove=System.splitObject(iWalls[i], -D03SteelFirstA->getLinkSurf(2));

  System.removeCell(iWalls[i]);

    }


   iWalls = D03LeadFirstA->getCells("Wall");
    for (size_t i =0; i<iWalls.size(); i++){
     
      System.splitObject(iWalls[i], -D03B4CFirstA->getLinkSurf(2));

    }

    
    
    D03SteelFirstB->setFront(*D03LeadFirstA,2);
    D03SteelFirstB->setBack(*D03FirstUpper,8);
    D03SteelFirstB->addInsertCell(D03BaseSteelStart[0]->getCell("VoidB"));
    //  D03LeadFirstA->addInsertCell(D03BaseSteelStart[0]->getCell("VoidB"));
     D03SteelFirstB->createAll(System,*D03LeadFirstA,2);
   System.splitObject(D03BaseSteelStart[0]->getCell("VoidB"),-D03SteelFirstB->getLinkSurf(2));

    attachSystem::addToInsertSurfCtrl(System,D03LeadFirstA->getCell("Void"),
				      *TPipeWall2);
    attachSystem::addToInsertSurfCtrl(System,D03LeadFirstA->getCell("Void"),
				      *TPipeWall3);
    attachSystem::addToInsertSurfCtrl(System,D03SteelFirstA->getCell("Void"),
    				      *TPipeWall3);
    attachSystem::addToInsertSurfCtrl(System,D03B4CFirstA->getCell("Void"),
    				      *TPipeWall3);
     
    
    System.removeCell(D03FirstUpper->getCell("VoidF"));

    std::array<std::shared_ptr<constructSystem::D03SteelInsert>,21>
               D03SteelInsertSystemF;

    std::array<std::shared_ptr<constructSystem::D03SteelInsert>,21>
               D03SteelInsertSystemB;

    std::array<std::shared_ptr<constructSystem::D03SteelSpacing>,21>
               D03SteelSpacingSystem;

           Control.addVariable("bifrostD03SteelSystemF0Length",100);
	   Control.addVariable("bifrostD03SteelSystemF0Left",46.5);
	   Control.addVariable("bifrostD03SteelSystemF0Right",46.5);
	   Control.addVariable("bifrostD03SteelSystemF0Height",60);
	   // Control.addVariable("bifrostD03SteelSystemF0VoidLeft",31.5);
	   //  Control.addVariable("bifrostD03SteelSystemF0VoidRight",31.5);
	   //  Control.addVariable("bifrostD03SteelSystemF0VoidHeight",46.5);
            Control.addVariable("bifrostD03SteelSystemF0VoidLeft",24.5);
	    Control.addVariable("bifrostD03SteelSystemF0VoidRight",24.5);
	    Control.addVariable("bifrostD03SteelSystemF0VoidHeight",40);
	    Control.addVariable("bifrostD03SteelSystemF0NWallLayersR",5);
	    Control.addVariable("bifrostD03SteelSystemF0NWallLayersL",5);
	    Control.addVariable("bifrostD03SteelSystemF0NRoofLayers",5);
	    Control.addVariable("bifrostD03SteelSystemF0NSeg",4);


Control.addVariable("bifrostD03SteelSystemF0InnerB4CLayerThick",1.5);
Control.addVariable("bifrostD03SteelSystemF0B4CTileMat","B4CConcrete");

 Control.addVariable("bifrostD03SteelSystemF19InnerB4CLayerThick",1.5);
Control.addVariable("bifrostD03SteelSystemF19B4CTileMat","B4CConcrete");

	   
	   Control.addVariable("bifrostD03SteelEnforcementLeft",31.5);
	   Control.addVariable("bifrostD03SteelEnforcementRight",31.5);
	   Control.addVariable("bifrostD03SteelEnforcementVoidLeft",31.5-7.5);
	   Control.addVariable("bifrostD03SteelEnforcementVoidRight",31.5-7.5);
	   Control.addVariable("bifrostD03SteelEnforcementHeight",46.5);
	   Control.addVariable("bifrostD03SteelEnforcementLength",20.0);
	   Control.addVariable("bifrostD03SteelEnforcementVoidHeight",46.5-7.5);

	   
std::shared_ptr<constructSystem::D03SteelInsert> D03SteelEnforcement    =
  std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostD03SteelEnforcement"));
 OR.addObject ("bifrostD03SteelEnforcement",D03SteelEnforcement);

    
    
     D03SteelInsertSystemF[0]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostD03SteelSystemF"+std::to_string(0)));
    OR.addObject ("bifrostD03SteelSystemF"+std::to_string(0),
		  D03SteelInsertSystemF[0]);
     D03SteelInsertSystemB[0]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostD03SteelSystemB"+std::to_string(0)));
    OR.addObject ("bifrostD03SteelSystemB"+std::to_string(0),
		  D03SteelInsertSystemB[0]);
     D03SteelSpacingSystem[0]=
       std::shared_ptr<constructSystem::D03SteelSpacing>
(new constructSystem::D03SteelSpacing("bifrostD03SpacingSystemB"+std::to_string(0)));
    OR.addObject ("bifrostD03SpacingSystemB"+std::to_string(0),
		  D03SteelSpacingSystem[0]);

    



    
    D03SteelInsertSystemF[0]->setFront(*D03FirstUpper,7);
    D03SteelInsertSystemF[0]->setBack(*D03SecondUpper,8);
    D03SteelInsertSystemF[0]->addInsertCell(D03BaseSteelStart[0]->getCell("VoidB"));
    D03SteelInsertSystemF[0]->addInsertCell(D03FirstBase->getCell("VoidGapC"));
    D03SteelInsertSystemF[0]->createAll(System,*D03FirstUpper,-7);
   
 //   D03SteelEnforcement->addInsertCell( D03SteelInsertSystemF[0]->getCell("Void"));
    if(0){
    D03SteelEnforcement->createAll(System, *D03SteelInsertSystemF[0],-2);

  iRemove=System.splitObject(D03SteelInsertSystemF[0]->getCell("Void"),
			     -D03SteelEnforcement->getLinkSurf(2));

  System.removeCell(iRemove);

  
  VPipeOutB->addInsertCell(D03SteelEnforcement->getCell("Void"));
    }
     iRemove=D03SecondUpper->getCell("VoidF");
      System.removeCell(iRemove);

     iRemove=D03FirstUpper->getCell("VoidB");
      System.removeCell(iRemove);


 
    D03SteelInsertSystemB[0]->setFront(*D03SecondUpper,7);
    D03SteelInsertSystemB[0]->setBack(*D03SecondUpper,-2);
    D03SteelInsertSystemB[0]->createAll(System,*D03SecondUpper,7);

     iRemove=D03SecondUpper->getCell("VoidB");
      System.removeCell(iRemove);

      
    
    /*
    D03SteelSpacingSystem[0]->setBack(*D03SecondUpper,-7);
    D03SteelSpacingSystem[0]->setFront(*D03SecondUpper,-8);
    D03SteelSpacingSystem[0]->createAll(System,*D03SecondUpper,-8);
    iRemove=D03SecondUpper->getCell("VoidGapC");
    System.removeCell(iRemove);
      */
      //  
   
      
         
    for (int i=0;i<19;i++){
     D03SteelInsertSystemF[i+1]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostD03SteelSystemF"+std::to_string(i+1)));
    OR.addObject ("bifrostD03SteelSystemF"+std::to_string(i+1),
		  D03SteelInsertSystemF[i+1]);
     D03SteelInsertSystemB[i+1]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostD03SteelSystemB"+std::to_string(i+1)));
    OR.addObject ("bifrostD03SteelSystemB"+std::to_string(i+1),
		  D03SteelInsertSystemB[i+1]);
     D03SteelSpacingSystem[i+1]=
       std::shared_ptr<constructSystem::D03SteelSpacing>
(new constructSystem::D03SteelSpacing("bifrostD03SpacingSystem"+std::to_string(i+1)));
    OR.addObject ("bifrostD03SpacingSystem"+std::to_string(i+1),
		  D03SteelSpacingSystem[i+1]);

    if (i<18){
    D03SteelInsertSystemF[i+1]->setFront(*D03UpperSystem[i],-1);
    D03SteelInsertSystemF[i+1]->setBack(*D03UpperSystem[i],8);
    D03SteelInsertSystemF[i+1]->createAll(System,*D03UpperSystem[i],-1);

      iRemove=D03UpperSystem[i]->getCell("VoidF");
      System.removeCell(iRemove);
    D03SteelInsertSystemB[i+1]->setFront(*D03UpperSystem[i],7);
    D03SteelInsertSystemB[i+1]->setBack(*D03UpperSystem[i],-2);
    D03SteelInsertSystemB[i+1]->createAll(System,*D03UpperSystem[i],7);

     iRemove=D03UpperSystem[i]->getCell("VoidB");
      System.removeCell(iRemove);

    D03SteelSpacingSystem[i+1]->setBack(*D03UpperSystem[i],-7);
    D03SteelSpacingSystem[i+1]->setFront(*D03UpperSystem[i],-8);
    D03SteelSpacingSystem[i+1]->createAll(System,*D03UpperSystem[i],-8);
    iRemove=D03UpperSystem[i]->getCell("VoidGapC");
    System.removeCell(iRemove);}
/*    if(i<5){
     VPipeOutA->addInsertCell(D03SteelInsertSystemF[i]->getCell("Void"));
     if (i>0)     VPipeOutA->addInsertCell(D03SteelSpacingSystem[i]->getCell("Void"));
     VPipeOutA->addInsertCell(D03SteelInsertSystemB[i]->getCell("Void"));
     }*/

    
    
    if(i==0){  VPipeOutA->addInsertCell(D03SteelInsertSystemF[i]->getCell("Void"));
      // VPipeOutA->addInsertCell(D03SteelSpacingSystem[i]->getCell("Void"));
      // VPipeOutB->addInsertCell(D03SteelInsertSystemF[i]->getCell("Void"));
  VPipeOutB->addInsertCell(D03SteelInsertSystemB[i]->getCell("Void"));
    };
  
    if((i>0)&&(i<19)){
     VPipeOutB->addInsertCell(D03SteelInsertSystemF[i]->getCell("Void"));
     VPipeOutB->addInsertCell(D03SteelSpacingSystem[i]->getCell("Void"));
     VPipeOutB->addInsertCell(D03SteelInsertSystemB[i]->getCell("Void"));
    }
        if(i>18){
     VPipeOutC->addInsertCell(D03SteelInsertSystemF[i]->getCell("Void"));
     VPipeOutC->addInsertCell(D03SteelSpacingSystem[i]->getCell("Void"));
     VPipeOutC->addInsertCell(D03SteelInsertSystemB[i]->getCell("Void"));
     }
    
    }

       
    D03SteelInsertSystemF[19]->setFront(*D03UpperShutter[0],8);
    D03SteelInsertSystemF[19]->setBack(*D03UpperShutterStart,8);
    D03SteelInsertSystemF[19]->createAll(System,*D03UpperShutter[0],8);
      iRemove=D03UpperShutterStart->getCell("VoidF");
      System.removeCell(iRemove);
      iRemove=D03UpperShutter[0]->getCell("VoidF");
      System.removeCell(iRemove);
    
    
    
    D03SteelInsertSystemB[19]->setFront(*D03UpperShutterStart,7);
    D03SteelInsertSystemB[19]->setBack(*D03UpperShutterStart,-2);
    D03SteelInsertSystemB[19]->createAll(System,*D03UpperShutterStart,7);

     iRemove=D03UpperShutterStart->getCell("VoidB");
      System.removeCell(iRemove);

    D03SteelSpacingSystem[19]->setBack(*D03UpperShutterStart,-7);
    D03SteelSpacingSystem[19]->setFront(*D03UpperShutterStart,-8);
    D03SteelSpacingSystem[19]->createAll(System,*D03UpperShutterStart,-8);
    iRemove=D03UpperShutterStart->getCell("VoidGapC");
    System.removeCell(iRemove);
    //creating steel in the upper part of  shutter shielding
    // 1 and 4 are B4Clayers

     VPipeOutB->addInsertCell(D03SteelInsertSystemF[19]->getCell("Void"));
     VPipeOutB->addInsertCell(D03SteelSpacingSystem[19]->getCell("Void"));
     VPipeOutB->addInsertCell(D03SteelInsertSystemB[19]->getCell("Void"));
    
     
    std::shared_ptr<constructSystem::D03SteelSpacing>
               D03SteelShutterPhantom =
             std::shared_ptr<constructSystem::D03SteelSpacing>
(new constructSystem::D03SteelSpacing("bifrostD03SteelShutterPhantom"));
    OR.addObject ("bifrostD03SteelShutterPhantom",D03SteelShutterPhantom);
  
          D03SteelShutterPhantom->setFront(*D03UpperShutter[0],7);
          D03SteelShutterPhantom->setBack(*D03UpperShutter[1],7);
	  D03SteelShutterPhantom->addInsertCell(D03BaseShutter[0]->getCell("VoidB"));
	  D03SteelShutterPhantom->addInsertCell(D03BaseShutter[1]->getCell("VoidB"));
	  // D03SteelShutterPhantom->addInsertCell(D03BaseShutter[1]->getCell("VoidGapC"));
	  //  D03SteelShutterPhantom->addInsertCell(D03BaseShutter[0]->getCell("VoidGapC"));
	  D03SteelShutterPhantom->addInsertCell(D03UpperShutter[0]->getCell("VoidLowG"));
	  D03SteelShutterPhantom->addInsertCell(D03UpperShutter[1]->getCell("VoidLowG"));
	  D03SteelShutterPhantom->createAll(System, *D03UpperShutter[0],8);
	  

    std::shared_ptr<constructSystem::D03SteelInsert>
               D03SteelShutterExtra =
             std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostD03SteelShutterExtra"));
    OR.addObject ("bifrostD03SteelShutterExtra",D03SteelShutterExtra);

      
          D03SteelShutterExtra->setFront(*D03UpperShutter[0],7);
          D03SteelShutterExtra->setBack(*D03UpperShutter[1],7);

	  //	  D03SteelShutterExtra->addInsertCell(D03SteelShutterPhantom->getCells()[0]);
      
	  D03SteelShutterExtra->createAll(System, *D03UpperShutter[0],8);

	  System.removeCell( D03SteelShutterExtra->getCell("Void"));
	  iWalls=D03SteelShutterExtra->getCells("Roof");

    for (size_t i=0; i<iWalls.size(); i++){

	  System.removeCell( iWalls[i]);
    }



    
    std::shared_ptr<constructSystem::D03SteelInsert>
               D03SteelVoidShutterExtra =
             std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostD03SteelVoidShutterExtra"));
    OR.addObject ("bifrostD03SteelVoidShutterExtra",D03SteelVoidShutterExtra);

      
          D03SteelVoidShutterExtra->setFront(*D03UpperShutter[0],-8);
          D03SteelVoidShutterExtra->setBack(*D03UpperShutter[1],-8);

	  //	  D03SteelVoidShutterExtra->addInsertCell(D03SteelShutterPhantom->getCells()[0]);
      
	  D03SteelVoidShutterExtra->createAll(System, *D03UpperShutter[0],8);
	  /*
	  std::vector<int> iWalls1=D03SteelVoidShutterExtra->getCells("Roof");
	  */
	  
  iWalls=D03SteelVoidShutterExtra->getCells();


  int voidShutterFront, voidShutterBack;
	 
    for (size_t i=0; i<iWalls.size(); i++){
      /* int flag=0;
      for (size_t i1=0; i1<iWalls1.size(); i1++){
	if (iWalls[i]==iWalls1[i1]) flag=1;
      }
      if (flag==1) System.removeCell( iWalls[i]); */

      if (iWalls[i]==D03SteelVoidShutterExtra->getCells("Void")[0]) {
	
	  voidShutterFront= System.splitObject(iWalls[i],D03SteelShutterExtra->getLinkSurf(1));
	  voidShutterBack= System.splitObject(iWalls[i],D03SteelShutterExtra->getLinkSurf(2));
	  System.removeCell(iWalls[i]);
	}
     else
      System.removeCell(System.splitObject(
		      System.splitObject(iWalls[i],-D03SteelShutterExtra->getLinkSurf(2)),
		      -D03SteelShutterExtra->getLinkSurf(1)));
    }



 
  
   std::array<std::shared_ptr<constructSystem::D03BaseBlock>,5> D03BaseSteelShutter;
   for (size_t i=0; i<5; i++) {
     D03BaseSteelShutter[i] =
      std::shared_ptr<constructSystem::D03BaseBlock>
   (new constructSystem::D03BaseBlock("bifrostD03BaseSteelShutter"+std::to_string(i)));
  OR.addObject("bifrostD03BaseSteelShutter"+std::to_string(i),D03BaseSteelShutter[i]);
   }
     D03BaseSteelShutter[1]->setFront(*D03BaseShutter[0],7);
      D03BaseSteelShutter[1]->setBack(*D03BaseShutter[1],7);
      D03BaseSteelShutter[1]->addInsertCell(D03BaseShutter[0]->getCell("VoidLow"));
      D03BaseSteelShutter[1]->addInsertCell(D03BaseShutter[1]->getCell("VoidLow"));
    D03BaseSteelShutter[1]->createAll(System,*D03BaseShutter[0],-7);


    splitIndex=D03BaseSteelShutter[1]->getLinkSurf(6);
    System.removeCell(D03BaseShutter[0]->getCell("VoidF"));
       
    iRemove=System.splitObject(D03BaseShutter[0]->getCell("VoidB"),-splitIndex);
      System.removeCell(iRemove);
    
    iRemove=System.splitObject(D03BaseShutter[1]->getCell("VoidB"),-splitIndex);
      System.removeCell(iRemove);

     D03BaseSteelShutter[3]->setFront(*D03BaseShutter[0],8);
          D03BaseSteelShutter[3]->setBack(*D03BaseShutter[0],-1);
     //  D03BaseSteelShutter[0]->addInsertCell(D03BaseShutter[0]->getCell("VoidLow"));
    D03BaseSteelShutter[3]->createAll(System,*D03BaseShutter[0],-8);
     //    iRemove=System.splitObject(D03BaseSteelShutter[3]->getCell("VoidF"),-splitIndex);
      //  System.removeCell(iRemove);
    
   
     D03BaseSteelShutter[0]->setFront(*D03BaseShutter[0],8);
     //      D03BaseSteelShutter[1]->setBack(*D03BaseShutter[1],7);
      D03BaseSteelShutter[0]->addInsertCell(D03BaseShutter[0]->getCell("VoidLow"));
      // D03BaseSteelShutter[1]->addInsertCell(D03BaseShutter[1]->getCell("VoidLow"));
    D03BaseSteelShutter[0]->createAll(System,*D03BaseShutter[0],-8);


    /*
    iWalls=D03BaseSteelShutter[0]->getCells();

    for (size_t i=0; i<iWalls.size(); i++){
      //   System.removeCell(System.splitObject(iWalls[i],-splitIndex));
      System.splitObject(iWalls[i],-splitIndex);
    }
    */
        iRemove=System.splitObject(D03BaseSteelShutter[3]->getCell("VoidF"),
  -splitIndex);
				   
      iRemove=System.splitObject(iRemove,
			    -D03BaseSteelShutter[0]->getLinkSurf(2));

	System.removeCell(iRemove);
      
      System.removeCell(D03BaseSteelShutter[3]->getCell("VoidLow"));

  
      /*
  splitIndex=D03BaseSteelShutter[0]->getLinkSurf(6);
    
  iRemove=System.splitObject(D03BaseShutter[0]->getCell("VoidF"),-splitIndex);

  splitIndex=D03BaseSteelShutter[0]->getLinkSurf(2);
  iRemove=System.splitObject(iRemove,-splitIndex);
    System.removeCell(iRemove);
    */

    

      D03BaseSteelShutter[4]->setFront(*D03BaseShutter[1],8);
      D03BaseSteelShutter[4]->setBack(*D03BaseShutter[2],2);
     //      D03BaseSteelShutter[1]->setBack(*D03BaseShutter[1],7);
     // D03BaseSteelShutter[4]->addInsertCell(D03BaseShutter[1]->getCell("VoidLow"));
      // D03BaseSteelShutter[1]->addInsertCell(D03BaseShutter[1]->getCell("VoidLow"));

      iWalls=D03BaseShutter[1]->getCells("WallFrontR");     
    for (size_t i=0; i<iWalls.size(); i++){
      D03BaseSteelShutter[4]->addInsertCell(iWalls[i]);
  }
      iWalls=D03BaseShutter[1]->getCells("WallFrontL");
      for (size_t i=0; i<iWalls.size(); i++){
      D03BaseSteelShutter[4]->addInsertCell(iWalls[i]);
  }


      D03BaseSteelShutter[4]->createAll(System,*D03BaseShutter[1],-8);

    /*        System.removeCell(System.splitObject(D03BaseShutter[1]->getCell("VoidB"),
    			 D03BaseSteelShutter[4]->getLinkSurf(-2)));
     
    */
    
    System.removeCell(D03BaseSteelShutter[4]->getCell("VoidB"));
    System.removeCell(D03BaseSteelShutter[4]->getCell("VoidF"));
     	

    System.removeCell(D03BaseSteelShutter[4]->getCell("VoidLow"));

    

     D03BaseSteelShutter[2]->setFront(*D03BaseShutter[1],8);
     //      D03BaseSteelShutter[1]->setBack(*D03BaseShutter[1],7);
      D03BaseSteelShutter[2]->addInsertCell(D03BaseShutter[1]->getCell("VoidLow"));
      // D03BaseSteelShutter[1]->addInsertCell(D03BaseShutter[1]->getCell("VoidLow"));
    D03BaseSteelShutter[2]->createAll(System,*D03BaseShutter[1],-8);
  splitIndex=D03BaseSteelShutter[2]->getLinkSurf(6);
    
  iRemove=System.splitObject(D03BaseShutter[1]->getCell("VoidF"),-splitIndex);

  splitIndex=D03BaseSteelShutter[2]->getLinkSurf(2);
  iRemove=System.splitObject(iRemove,-splitIndex);
    System.removeCell(iRemove);

     
   
         
   std::vector<int> iVoid;
    System.removeCell(D03UpperShutter[0]->getCell("VoidB"));
    System.removeCell(System.splitObject(D03UpperShutter[0]->getCell("VoidGapC"),
					 -D03SteelVoidShutterExtra->getLinkSurf(5)));
	iVoid=D03UpperShutter[0]->getCells("VoidGapIF");
	for(int i=0;i<iVoid.size();i++)	System.removeCell(iVoid[i]);
        iVoid=D03UpperShutter[1]->getCells("VoidGapIF");
	for(int i=0;i<iVoid.size();i++)	System.removeCell(iVoid[i]);
   System.removeCell(D03UpperShutter[1]->getCell("VoidB"));
    System.removeCell(System.splitObject(D03UpperShutter[1]->getCell("VoidGapC"),
					 -D03SteelVoidShutterExtra->getLinkSurf(5)));
   
 
 
 
        std::array<std::shared_ptr<constructSystem::D03SteelInsert>,9>
               D03SteelShutter;
    for (int i=0;i<9;i++){
     D03SteelShutter[i]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostD03SteelShutter"+std::to_string(i)));
    OR.addObject ("bifrostD03SteelShutter"+std::to_string(i),
		  D03SteelShutter[i]);}

    
    D03SteelShutter[4]->setFront(*D03UpperShutter[0],7);

    D03SteelShutter[4]->setBack(*D03UpperShutter[1],7);

    D03SteelShutter[4]->addInsertCell(D03SteelShutterPhantom->getCell("Void"));
D03SteelShutter[4]->createAll(System,*D03UpperShutter[0],8);

 std::vector<int> iCells=D03BaseShutter[2]->getCells("WallBackR");
 for(size_t ii=0; ii<iCells.size();ii++){ System.findObject(iCells[ii])->setMaterial(145);
   System.setMaterialDensity(iCells[ii]);}


iCells=D03BaseShutter[2]->getCells("WallBackL");
 for(size_t ii=0; ii<iCells.size();ii++){ System.findObject(iCells[ii])->setMaterial(145);
   System.setMaterialDensity(iCells[ii]);}

 iCells=D03BaseShutter[2]->getCells("WallFrontR");
 for(size_t ii=0; ii<iCells.size();ii++){ System.findObject(iCells[ii])->setMaterial(145);
   System.setMaterialDensity(iCells[ii]);}



 
 int iCell;
 //=D03SteelVoidShutterExtra->getCell("Void");
   /*=System.splitObject(D03SteelShutterPhantom->getCell("Void"),
			      D03SteelShutter[4]->getLinkSurf(1));
 VPipeOutC->addInsertCell(iCell);
   */
  
 D03SteelShutter[2]->setFront(*D03UpperShutter[0],-8);
  //    D03SteelShutter[0]->addInsertCell(D03SteelShutterPhantom->getCell("Void"));
     D03SteelShutter[2]->addInsertCell(voidShutterFront);
     D03SteelShutter[2]->addInsertCell(D03SteelShutter[4]->getCell("Void"));
    D03SteelShutter[2]->createAll(System,*D03UpperShutter[0],8);
    // attachSystem::addToInsertSurfCtrl(System, *D03SteelVoidShutterExtra,*D03SteelShutter[2]);
				      
    
iCell=System.splitObject(D03SteelShutter[4]->getCell("Void"),
			     -D03SteelShutter[2]->getLinkSurf(2));

 VPipeOutC->addInsertCell(iCell);
 VPipeOutC->addInsertCell(voidShutterFront);

    
D03SteelShutter[3]->setFront(*D03SteelShutter[2],2);
D03SteelShutter[3]->addInsertCell(D03SteelShutter[4]->getCell("Void"));
D03SteelShutter[3]->createAll(System,*D03SteelShutter[2],2);
   

iCell=System.splitObject(D03SteelShutter[4]->getCell("Void"),
			     -D03SteelShutter[3]->getLinkSurf(2));

 VPipeOutC->addInsertCell(iCell);
 
 
    D03SteelShutter[6]->setFront(*D03UpperShutter[1],-8);
    
    D03SteelShutter[6]->addInsertCell(D03SteelShutter[4]->getCell("Void"));
    //    std::cout << voidShutterBack << std::endl;
    D03SteelShutter[6]->addInsertCell(voidShutterBack);
     
     D03SteelShutter[6]->addInsertCell(D03SteelShutterPhantom->getCell("Void"));
    D03SteelShutter[6]->createAll(System,*D03UpperShutter[1],8);

    // attachSystem::addToInsertSurfCtrl(System,*D03SteelVoidShutterExtra,*D03SteelShutter[6]);
    
     iCell=System.splitObject(D03SteelShutter[4]->getCell("Void"),
    			     -D03SteelShutter[6]->getLinkSurf(2));

     RecPipe[0]->addInsertCell(iCell);
     RecPipe[0]->addInsertCell(voidShutterBack);
     
    D03SteelShutter[5]->setFront(*D03SteelShutter[6],2);
    D03SteelShutter[5]->addInsertCell(D03SteelShutter[4]->getCell("Void"));
    D03SteelShutter[5]->createAll(System,*D03SteelShutter[6],2);
    
    iCell=System.splitObject(D03SteelShutter[4]->getCell("Void"),
    			     -D03SteelShutter[5]->getLinkSurf(2));
    RecPipe[0]->addInsertCell(iCell);
    

    D03SteelShutter[1]->createAll(System, *D03SteelInsertSystemF[19],1);
    System.removeCell(D03SteelShutter[1]->getCell("Void"));
    //    iRemove=D03UpperShutterStart->getCell("VoidGapC");
    
   
    //    std::cout << "Steel in shutter" << std::endl;


    attachSystem::addToInsertSurfCtrl(System,
				      *D03UpperShutter[0], *D03SteelShutter[1]);
    
    D03SteelShutter[7]->createAll(System, *D03UpperShutter[1],-8);
    System.removeCell(D03SteelShutter[7]->getCell("Void"));
    attachSystem::addToInsertSurfCtrl(System,
				      *D03UpperShutter[1], *D03SteelShutter[7]);

    D03SteelShutter[0]->setBack(*D03SteelInsertSystemF[19],-2);
    D03SteelShutter[0]->createAll(System, *D03SteelShutter[1],2);
    System.removeCell(D03SteelShutter[0]->getCell("Void")); 

    attachSystem::addToInsertSurfCtrl(System,
				      *D03UpperShutter[0], *D03SteelShutter[0]);
    attachSystem::addToInsertSurfCtrl(System,
				      *D03UpperShutterStart, *D03SteelShutter[0]);

      

    /*
   std::vector<int> iCells;
   iCells=D03UpperShutter[0]->getCells("WallFrontR");
   for(size_t i=0; i<iCells.size(); i++){
     std::cout << i << std::endl;
     try{  iRemove= System.splitObject(iCells[i],-D03SteelShutter[0]->getLinkSurf(2));
       System.removeCell(iRemove);} catch(...)
       {std::cout << "Exception  " << i << std::endl;};
   };
    */
   /*
      iCells=D03UpperShutter[0]->getCells("WallFrontL");
   for(size_t i=0; i<3; i++){
     try{  iRemove= System.splitObject(iCells[i],-D03SteelShutter[0]->getLinkSurf(2));
       System.removeCell(iRemove);} catch(...){};
   };

   iCells=D03UpperShutter[0]->getCells("WallFrontT");
   for(size_t i=0; i<3; i++){
     try{  iRemove= System.splitObject(iCells[i],-D03SteelShutter[0]->getLinkSurf(2));
       System.removeCell(iRemove);} catch(...){};
   };
   */
  
  
       
   
       
     
std::shared_ptr<constructSystem::D03UpperBlock> D03UpperShutterEnd =
  std::shared_ptr<constructSystem::D03UpperBlock>
		 (new constructSystem::D03UpperBlock("bifrostD03UpperShutterEnd"));
  OR.addObject ("bifrostD03UpperShutterEnd",D03UpperShutterEnd);
  D03UpperShutterEnd->addInsertCell(voidRear2);
  D03UpperShutterEnd->createAll(System,*D03UpperShutter[1],1);




    D03SteelShutter[8]->setBack(*D03UpperShutterEnd,7);
    D03SteelShutter[8]->createAll(System, *D03SteelShutter[7],2);
    System.removeCell(D03SteelShutter[8]->getCell("Void")); 

    attachSystem::addToInsertSurfCtrl(System,
				      *D03UpperShutter[1], *D03SteelShutter[8]);
    attachSystem::addToInsertSurfCtrl(System,
				      *D03UpperShutterEnd, *D03SteelShutter[8]);

    

  
   voidRear2=System.splitObject(voidRear2,D03UpperShutterEnd->getLinkSurf(1));     

    
   //Now optics  
   //    VPipeOutA->addInsertCell(ShieldGap1->getCell("Void"));
   //  VPipeOutA->addInsertCell(InnerShieldA->getCell("Void"));
  VPipeOutA->addInsertCell(D03SteelFirstB->getCell("Void"));
    VPipeOutA->addInsertCell(D03FirstUpper->getCell("VoidGapC"));
    System.splitObject(D03SteelFirstA->getCell("Void"), bunkerObj.getLinkSurf(-2));
    VPipeOutA->addInsertCell(D03SteelFirstA->getCell("Void"));

  VPipeOutB->addInsertCell(D03SecondUpper->getCell("VoidGapC"));
  VPipeOutA->createAll(System,FocusWall2->getKey("Guide0"),2);

  VPipeOutB->addInsertCell(System.splitObject(
			      D03SteelInsertSystemF[0]->getCell("Void"),
					      VPipeOutA->getLinkSurf(2)));

  
  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
    
    
  //     splitIndex= FocusOutA->getKey("Guide0").getLinkSurf(-2);
  /* splitIndex = VPipeOutA->getLinkSurf(2);
  int voidThis=InnerShieldA->getCell("Void");
    int voidNext =  System.splitObject(voidThis,splitIndex);
  */
 
  
  
  // Elliptic 25-3.6m section
    //  VPipeOutB->addInsertCell(ShieldA->getCell("Void"));
  //   VPipeOutB->addInsertCell(voidNext);
  // VPipeOutB->createAll(System,FocusOutA->getKey("Guide0"),2);

  VPipeOutB->setFront(*VPipeOutA,2);

  VPipeOutB->createAll(System,*VPipeOutA,2);

  VPipeOutC->addInsertCell(
	 System.splitObject(D03SteelInsertSystemF[19]->getCell("Void") ,
					    VPipeOutB->getLinkSurf(2)));

  FocusOutA->addInsertCell(VPipeOutB->getCells("Void"));
  //  FocusOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);
   FocusOutA->createAll(System,FocusWall2->getKey("Guide0"),2,
			 FocusWall2->getKey("Guide0"),2);
  
  FocusOutB->addInsertCell(VPipeOutB->getCells("Void"));
  //  FocusOutB->createAll(System,*VPipeOutB,0,*VPipeOutB,0);
  FocusOutB->createAll(System,FocusOutA->getKey("Guide0"),2,
		       FocusOutA->getKey("Guide0"),2);

  /*
  // Elliptic 25-3.6 m section
  VPipeOutC->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutC->createAll(System,FocusOutB->getKey("Guide0"),2);

  FocusOutC->addInsertCell(VPipeOutC->getCells("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0,*VPipeOutC,0);
  */
  
  // Elliptic 25-3.6 m section, last part
  // VPipeOutC->addInsertCell(ShieldA->getCell("Void"));
  //   VPipeOutC->addInsertCell(voidNext);
  //  VPipeOutC->addInsertCell(ShutterInnerShield->getCells("Void"));
  //  VPipeOutC->addInsertCell(ShutterFrontHorseshoe->getCells("Void"));

  /*   VPipeOutC->addInsertCell(D03SteelShutter[0]->getCell("Void"));
    VPipeOutC->addInsertCell(D03SteelShutter[1]->getCell("Void"));
   VPipeOutC->addInsertCell(D03SteelShutterPhantom->getCell("Void"));
  */
   VPipeOutC->addInsertCell(D03SteelShutter[4]->getCell("Void"));
   VPipeOutC->setFront(*VPipeOutB,2);
   // VPipeOutC->addInsertCell(D03SteelVoidShutterExtra->getCell("Void"));
   VPipeOutC->createAll(System,*VPipeOutB,2);
   //     attachSystem::addToInsertForced(System,*D03SteelVoidShutterExtra,*VPipeOutC);

   FocusOutC->addInsertCell(VPipeOutB->getCells("Void"));
   FocusOutC->addInsertCell(VPipeOutC->getCells("Void"));
   FocusOutC->createAll(System,FocusOutB->getKey("Guide0"),2,
			FocusOutB->getKey("Guide0"),2);

   splitIndex = VPipeOutC->getLinkSurf(2);
  int  voidThis=D03SteelShutter[4]->getCell("Void");
  int  voidShutter =  System.splitObject(voidThis,splitIndex);


   
   ShutterAFrame->addInsertCell(voidShutter);
   ShutterAFrame->addInsertCell(D03BaseShutter[0]->getCell("VoidB"));
   ShutterAFrame->addInsertCell(D03BaseShutter[1]->getCell("VoidB"));
   ShutterAFrame->addInsertCell(D03BaseSteelShutter[1]->getCell("VoidF"));
  
   ShutterAFrame->createAll(System,FocusOutC->getKey("Guide0"),2);

   ShutterARails->createAll(System,FocusOutC->getKey("Guide0"),2);
  

  // System.removeCell(ShutterAFrame->getCells("Void"));
attachSystem::addToInsertSurfCtrl(System,*ShutterAFrame,*ShutterARails);  

   
  ShutterAB4C->addInsertCell(voidShutter);
 
  
  ShutterAB4C->createAll(System,*ShutterAFrame,0);
  attachSystem::addToInsertSurfCtrl(System,*ShutterARails,*ShutterAB4C);  


  //  return 0;
  
  ShutterAPedestal->addInsertCell(D03BaseSteelShutter[1]->getCell("VoidF"));
  ShutterAPedestal->createAll(System,*D03BaseSteelShutter[1],-8);

  //Beyond shutter to chopper

    

  std::array<std::shared_ptr<constructSystem::E02BaseBlock>,26> E02Base;
  //Making first block match opening for the pillars in E02
  Control.addVariable("bifrostE02Base0LengthF",835.5-44-200);
  //Making block no4 fit the chopper pit
	 Control.addVariable("bifrostE02Base4LengthF",89.5-4.5+5);
	 Control.addVariable("bifrostE02Base4LengthB",10-4.5);
	 Control.addVariable("bifrostE02Base4Gap",380-5);//211+80+80);
	 Control.addVariable("bifrostE02Base5LengthF",103);
	 Control.addVariable("bifrostE02Base22LengthB",460);
	 Control.addVariable("bifrostE02Base23LengthF",200);
         Control.addVariable("bifrostE02Base23LengthB",200);
	 Control.addVariable("bifrostE02Base23Height",115);
	 Control.addVariable("bifrostE02Base23Gap",1);
	 Control.addVariable("bifrostE02Base23Width1Front",230);
	 Control.addVariable("bifrostE02Base23Width1Back",230);
	 Control.addVariable("bifrostE02Base23Width3Front",150);
	 Control.addVariable("bifrostE02Base23Width3Back",150);
	 Control.addVariable("bifrostE02Base23YStep",260.2);
	 Control.addVariable("bifrostE02Base23HeightUp",115);

      	 Control.addVariable("bifrostE02Base25LengthF",100);
         Control.addVariable("bifrostE02Base25LengthB",200);
	 Control.addVariable("bifrostE02Base25Height",115);
	 Control.addVariable("bifrostE02Base24Gap",1);
	 Control.addVariable("bifrostE02Base25Gap",1);
	 Control.addVariable("bifrostE02Base25Width1Front",230);
	 Control.addVariable("bifrostE02Base25Width1Back",230);
	 Control.addVariable("bifrostE02Base25Width3Front",150);
	 Control.addVariable("bifrostE02Base25Width3Back",150);
	 //	 Control.addVariable("bifrostE02Base25YStep",250);
	 Control.addVariable("bifrostE02Base25HeightUp",115);

	 for (int i=0; i< 26; i++){
         E02Base[i]=std::shared_ptr<constructSystem::E02BaseBlock>
	   (new constructSystem::E02BaseBlock("bifrostE02Base"+std::to_string(i)));
	 OR.addObject ("bifrostE02Base"+std::to_string(i),E02Base[i]);
 }
	 
      
  //       E02Base[0]->addInsertCell(voidRear1);
    E02Base[0]->createAll(System,*D03BaseShutter[2],1);
    System.removeCell(voidRearShutter);

   
    for (int i=1; i<23; i++){
      //        if(i>20) Control.addVariable("LayerFlag",1);
 //    E02Base[i]->addInsertCell(voidRear1new);
        E02Base[i]->createAll(System,*E02Base[i-1],2);
  }
    //	Control.addVariable("LayerFlag",0);
    int voidE01 = System.splitObject(E02Void,E02Base[22]->getLinkSurf(2));
    E02Floor = System.splitObject(E02Void, E02Base[0]->getLinkSurf(5));



      System.findObject(E02Floor)->setMaterial(144);
      System.setMaterialDensity(E02Floor);


    
    int voidE02BaseL=System.splitObject(E02Void, E02Base[0]->getLinkSurf(3));
    
    int voidE02BaseR=System.splitObject(E02Void, E02Base[0]->getLinkSurf(4));
   
    int voidE02Base=System.splitObject(E02Void, -E02Base[0]->getLinkSurf(6));
    // iRemove=System.splitObject(voidE02Base, E02Base[24]->getLinkSurf(2));
 System.removeCell(voidE02Base);

 
 
    std::array< std::shared_ptr<constructSystem::E02PillarCover>,23> E02PillarCoverL;
    std::array<std::shared_ptr<constructSystem::E02PillarCover>,23> E02PillarCoverR;
    
    Control.addVariable("bifrostE02PillarCoverL22Right",138-59);
    Control.addVariable("bifrostE02PillarCoverR22Left",138-59);
    /*Control.addVariable("bifrostE02PillarCoverL22VoidLeft",211*0.5);
    Control.addVariable("bifrostE02PillarCoverR22VoidLeft",211*0.5);
    Control.addVariable("bifrostE02PillarCoverL22Right",380*0.5);
    Control.addVariable("bifrostE02PillarCoverR22Right",380*0.5);
    Control.addVariable("bifrostE02PillarCoverL22VoidRight",211*0.5);
    Control.addVariable("bifrostE02PillarCoverR22VoidRight",211*0.5);
    */
    
    Control.addVariable("bifrostE02PillarCoverL22Height",115);
    Control.addVariable("bifrostE02PillarCoverR22Height",115);
    Control.addVariable("bifrostE02PillarCoverL22VoidLength",15);
        Control.addVariable("bifrostE02PillarCoverL22VoidMat","LimestoneConcrete");
    Control.addVariable("bifrostE02PillarCoverL22Length",57);
    Control.addVariable("bifrostE02PillarCoverR22VoidLength",22);
    Control.addVariable("bifrostE02PillarCoverR22Length",57);

    std::vector<int> voidChopperR;   std::vector<int> voidChopperL;
    for (int i=0; i<23; i++){
      //     if(i>20) Control.addVariable("LayerFlag",1);
      //          std::cout << i << std::endl;
     E02PillarCoverL[i]=std::shared_ptr<constructSystem::E02PillarCover>
       (new constructSystem::E02PillarCover("bifrostE02PillarCoverL"+
					    std::to_string(i)));
     	 OR.addObject ("bifrostE02PillarCoverL"+std::to_string(i),
		       E02PillarCoverL[i]);
      E02PillarCoverL[i]->addInsertCell(voidE02BaseL);
      voidChopperL.push_back(voidE02BaseL);
      if (i==4)  E02PillarCoverL[i]->addInsertCell(E02Base[4]->getCell("VoidLow"));
     E02PillarCoverL[i]->createAll(System,*E02Base[i],3);
     if (i<22)  voidE02BaseL=
     	System.splitObject(voidE02BaseL, E02PillarCoverL[i]->getLinkSurf(4) );
     
       // std::cout << "left" << std::endl;
     E02PillarCoverR[i]=std::shared_ptr<constructSystem::E02PillarCover>
	(new constructSystem::E02PillarCover("bifrostE02PillarCoverR"+
					     std::to_string(i)));
     	 OR.addObject ("bifrostE02PillarCoverR"+std::to_string(i),
		       E02PillarCoverR[i]);
      E02PillarCoverR[i]->addInsertCell(voidE02BaseR);
      voidChopperR.push_back(voidE02BaseR);
      //   std::cout << "aa" << std::endl; 
      if (i==4)  E02PillarCoverR[i]->addInsertCell(E02Base[4]->getCell("VoidLow"));
       E02PillarCoverR[i]->createAll(System,*E02Base[i],4);
       // std::cout << "aa" << std::endl; 
       if (i<22)  voidE02BaseR=
     	System.splitObject(voidE02BaseR, E02PillarCoverR[i]->getLinkSurf(3) );
       //  std::cout << "right" << std::endl;
    }

    System.removeCell(E02Base[4]->getCell("VoidGapL"));
    System.removeCell(E02Base[4]->getCell("VoidGapR"));
    E02Base[23]->addInsertCell(voidE02BaseR);
    E02Base[23]->addInsertCell(voidE02BaseL);
    //E02Base[23]->setFront(*E02PillarCoverR[22],3);
    E02Base[23]->setBack(*E02Base[22],-2);
     E02Base[23]->createAll(System,*E02Base[22],-1); 
    System.removeCell(E02Base[23]->getCell("VoidF"));
    System.removeCell(E02Base[23]->getCell("VoidB"));
    System.removeCell(E02Base[23]->getCell("VoidGapC"));

    attachSystem::addToInsertSurfCtrl(System, E02Base[23]->getCell("VoidLow"),*E02Base[22]);
    std::vector<int>Base4TopCells = E02PillarCoverL[4]->getCells("WallT");
    for (size_t i=0; i<Base4TopCells.size();i++){
      System.removeCell(Base4TopCells[i]);}
    //Steel in the base of chopper pit

  
    /*
    //Chicanes in teh chopper pit   
    std::array< std::shared_ptr<beamlineSystem::GuideLine>,5> CChicane;
    for (int i=0; i<5;i++){
      CChicane[i]=std::shared_ptr<beamlineSystem::GuideLine>
	  (new beamlineSystem::GuideLine("bifrostCChicane"+std::to_string(i)));
		   OR.addObject("bifrostCChicane"+std::to_string(i),CChicane[i]);
		   //    E02Base[4]->setBack(*E02UpperSystem[25],-1);
		   }
    
    std::array< std::shared_ptr<beamlineSystem::GuideLine>,5> PhantCChicane;
    for (int i=0; i<5;i++){
      PhantCChicane[i]=std::shared_ptr<beamlineSystem::GuideLine>
	  (new beamlineSystem::GuideLine("PhantombifrostCChicane"+std::to_string(i)));
		   OR.addObject("PhantombifrostCChicane"+std::to_string(i),PhantCChicane[i]);
		   //    E02Base[4]->setBack(*E02UpperSystem[25],-1);
		   }

    PhantCChicane[1]->createAll(System,*E02PillarCoverL[4],-2,*E02PillarCoverL[4],-2);
    // CChicane[2]->setFront(CChicane[1]->getKey("Guide0"),1);
    CChicane[2]->createAll(System,PhantCChicane[1]->getKey("Guide0"),2,
			   PhantCChicane[1]->getKey("Guide0"),2);
    CChicane[1]->setBack(CChicane[2]->getKey("Guide0"),1);
    CChicane[1]->createAll(System,*E02PillarCoverL[4],-2,*E02PillarCoverL[4],-2);

    std::vector<int> iRemoveCells=PhantCChicane[1]->getCells();
    for (size_t i=0; i<iRemoveCells.size();i++){System.removeCell(iRemoveCells[i]);}
    //     PhantCChicane[3]->setFront(CChicane[2]->getKey("Guide0"),2);
    PhantCChicane[3]->createAll(System,CChicane[2]->getKey("Guide0"),2,
			   CChicane[2]->getKey("Guide0"),2);
    //CChicane[4]->setFront(CChicane[3]->getKey("Guide0"),2);
    CChicane[4]->createAll(System,PhantCChicane[3]->getKey("Guide0"),2,
			   PhantCChicane[3]->getKey("Guide0"),2);

    CChicane[3]->setBack(CChicane[4]->getKey("Guide0"),1);
    CChicane[3]->createAll(System,CChicane[2]->getKey("Guide0"),2,
			   CChicane[2]->getKey("Guide0"),2);

  iRemoveCells=PhantCChicane[3]->getCells();
    for (size_t i=0; i<iRemoveCells.size();i++){System.removeCell(iRemoveCells[i]);}
        */

    
    std::array<std::shared_ptr<constructSystem::D03UpperBlock>,102> E02UpperSystem;
    ireverse=0;
           Control.addVariable("bifrostE02Upper19Length1Mid",57.5-5);
    	        Control.addVariable("bifrostE02Upper19Length1Edge",42.5-5);
      
           Control.addVariable("bifrostE02Upper20Length1Edge",42.5+5);
       Control.addVariable("bifrostE02Upper20Length1Mid",57.5+5);
      

    Control.addVariable("bifrostE02Upper19YStep",100-5);
     Control.addVariable("bifrostE02Upper20HeightUpBack",225);
       Control.addVariable("bifrostE02Upper20Length2Edge",60);
       Control.addVariable("bifrostE02Upper20Width1Back",340);
       Control.addVariable("bifrostE02Upper20NSegB",4);
	 Control.addVariable("bifrostE02Upper92HeightLowBack",35);
	 Control.addVariable("bifrostE02Upper92Width3Back",45);
	 for (int i=93; i<102; i++){
	   Control.addVariable("bifrostE02Upper"+std::to_string(i)+"HeightLowBack",35);
	   Control.addVariable("bifrostE02Upper"+std::to_string(i)+"Width3Back",45);
	   Control.addVariable("bifrostE02Upper"+std::to_string(i)+"HeightLowFront",35);
	   Control.addVariable("bifrostE02Upper"+std::to_string(i)+"Width3Front",45);}
	 // Control.addVariable("bifrostE02Upper92HeightUpBack",135);
	 // Control.addVariable("bifrostE02Upper92NSegB",2);
		 Control.addVariable("bifrostE02Upper92NLayers",6);

       for (int i=93;i<102;i++){
	 Control.addVariable("bifrostE02Upper"+std::to_string(i)+"NLayers",5);
	 Control.addVariable("bifrostE02Upper"+std::to_string(i)+"HeightLowBack",35);
    // Control.addVariable("bifrostE02Upper"+std::to_string(i)+"HeightUpBack",135);
      	 Control.addVariable("bifrostE02Upper"+std::to_string(i)+"HeightLowFront",35);
      // Control.addVariable("bifrostE02Upper"+std::to_string(i)+"HeightUpFront",135);
       }
       
       // Control.addVariable("bifrostE02Upper20NSegB",3);
       //  Control.addVariable("bifrostE02Upper20Width3Back",220);

	/*
    Control.addVariable("bifrostE02Upper22HeightUpFront",225);
       Control.addVariable("bifrostE02Upper22Length2Edge",60);
       Control.addVariable("bifrostE02Upper22Width1Front",340);
        Control.addVariable("bifrostE02Upper22Width3Front",220);
    Control.addVariable("bifrostE02Upper22HeightUpBack",225);
       Control.addVariable("bifrostE02Upper22Length2Edge",60);
       Control.addVariable("bifrostE02Upper22Width1Back",340);
        Control.addVariable("bifrostE02Upper22Width3Back",220);

    Control.addVariable("bifrostE02Upper23HeightUpFront",225);
       Control.addVariable("bifrostE02Upper23Length1Edge",60);
       Control.addVariable("bifrostE02Upper23Width1Front",340);
        Control.addVariable("bifrostE02Upper23Width3Front",223);
	*/
	
    Control.addVariable("bifrostE02Upper2Length1Edge",529.5);
    Control.addVariable("bifrostE02Upper2Length1Mid",544.5);
    for (int i=0;i<20;i++){
      //	  if (i>86)     Control.addVariable("LayerFlag",1);
      //	  else Control.addVariable("LayerFlag",0);
	if(ireverse){
      	Control.addVariable("bifrostE02Upper"+std::to_string(i)+"XYAngle",180);
      if (i!=19)Control.addVariable("bifrostE02Upper"+std::to_string(i)+"YStep",100);}
     E02UpperSystem[i]=std::shared_ptr<constructSystem::D03UpperBlock>
	(new constructSystem::D03UpperBlock("bifrostE02Upper"+std::to_string(i)));
    OR.addObject ("bifrostE02Upper"+std::to_string(i),E02UpperSystem[i]);
    
    if (i==0) {E02UpperSystem[i]->addInsertCell(voidRear2);
      E02UpperSystem[i]->createAll(System,*D03UpperShutterEnd,1);}
    else { if (i<3) E02UpperSystem[i]->addInsertCell(voidRear2);
	E02UpperSystem[i]->createAll(System,*E02UpperSystem[i-1],ireverse+1);}
    //    std::cout<<ireverse<<std::endl;
    //   }
      ireverse=abs(ireverse-1);
    }

    

        int voidRight3,voidLeft3,voidTop3,E02VoidChopper;



      splitIndex = E02UpperSystem[19]->getLinkSurf(1);
      E02VoidChopper =  System.splitObject(E02Void,splitIndex);

      
    splitIndex =  E02UpperSystem[17]->getLinkSurf(6);
      voidTop3 =  System.splitObject(E02Void,splitIndex);
     
    splitIndex =  E02UpperSystem[17]->getLinkSurf(4);
    voidRight3 =  System.splitObject(E02Void,splitIndex);

      splitIndex =  E02UpperSystem[17]->getLinkSurf(3);
      voidLeft3 =  System.splitObject(E02Void,splitIndex);
      
     System.removeCell(E02Void);


    Control.addVariable("bifrostE02Upper21HeightUpFront",225);
    Control.addVariable("bifrostE02Upper21HeightMid",190);
    Control.addVariable("bifrostE02Upper21HeightLowFront",165);
       Control.addVariable("bifrostE02Upper21Length1Edge",40);
       Control.addVariable("bifrostE02Upper21Length2Edge",57.5);
       Control.addVariable("bifrostE02Upper21Length1Mid",57.5);
       Control.addVariable("bifrostE02Upper21Length2Mid",42.5);
       Control.addVariable("bifrostE02Upper21Width1Front",340);
        Control.addVariable("bifrostE02Upper21Width3Front",220);
         Control.addVariable("bifrostE02Upper21Width2",280);
    Control.addVariable("bifrostE02Upper21HeightUpBack",225);
    Control.addVariable("bifrostE02Upper21HeightLowBack",165);
       Control.addVariable("bifrostE02Upper21Width1Back",340);
        Control.addVariable("bifrostE02Upper21Width3Back",220);

    Control.addVariable("bifrostE02Upper22HeightUpFront",225);
    Control.addVariable("bifrostE02Upper22HeightMid",190);
    Control.addVariable("bifrostE02Upper22HeightLowFront",165);
       Control.addVariable("bifrostE02Upper22Length1Edge",40);
       Control.addVariable("bifrostE02Upper22Length2Edge",57.5);
       Control.addVariable("bifrostE02Upper22Length1Mid",57.5);
       Control.addVariable("bifrostE02Upper22Length2Mid",42.5);
       Control.addVariable("bifrostE02Upper22Width1Front",340);
        Control.addVariable("bifrostE02Upper22Width3Front",220);
         Control.addVariable("bifrostE02Upper22Width2",280);
    Control.addVariable("bifrostE02Upper22HeightUpBack",225);
    Control.addVariable("bifrostE02Upper22HeightLowBack",165);
       Control.addVariable("bifrostE02Upper22Width1Back",340);
        Control.addVariable("bifrostE02Upper22Width3Back",220);
    Control.addVariable("bifrostE02Upper23HeightUpBack",225);
       Control.addVariable("bifrostE02Upper23Length2Edge",60);
       Control.addVariable("bifrostE02Upper23Width1Back",340);
       Control.addVariable("bifrostE02Upper23NSegB",4);




       /*       Control.addVariable("bifrostE02Upper101Length1Edge",8.8);
       Control.addVariable("bifrostE02Upper101Length1Mid",23.8);
       */

       Control.addVariable("bifrostE02Upper101Length1Edge",3.8);
       Control.addVariable("bifrostE02Upper101Length1Mid",18.8);

       Control.addVariable("bifrostE02Upper101Length2Edge",62.5);
       Control.addVariable("bifrostE02Upper101Gap",2.2);
       
       //      Control.addVariable("bifrostE02Upper101Length1Mid",23.8);

       
	for (int i=20;i<102;i++){
	  //	  if (i>86)     Control.addVariable("LayerFlag",1);
	  //	  else Control.addVariable("LayerFlag",0);
  
	  if((ireverse)&&(i!=21)||(i==22)){
   Control.addVariable("bifrostE02Upper"+std::to_string(i)+"XYAngle",180);
if(i==22) Control.addVariable("bifrostE02Upper"+std::to_string(i)+"YStep",97.5);
 else	Control.addVariable("bifrostE02Upper"+std::to_string(i)+"YStep",100);}
  if (i==23) 	Control.addVariable("bifrostE02Upper"+std::to_string(i)+"YStep",102.5);
  if (i==101) 	Control.addVariable("bifrostE02Upper"+std::to_string(i)+"YStep",100-41.5+7.8);
       E02UpperSystem[i]=std::shared_ptr<constructSystem::D03UpperBlock>
	(new constructSystem::D03UpperBlock("bifrostE02Upper"+std::to_string(i)));
    OR.addObject ("bifrostE02Upper"+std::to_string(i),E02UpperSystem[i]);
    if (i<24) E02UpperSystem[i]->addInsertCell(E02VoidChopper);
    if ((i==22)||(i==23))
      {  ireverse=abs(ireverse-1);
	E02UpperSystem[i]->createAll(System,*E02UpperSystem[i-1],ireverse+1);
     }else{  E02UpperSystem[i]->createAll(System,*E02UpperSystem[i-1],ireverse+1);
      ireverse=abs(ireverse-1);} 
	}

	
      splitIndex = E02UpperSystem[23]->getLinkSurf(1);
      int E02VoidLast =  System.splitObject(E02VoidChopper,splitIndex);

      int voidTmp;
splitIndex=E02UpperSystem[20]->getLinkSurf(1);
 voidTmp      =System.splitObject(voidChopperR[4],-splitIndex);
  splitIndex=E02UpperSystem[20]->getLinkSurf(4);
 voidTmp      =System.splitObject(voidTmp,-splitIndex);
splitIndex=E02UpperSystem[20]->getLinkSurf(6);
 voidTmp      =System.splitObject(voidTmp,-splitIndex);
  System.removeCell(voidTmp);
 
splitIndex=E02UpperSystem[20]->getLinkSurf(1);
 voidTmp      =System.splitObject(voidChopperL[4],-splitIndex);
  splitIndex=E02UpperSystem[20]->getLinkSurf(3);
 voidTmp      =System.splitObject(voidTmp,-splitIndex);
splitIndex=E02UpperSystem[20]->getLinkSurf(6);
 voidTmp      =System.splitObject(voidTmp,-splitIndex);
  System.removeCell(voidTmp);

  
splitIndex=E02UpperSystem[23]->getLinkSurf(1);
 voidTmp      =System.splitObject(voidChopperL[5],-splitIndex);
   splitIndex=E02UpperSystem[23]->getLinkSurf(4);
 voidTmp      =System.splitObject(voidTmp,-splitIndex);
 splitIndex=E02UpperSystem[22]->getLinkSurf(6);
 voidTmp      =System.splitObject(voidTmp,-splitIndex);
 splitIndex=E02UpperSystem[23]->getLinkSurf(5);
 voidTmp      =System.splitObject(voidTmp,-splitIndex);
  System.removeCell(voidTmp);
   
splitIndex=E02UpperSystem[23]->getLinkSurf(1);
 voidTmp      =System.splitObject(voidChopperR[5],-splitIndex);
  splitIndex=E02UpperSystem[23]->getLinkSurf(3);
 voidTmp      =System.splitObject(voidTmp,-splitIndex);
splitIndex=E02UpperSystem[23]->getLinkSurf(6);
 voidTmp      =System.splitObject(voidTmp,-splitIndex);
splitIndex=E02UpperSystem[23]->getLinkSurf(5);
 voidTmp      =System.splitObject(voidTmp,-splitIndex);
  System.removeCell(voidTmp);

  
  
      splitIndex =  E02UpperSystem[92]->getLinkSurf(9);
   
   
      int E02VoidFinal =  System.splitObject(E02VoidLast,splitIndex);
	
    splitIndex =  E02UpperSystem[91]->getLinkSurf(6);
      voidTop3 =  System.splitObject(E02VoidLast,splitIndex);
     
    splitIndex =  E02UpperSystem[91]->getLinkSurf(4);
    voidRight3 =  System.splitObject(E02VoidLast,splitIndex);

      splitIndex =  E02UpperSystem[91]->getLinkSurf(3);
      voidLeft3 =  System.splitObject(E02VoidLast,splitIndex);
      
          System.removeCell(E02VoidLast);

      
	  // Now extra plates for chopper shielding

	  std::shared_ptr<constructSystem::InnerShield> ChopperL=
	    std::shared_ptr<constructSystem::InnerShield> (new constructSystem::InnerShield("ChopperBlockL"));
    OR.addObject ("ChopperBlockL", ChopperL);

	  std::shared_ptr<constructSystem::InnerShield> ChopperR=
	    std::shared_ptr<constructSystem::InnerShield> (new constructSystem::InnerShield("ChopperBlockR"));
    OR.addObject ("ChopperBlockR", ChopperR);
    Control.addVariable("ChopperBlockLDefMat","SteelS355");
    Control.addVariable("ChopperBlockLVoidMat","SteelS355");
Control.addVariable("ChopperBlockLLength",57-15+5.1);
Control.addVariable("ChopperBlockLLeft",47);
Control.addVariable("ChopperBlockLRight",47);
Control.addVariable("ChopperBlockLHeight",20);
Control.addVariable("ChopperBlockLDepth",0.0);
Control.addVariable("ChopperBlockLSpacing",0.0);
Control.addVariable("ChopperBlockLXStep",122);
Control.addVariable("ChopperBlockLZStep",0.5);
Control.addVariable("ChopperBlockLNSeg",3);
  
Control.addVariable("ChopperBlockLVoidLeft",15);
Control.addVariable("ChopperBlockLVoidRight",15);
Control.addVariable("ChopperBlockLVoidHeight",5);
Control.addVariable("ChopperBlockLNRoofLayers",3);
Control.addVariable("ChopperBlockLNWallLayersL",2);
Control.addVariable("ChopperBlockLNWallLayersR",2);
Control.addVariable("ChopperBlockLNFloorLayers",1);
Control.addVariable("ChopperBlockLVoidDepth",0);
Control.addVariable("ChopperBlockLSplitVoid",1);


//  attachSystem::addToInsertForced(System,
 ChopperL->addInsertCell(E02UpperSystem[20]->getCells("VoidOuter")[1]);
 ChopperL->createAll(System,*E02UpperSystem[20],-1);
 


    Control.addVariable("ChopperBlockRDefMat","LimestoneConcrete");
    Control.addVariable("ChopperBlockRVoidMat","LimestoneConcrete");
Control.addVariable("ChopperBlockRLength",57-15+5.1);
Control.addVariable("ChopperBlockRLeft",47);
Control.addVariable("ChopperBlockRRight",47);
Control.addVariable("ChopperBlockRHeight",40);
Control.addVariable("ChopperBlockRDepth",0);
Control.addVariable("ChopperBlockRSpacing",0.0);
Control.addVariable("ChopperBlockRXStep",-122);
Control.addVariable("ChopperBlockRZStep",0.5);
Control.addVariable("ChopperBlockRNSeg",3);
Control.addVariable("ChopperBlockRSplitVoid",1);
  
Control.addVariable("ChopperBlockRVoidLeft",15);
Control.addVariable("ChopperBlockRVoidRight",15);
Control.addVariable("ChopperBlockRVoidHeight",10);
Control.addVariable("ChopperBlockRNRoofLayers",3);
Control.addVariable("ChopperBlockRNFloorLayers",1);
Control.addVariable("ChopperBlockRNWallLayersL",2);
Control.addVariable("ChopperBlockRNWallLayersR",2);
Control.addVariable("ChopperBlockRVoidDepth",0);
 ChopperR->addInsertCell(E02UpperSystem[20]->getCells("VoidOuter")[0]);
 ChopperR->createAll(System,*E02UpperSystem[20],-1);



 

 
//int voidRear1new=System.splitObject(voidRear1,-E02UpperSystem[0]->getLinkSurf(1));
  

    std::array<std::shared_ptr<constructSystem::D03SteelInsert>,103>
               E02SteelInsertSystemF;

    std::array<std::shared_ptr<constructSystem::D03SteelInsert>,103>
               E02SteelInsertSystemB;

    std::array<std::shared_ptr<constructSystem::D03SteelSpacing>,103>
               E02SteelSpacingSystem;

    std::shared_ptr<constructSystem::D03SteelInsert>  E02SteelChopperBack=
      std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02SteelSystemChopperBack"));
    OR.addObject ("bifrostE02SteelSystemChopperBack", E02SteelChopperBack);


    E02SteelChopperBack->setFront(*E02UpperSystem[22],-1);
    E02SteelChopperBack->createAll(System,*E02UpperSystem[22],-1);
      System.removeCell(E02SteelChopperBack->getCell("Void"));

      std::shared_ptr<constructSystem::D03SteelInsert>  E02SteelChopperFront=
             std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02SteelSystemChopperFront"));
    OR.addObject ("bifrostE02SteelSystemChopperFront", E02SteelChopperFront);
    E02SteelChopperFront->setFront(*E02UpperSystem[21],-1);
    E02SteelChopperFront->createAll(System,*E02UpperSystem[21],-1);
    System.removeCell(E02SteelChopperFront->getCell("Void"));


    
    
std::shared_ptr<constructSystem::D03BaseBlock> E02BaseSteelChopper;

 E02BaseSteelChopper =
      std::shared_ptr<constructSystem::D03BaseBlock>
   (new constructSystem::D03BaseBlock("bifrostE02BaseSteelChopper"));
  OR.addObject("bifrostE02BaseSteelChopper",E02BaseSteelChopper);
  
  E02BaseSteelChopper->addInsertCell(E02PillarCoverL[4]->getCells("WallBack"));
  E02BaseSteelChopper->addInsertCell(E02PillarCoverR[4]->getCells("WallBack"));
   E02BaseSteelChopper->addInsertCell(E02PillarCoverL[4]->getCells("Void"));
   E02BaseSteelChopper->addInsertCell(E02PillarCoverR[4]->getCells("Void"));

   E02BaseSteelChopper->addInsertCell(E02Base[4]->getCells("VoidGapC"));

  E02BaseSteelChopper->setFront(*E02SteelChopperBack,2);
  E02BaseSteelChopper->setBack(*E02SteelChopperFront,2);
   E02BaseSteelChopper->createAll(System,*E02Base[4],-2);
   //   System.removeCell(E02BaseSteelChopper->getCell("VoidF"));
   //  System.removeCell(E02BaseSteelChopper->getCell("VoidB"));
  System.removeCell(E02BaseSteelChopper->getCell("VoidLow"));

  System.removeCell(E02UpperSystem[21]->getCell("VoidB"));
  System.removeCell(E02UpperSystem[21]->getCell("VoidF"));
  //   System.removeCell(E02UpperSystem[21]->getCell("VoidGapC"));
  //  System.removeCell(E02UpperSystem[22]->getCell("VoidGapC"));
  System.removeCell(E02UpperSystem[22]->getCell("VoidB"));
  System.removeCell(E02UpperSystem[22]->getCell("VoidF"));



 Control.addVariable("bifrostE02SteelSystemB0InnerB4CLayerThick",1.5);
Control.addVariable("bifrostE02SteelSystemB0B4CTileMat","B4CConcrete");



    
    
     E02SteelInsertSystemF[0]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02SteelSystemF"+std::to_string(0)));
    OR.addObject ("bifrostE02SteelSystemF"+std::to_string(0),
		  E02SteelInsertSystemF[0]);
     E02SteelInsertSystemB[0]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02SteelSystemB"+std::to_string(0)));
    OR.addObject ("bifrostE02SteelSystemB"+std::to_string(0),
		  E02SteelInsertSystemB[0]);
     E02SteelSpacingSystem[0]=
       std::shared_ptr<constructSystem::D03SteelSpacing>
(new constructSystem::D03SteelSpacing("bifrostE02SpacingSystemB"+std::to_string(0)));
    OR.addObject ("bifrostE02SpacingSystemB"+std::to_string(0),
		  E02SteelSpacingSystem[0]);

    

    
           E02SteelInsertSystemB[0]->setFront(*D03SteelShutter[6],1);
	    E02SteelInsertSystemB[0]->setBack(*D03UpperShutterEnd,7);
    //    E02SteelInsertSystemF[0]->addInsertCell(E02FirstBase->getCell("VoidF"));
    // E02SteelInsertSystemF[0]->addInsertCell(E02FirstBase->getCell("VoidGapC"));
    E02SteelInsertSystemB[0]->createAll(System,*D03SteelShutter[6],1);

     iRemove=D03UpperShutter[1]->getCell("VoidF");
      System.removeCell(iRemove);

      iRemove=D03UpperShutterEnd->getCell("VoidB");
      System.removeCell(iRemove);

   E02SteelSpacingSystem[0]->setFront(*D03UpperShutterEnd,-7);
    E02SteelSpacingSystem[0]->setBack(*D03UpperShutterEnd,-8);
    E02SteelSpacingSystem[0]->createAll(System,*D03UpperShutterEnd,-7);

          
        E02SteelInsertSystemF[0]->setFront(*D03UpperShutterEnd,8);
      E02SteelInsertSystemF[0]->setBack(*D03UpperShutterEnd,-1);
    E02SteelInsertSystemF[0]->createAll(System,*D03UpperShutterEnd,-8);

     iRemove=D03UpperShutterEnd->getCell("VoidF");
    System.removeCell(iRemove);
    
     iRemove=D03UpperShutterEnd->getCell("VoidGapC");
    System.removeCell(iRemove);
     
      
    
    /*
    E02SteelSpacingSystem[0]->setBack(*E02SecondUpper,-7);
    E02SteelSpacingSystem[0]->setFront(*E02SecondUpper,-8);
    E02SteelSpacingSystem[0]->createAll(System,*E02SecondUpper,-8);
    iRemove=E02SecondUpper->getCell("VoidGapC");
    System.removeCell(iRemove);
     */ 
      //  
    
    //     Control.addVariable("LayerFlag",0);
    
    for (int i=0;i<93;i++){
      //  if (i>86)   Control.addVariable("LayerFlag",1);

      if (i<92){
     E02SteelInsertSystemF[i+1]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02SteelSystemF"+std::to_string(i+1)));
    OR.addObject ("bifrostE02SteelSystemF"+std::to_string(i+1),
		  E02SteelInsertSystemF[i+1]);
     E02SteelInsertSystemB[i+1]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02SteelSystemB"+std::to_string(i+1)));
    OR.addObject ("bifrostE02SteelSystemB"+std::to_string(i+1),
		  E02SteelInsertSystemB[i+1]);
     E02SteelSpacingSystem[i+1]=
       std::shared_ptr<constructSystem::D03SteelSpacing>
(new constructSystem::D03SteelSpacing("bifrostE02SpacingSystem"+std::to_string(i+1)));
    OR.addObject ("bifrostE02SpacingSystem"+std::to_string(i+1),
		  E02SteelSpacingSystem[i+1]);

    if (i==22)     E02SteelInsertSystemF[i+1]->setFront(*E02SteelChopperBack,2);
    else if (i==21)     E02SteelInsertSystemF[i+1]->setFront(*E02SteelChopperFront,2);
    else E02SteelInsertSystemF[i+1]->setFront(*E02UpperSystem[i],-1);
    E02SteelInsertSystemF[i+1]->setBack(*E02UpperSystem[i],8);
    E02SteelInsertSystemF[i+1]->createAll(System,*E02UpperSystem[i],-1);

    if((i!=21)&&(i!=22)){ iRemove=E02UpperSystem[i]->getCell("VoidF");
      System.removeCell(iRemove);}
    E02SteelInsertSystemB[i+1]->setFront(*E02UpperSystem[i],7);
      if (i==23) E02SteelInsertSystemB[i+1]->setBack(*E02SteelChopperBack,-2);
      else if (i==20) E02SteelInsertSystemB[i+1]->setBack(*E02SteelChopperFront,-2);
     else E02SteelInsertSystemB[i+1]->setBack(*E02UpperSystem[i],-2);
    E02SteelInsertSystemB[i+1]->createAll(System,*E02UpperSystem[i],7);

    if((i!=21)&&(i!=22)){ iRemove=E02UpperSystem[i]->getCell("VoidB");
      System.removeCell(iRemove);}

    E02SteelSpacingSystem[i+1]->setBack(*E02UpperSystem[i],-7);
    E02SteelSpacingSystem[i+1]->setFront(*E02UpperSystem[i],-8);
    E02SteelSpacingSystem[i+1]->createAll(System,*E02UpperSystem[i],-8);
    // if((i!=21)&&(i!=22))
      { iRemove=E02UpperSystem[i]->getCell("VoidGapC");
      System.removeCell(iRemove);}
    } else {//i=92 -- only F part is steel B pat is concrete
	// Control.addVariable("LayerFlag",1);

      E02SteelInsertSystemF[i+1]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02SteelSystemF"+std::to_string(i+1)));
    OR.addObject ("bifrostE02SteelSystemF"+std::to_string(i+1),
		  E02SteelInsertSystemF[i+1]);
    E02SteelInsertSystemF[i+1]->setFront(*E02UpperSystem[i],-1);
    E02SteelInsertSystemF[i+1]->setBack(*E02UpperSystem[i],8);
    E02SteelInsertSystemF[i+1]->createAll(System,*E02UpperSystem[i],-1);
      iRemove=E02UpperSystem[i]->getCell("VoidF");
      System.removeCell(iRemove);

      E02SteelSpacingSystem[i+1]=
       std::shared_ptr<constructSystem::D03SteelSpacing>
(new constructSystem::D03SteelSpacing("bifrostE02SpacingSystem"+std::to_string(i+1)));
    OR.addObject ("bifrostE02SpacingSystem"+std::to_string(i+1),
		  E02SteelSpacingSystem[i+1]);

          E02SteelSpacingSystem[i+1]->setBack(*E02UpperSystem[i],-7);
    E02SteelSpacingSystem[i+1]->setFront(*E02UpperSystem[i],-8);
    E02SteelSpacingSystem[i+1]->createAll(System,*E02UpperSystem[i],-8);
    iRemove=E02UpperSystem[i]->getCell("VoidGapC");
    System.removeCell(iRemove);
  
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"VoidLeft",148*0.5);
 Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"VoidRight",148*0.5);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"VoidHeight",86.5);

  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"Left",215*0.5);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"Right",215*0.5);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"Height",120.0);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"DefMat",
		      "LimestoneConcrete");
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"Spacing",1.0);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"Depth",-16);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"VoidDepth",-16);

        E02SteelInsertSystemB[i+1]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02SteelSystemB"+std::to_string(i+1)));
    OR.addObject ("bifrostE02SteelSystemB"+std::to_string(i+1),
		  E02SteelInsertSystemB[i+1]);

     E02SteelInsertSystemB[i+1]->setBack(*E02UpperSystem[i],-2);
    E02SteelInsertSystemB[i+1]->createAll(System,*E02UpperSystem[i],-9);
  
    std::vector<int> iR ;
     try{iR=E02SteelInsertSystemB[i+1]->getCells("Floor");
      for(int i=0 ; i< iR.size();i++) System.removeCell(iR[i]);
     }catch(...){};
     System.removeCell(E02SteelInsertSystemB[i+1]->getCell("Void"));
    }
    }


    //     Control.addVariable("LayerFlag",1);


    
for (int i=93;i<102;i++){
  // std::cout << i << std::endl;
  Control.addVariable("bifrostE02SteelSystemF"+std::to_string(i+1)+"VoidLeft",148*0.5);
  Control.addVariable("bifrostE02SteelSystemF"+std::to_string(i+1)+"VoidRight",148*0.5);
  Control.addVariable("bifrostE02SteelSystemF"+std::to_string(i+1)+"VoidHeight",86.5);

  Control.addVariable("bifrostE02SteelSystemF"+std::to_string(i+1)+"Left",215*0.5);
  Control.addVariable("bifrostE02SteelSystemF"+std::to_string(i+1)+"Right",215*0.5);
  Control.addVariable("bifrostE02SteelSystemF"+std::to_string(i+1)+"Height",120.0);

    Control.addVariable("bifrostE02SteelSystemF"+std::to_string(i+1)+"DefMat",
		      "LimestoneConcrete");
    // Control.addVariable("bifrostE02SteelSystemF"+std::to_string(i+1)+"Spacing",1.0);
  Control.addVariable("bifrostE02SteelSystemF"+std::to_string(i+1)+"Depth",-16);
  Control.addVariable("bifrostE02SteelSystemF"+std::to_string(i+1)+"VoidDepth",-16);

  Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"VoidLeft",148*0.5);
  Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"VoidRight",148*0.5);
  Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"VoidHeight",86.5);

  Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"Left",215*0.5);
  Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"Right",215*0.5);
  Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"Height",120.0);

    Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"NWallLayersR",
		     2);
    Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"NWallLayersL",
		      2);
    Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"NRoofLayers",
		      2);
    Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"DefMat",
		      "Air");
  // Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"Spacing",1.0);
  Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"Depth",-16);
  Control.addVariable("bifrostE02SpacingSystem"+std::to_string(i+1)+"VoidDepth",-16);

  
  
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"VoidLeft",148*0.5);
 Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"VoidRight",148*0.5);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"VoidHeight",86.5);

  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"Left",215*0.5);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"Right",215*0.5);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"Height",120.0);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"DefMat",
		      "LimestoneConcrete");
  //Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"Spacing",1.0);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"Depth",-16);
  Control.addVariable("bifrostE02SteelSystemB"+std::to_string(i+1)+"VoidDepth",-16);

  
        E02SteelInsertSystemB[i+1]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02SteelSystemB"+std::to_string(i+1)));
    OR.addObject ("bifrostE02SteelSystemB"+std::to_string(i+1),
		  E02SteelInsertSystemB[i+1]);

        E02SteelInsertSystemF[i+1]=
       std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02SteelSystemF"+std::to_string(i+1)));
    OR.addObject ("bifrostE02SteelSystemF"+std::to_string(i+1),
		  E02SteelInsertSystemB[i+1]);

        E02SteelSpacingSystem[i+1]=
       std::shared_ptr<constructSystem::D03SteelSpacing>
(new constructSystem::D03SteelSpacing("bifrostE02SpacingSystem"+std::to_string(i+1)));
    OR.addObject ("bifrostE02SpacingSystem"+std::to_string(i+1),
		  E02SteelSpacingSystem[i+1]);

    //  E02SteelInsertSystemB[i+1]->setFront(*E02UpperSystem[i],-9);

    E02SteelInsertSystemB[i+1]->setBack(*E02UpperSystem[i],-2);
    E02SteelInsertSystemB[i+1]->createAll(System,*E02UpperSystem[i],-9);

    E02SteelInsertSystemF[i+1]->setBack(*E02UpperSystem[i],-1);
    E02SteelInsertSystemF[i+1]->createAll(System,*E02UpperSystem[i],-10);
    System.removeCell(E02SteelInsertSystemB[i+1]->getCell("Void"));
    System.removeCell(E02SteelInsertSystemF[i+1]->getCell("Void"));

         E02SteelSpacingSystem[i+1]->setBack(*E02UpperSystem[i],-9);
 
    //  E02SteelSpacingSystem[i+1]->setFront(*E02UpperSystem[i],10);
    //    std::cout << "spacing"<<i+1<< std::endl; 
       E02SteelSpacingSystem[i+1]->createAll(System,*E02UpperSystem[i],10);
 
     System.removeCell(E02SteelSpacingSystem[i+1]->getCell("Void"));
 
    std::vector<int> iR ;
     try{iR=E02SteelInsertSystemB[i+1]->getCells("Floor");
      for(int i=0 ; i< iR.size();i++) System.removeCell(iR[i]);
     }catch(...){};

         try{iR=E02SteelInsertSystemF[i+1]->getCells("Floor");
      for(int i=0 ; i< iR.size();i++) System.removeCell(iR[i]);
     }catch(...){};
  
	          try{iR=E02SteelSpacingSystem[i+1]->getCells("Floor");
      for(int i=0 ; i< iR.size();i++) System.removeCell(iR[i]);
     }catch(...){};
	 
 }




    std::shared_ptr<constructSystem::D03SteelInsert>  E02B4CChopperBack=
      std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02B4CSystemChopperBack"));
    OR.addObject ("bifrostE02B4CSystemChopperBack", E02B4CChopperBack);


    //    E02B4CChopperBack->setFront(*E02UpperSystem[22],-1);
    //    E02B4CChopperBack->addInsertCell(E02SteelInsertSystemF[22]->getCell("Void"));
  E02B4CChopperBack->createAll(System,*E02SteelChopperBack,-2);
      System.removeCell(E02B4CChopperBack->getCell("Void"));
    attachSystem::addToInsertSurfCtrl(System,*E02SteelChopperBack,*E02B4CChopperBack);

      std::shared_ptr<constructSystem::D03SteelInsert>  E02B4CChopperFront=
             std::shared_ptr<constructSystem::D03SteelInsert>
(new constructSystem::D03SteelInsert("bifrostE02B4CSystemChopperFront"));
    OR.addObject ("bifrostE02B4CSystemChopperFront", E02B4CChopperFront);
    //    E02B4CChopperFront->setFront(*E02UpperSystem[21],-1);
    E02B4CChopperFront->createAll(System,*E02SteelChopperFront,-2);
    attachSystem::addToInsertSurfCtrl(System,*E02SteelChopperFront,*E02B4CChopperFront);
    System.removeCell(E02B4CChopperFront->getCell("Void"));





 
//Central part of extra shielding in E02
 int voidTop4, voidRight4, voidLeft4;
      splitIndex =  E02SteelInsertSystemF[94]->getLinkSurf(6);
      voidTop4 =  System.splitObject(E02VoidFinal,-splitIndex);
      splitIndex =  E02SteelInsertSystemF[94]->getLinkSurf(5);
      voidTmp =  System.splitObject(voidTop4,splitIndex);
      System.removeCell(voidTop4);

      splitIndex =  E02UpperSystem[94]->getLinkSurf(3);
      voidTmp =  System.splitObject(voidTmp,-splitIndex);
      splitIndex =  E02UpperSystem[94]->getLinkSurf(4);
      voidTmp =  System.splitObject(voidTmp,-splitIndex);

      System.removeCell(voidTmp);

      splitIndex =  E02SteelInsertSystemB[93]->getLinkSurf(1);
      voidTmp =  System.splitObject(voidE02BaseR,-splitIndex);

      splitIndex =  E02SteelInsertSystemF[94]->getLinkSurf(6);
      voidTmp =  System.splitObject(voidTmp,-splitIndex);
      splitIndex =  E02SteelInsertSystemF[94]->getLinkSurf(4);
      voidTmp =  System.splitObject(voidTmp,-splitIndex);
      splitIndex =  E02SteelInsertSystemF[94]->getLinkSurf(5);
      voidTmp =  System.splitObject(voidTmp,-splitIndex);
      System.removeCell(voidTmp);

      splitIndex =  E02SteelInsertSystemB[93]->getLinkSurf(1);
      voidTmp =  System.splitObject(voidE02BaseL,-splitIndex);
      splitIndex =  E02SteelInsertSystemF[94]->getLinkSurf(6);
      voidTmp =  System.splitObject(voidTmp,-splitIndex);
      splitIndex =  E02SteelInsertSystemF[94]->getLinkSurf(3);
      voidTmp =  System.splitObject(voidTmp,-splitIndex);
      splitIndex =  E02SteelInsertSystemF[94]->getLinkSurf(5);
      voidTmp =  System.splitObject(voidTmp,-splitIndex);
      System.removeCell(voidTmp);

      /********************************/
       D03SteelVoid->setFront(*D03SecondUpper,7);
      D03SteelVoid->setBack(*E02UpperSystem[92],-7);
      //  D03SteelVoid->setBack(*D03UpperShutter[0],-7);
      iVoid=D03BaseSteelStart[0]->getCells("VoidB");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
iVoid=D03FirstBase->getCells("VoidB");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
iVoid=D03BaseSteelStart[0]->getCells("VoidF");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
    iVoid=D03FirstBase->getCells("VoidGapC");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
    // iVoid=D03BaseShutter[0]->getCells("VoidF");
     iVoid=D03BaseSteelShutter[3]->getCells("VoidF");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
     iVoid=D03BaseSteelShutter[3]->getCells("VoidB");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
        iVoid=D03BaseShutter[0]->getCells("VoidB");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);

    iVoid=D03BaseShutter[2]->getCells("VoidB");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);

    iVoid=D03BaseShutter[2]->getCells("VoidF");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);

    iVoid=D03BaseShutter[2]->getCells("VoidGapC");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);

    /*iVoid=D03BaseShutter[0]->getCells("VoidGapC");
    for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
    */
      

    for (int i=0; i<4; i++){
      iVoid=D03BaseSystem[i]->getCells("VoidF");
      for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
      iVoid=D03BaseSystem[i]->getCells("VoidB");
      for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
      iVoid=D03BaseSystem[i]->getCells("VoidGapC");
      for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
    }

    
    for (int i=0;i<22;i++){
      iVoid=E02Base[i]->getCells("VoidF");
      for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
      iVoid=E02Base[i]->getCells("VoidB");
      for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
      iVoid=E02Base[i]->getCells("VoidGapC");
      for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
    }
     iVoid=E02Base[22]->getCells("VoidF");
      for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
     

      iVoid=D03BaseShutter[1]->getCells("VoidF");
      for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
      iVoid=D03BaseShutter[1]->getCells("VoidB");
      for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
      /*      iVoid=D03BaseShutter[1]->getCells("VoidGapC");
      for(size_t i=0;i<iVoid.size();i++)
       D03SteelVoid->addInsertCell(iVoid[i]);
      */
      /*      D03SteelVoid->addInsertCell(E02UpperSystem[21]->getCell("VoidF"));
      D03SteelVoid->addInsertCell(E02UpperSystem[21]->getCell("VoidB"));
      D03SteelVoid->addInsertCell(E02UpperSystem[22]->getCell("VoidF"));
      D03SteelVoid->addInsertCell(E02UpperSystem[22]->getCell("VoidB"));
       D03SteelVoid->addInsertCell(E02UpperSystem[21]->getCell("VoidGapC"));
      D03SteelVoid->addInsertCell(E02UpperSystem[22]->getCell("VoidGapC"));
      */

    D03SteelVoid->createAll(System,*D03SecondUpper,7);
     //Created just to free space for the steel insertions, now removing
     std::vector<int> iR ;
     try{iR= D03SteelVoid->getCells();
      for(int i=0 ; i< iR.size();i++) System.removeCell(iR[i]);
     }catch(...){};


     

  const attachSystem::FixedComp* LinkPtr= &FocusOutC->getKey("Guide0");

  RecPipe[0]->addInsertCell(voidShutter);
  RecPipe[0]->addInsertCell(D03SteelShutter[4]->getCell("Void"));
  RecPipe[0]->addInsertCell(D03SteelShutterPhantom->getCell("Void"));
  RecPipe[0]->addInsertCell(E02SteelInsertSystemB[0]->getCell("Void"));
  RecPipe[0]->setBack(*E02SteelInsertSystemB[0],-2);
  RecPipe[1]->setBack(*E02SteelInsertSystemB[22],-2);
	RecPipe[0]->createAll(System,FocusOutC->getKey("Guide0"),2);
	//     attachSystem::addToInsertForced(System,*D03SteelVoidShutterExtra,*RecPipe[0 ]);

	for(int i=0; i<23;i++){
	  RecPipe[1]->addInsertCell(E02SteelInsertSystemF[i]->getCell("Void"));
	  if(i>0)RecPipe[1]->addInsertCell(E02SteelInsertSystemB[i]->getCell("Void"));
	  RecPipe[1]->addInsertCell(E02SteelSpacingSystem[i]->getCell("Void"));
	}
	
	RecPipe[1]->addInsertCell(E02BaseSteelChopper->getCell("VoidF"));	
	RecPipe[1]->addInsertCell(E02BaseSteelChopper->getCell("VoidB"));	
      RecPipe[1]->createAll(System,*RecPipe[0],2);
      voidTmp=RecPipe[1]->getCell("Void");
      for (int i=0; i<nGuideSection; i++){  
	if (i==0)  RecFocus[i]->addInsertCell(RecPipe[0]->getCell("Void"));
      RecFocus[i]->addInsertCell(voidTmp);
      RecFocus[i]->createAll(System,*LinkPtr,2,*LinkPtr,2);
      LinkPtr= &RecFocus[i]->getKey("Guide0");
     
      voidTmp=System.splitObject(voidTmp,RecFocus[i]->getKey("Guide0").getLinkSurf(2));

      }

      for(int i=23; i<94;i++){
	SndPipe[0]->addInsertCell(E02SteelInsertSystemF[i]->getCell("Void"));
      if (i<93)  SndPipe[0]->addInsertCell(E02SteelInsertSystemB[i]->getCell("Void"));
         SndPipe[0]->addInsertCell(E02SteelSpacingSystem[i]->getCell("Void"));
	}
      
      for(int i=92; i<102;i++){
	if(i>92)SndPipe[0]->addInsertCell(E02UpperSystem[i]->getCell("VoidF"));
	  SndPipe[0]->addInsertCell(E02UpperSystem[i]->getCell("VoidB"));
	if(i>92)  SndPipe[0]->addInsertCell(E02UpperSystem[i]->getCell("VoidGapC"));
	  }
	SndPipe[0]->addInsertCell(E02BaseSteelChopper->getCell("VoidF"));	
	SndPipe[0]->addInsertCell(E02BaseSteelChopper->getCell("VoidB"));	

      SndPipe[0]->setBack(*E02UpperSystem[101],-1);
      SndPipe[0]->createAll(System, *RecPipe[1], 2);     
     
      voidTmp=SndPipe[0]->getCell("Void");

      B4CChopper->addInsertCell(voidTmp);
      B4CChopper->createAll(System,*SndPipe[0],-1);
      voidTmp=System.splitObject(voidTmp,B4CChopper->getLinkSurf(2));

      BeamMonitorChopper->addInsertCell(voidTmp);
      BeamMonitorChopper->createAll(System,*SndPipe[0],-1);
      voidTmp=System.splitObject(voidTmp,BeamMonitorChopper->getLinkSurf(2));
      for(size_t i=0;i<nSndSection;i++)
    {
      // Rectangle 6m section
      //      SndPipe[i]->addInsertCell(ShieldB->getCell("Void"));
      SndFocus[i]->addInsertCell(voidTmp);
      if (i<(nSndSection-1))
	{  SndFocus[i]->createAll(System,*LinkPtr,2,*LinkPtr,2);
     voidTmp=System.splitObject(voidTmp,SndFocus[i]->getKey("Guide0").getLinkSurf(2));
	}      else
	{   SndFocus[i]->setBack(*E02SteelInsertSystemB[84],-1);
	    SndFocus[i]->createAll(System,*LinkPtr,2,*LinkPtr,2);
	    voidTmp=System.splitObject(voidTmp,SndFocus[i]->getKey("Guide0").getLinkSurf(2));
	}
      LinkPtr= &SndFocus[i]->getKey("Guide0");
    }

      for(size_t i=0;i<nEllSection;i++)
    {
      //      std::cout << i << std::endl;
      // Elliptic 6m sections
      // EllPipe[i]->addInsertCell(ShieldB->getCell("Void"));
      //  if (i==0) EllPipe[i]->createAll(System,*SndPipe[0],2);
      // else       EllPipe[i]->createAll(System,*EllPipe[i-1],2);
      EllFocus[i]->addInsertCell(voidTmp);
       if (i==nEllSection-1) EllFocus[i]->setBack(*E02UpperSystem[101],-1);
      	   
      EllFocus[i]->createAll(System,*LinkPtr,2,*LinkPtr,2);
      if (i!=nEllSection-1)
	voidTmp=System.splitObject(voidTmp,EllFocus[i]->getKey("Guide0").getLinkSurf(2));
      LinkPtr= &EllFocus[i]->getKey("Guide0");
      }

	   

       std::array<std::shared_ptr<constructSystem::D03SupportBlock>,2> E01SupportSystem;
    for (int i=0;i<2;i++){  

      //           Control.addVariable("bifrostE01SupportYStep",3);
      // if (i==0)   Control.addVariable("bifrostE01Support"+std::to_string(i)+"ZStep",-113.7);
          Control.addVariable("bifrostE01Support"+std::to_string(i)+"Length1Edge",80);
           Control.addVariable("bifrostE01Support"+std::to_string(i)+"Length1Mid",95);
           Control.addVariable("bifrostE01Support"+std::to_string(i)+"Length1Low",60);
           Control.addVariable("bifrostE01Support"+std::to_string(i)+"Length2Low",50);
           Control.addVariable("bifrostE01Support"+std::to_string(i)+"BuildTube",0);
  Control.addVariable("bifrostE01Support"+std::to_string(i)+"HeightUp",70);
  Control.addVariable("bifrostE01Support"+std::to_string(i)+"HeightLow",130);

	   Control.addVariable("bifrostE01Support"+std::to_string(i)+"Width1B",240);
	   Control.addVariable("bifrostE01Support"+std::to_string(i)+"Width1F",240);

      E01SupportSystem[i]=std::shared_ptr<constructSystem::D03SupportBlock>
	(new constructSystem::D03SupportBlock("bifrostE01Support"+std::to_string(i)));
      OR.addObject ("bifrostE01Support"+std::to_string(i),E01SupportSystem[i]);}
    E01SupportSystem[0]->createAll(System,*E02Base[22],2);
  
    E01SupportSystem[1]->setBack(*E02UpperSystem[101],-1);
    E01SupportSystem[1]->createAll(System, *E01SupportSystem[0],2);
    int E01Floor=System.splitObject(voidE01,E01SupportSystem[0]->getLinkSurf(5));
    System.findObject(E01Floor)->setMaterial(144);
    System.setMaterialDensity(E01Floor);


    int voidCave=System.splitObject(voidE01,E02UpperSystem[101]->getLinkSurf(1));

    voidTmp=System.splitObject(voidE01,-E01SupportSystem[0]->getLinkSurf(6));
int voidE1LowL=System.splitObject(voidTmp,E01SupportSystem[0]->getLinkSurf(3));
int voidE1LowR=System.splitObject(voidTmp,E01SupportSystem[0]->getLinkSurf(4));
 System.removeCell(voidTmp);

 //     Control.addVariable("LayerFlag",1);

 

    //    E02Base[i]->addInsertCell(voidRear1new);
 E02Base[24]->setBack(*E02UpperSystem[101],-1);
 E02Base[25]->setBack(*E02UpperSystem[101],-1);
 E02Base[24]->createAll(System,*E02Base[22],2);

	
   
    
    int voidE01BaseL=System.splitObject(voidE01, E02Base[24]->getLinkSurf(3));
    
    int voidE01BaseR=System.splitObject(voidE01, E02Base[24]->getLinkSurf(4));
   
    int voidE01Base=System.splitObject(voidE01, -E02Base[24]->getLinkSurf(6));
    // iRemove=System.splitObject(voidE02Base, E02Base[24]->getLinkSurf(2));
 System.removeCell(voidE01Base);

 E02Base[25]->addInsertCell(voidE01BaseL);
 E02Base[25]->addInsertCell(voidE01BaseR);
 
 E02Base[25]->createAll(System,*E02Base[22],2);
 System.removeCell( E02Base[25]->getCell("VoidB"));
 System.removeCell( E02Base[25]->getCell("VoidF"));
 System.removeCell( E02Base[25]->getCell("VoidGapC"));
  attachSystem::addToInsertForced(System,
			       E02Base[25]->getCells("VoidLow"),*E02Base[24]);

  //       int E01voidTop =  System.splitObject(voidE01Base,splitIndex);
	
    splitIndex =  E02SteelInsertSystemB[101]->getLinkSurf(6);

      voidTmp =  System.splitObject(voidE01,-splitIndex);
     
      splitIndex =  E02SteelInsertSystemB[101]->getLinkSurf(5);
    System.removeCell(System.splitObject(voidTmp,-splitIndex));
   
    splitIndex =  E02UpperSystem[101]->getLinkSurf(4);
    voidTmp =  System.splitObject(voidTmp,-splitIndex);
   
      splitIndex =  E02UpperSystem[101]->getLinkSurf(3);
     voidTmp  =  System.splitObject(voidTmp,-splitIndex);
    
          System.removeCell(voidTmp);

    splitIndex =  E02SteelInsertSystemB[101]->getLinkSurf(6);
     voidTmp =  System.splitObject(voidE01BaseL,-splitIndex);

    splitIndex =  E02SteelInsertSystemB[101]->getLinkSurf(5);
     voidTmp =  System.splitObject(voidTmp,-splitIndex);

    splitIndex =  E02SteelInsertSystemB[101]->getLinkSurf(3);
     voidTmp =  System.splitObject(voidTmp,-splitIndex);
     System.removeCell(voidTmp);

     
    splitIndex =  E02SteelInsertSystemB[101]->getLinkSurf(6);
     voidTmp =  System.splitObject(voidE01BaseR,-splitIndex);

    splitIndex =  E02SteelInsertSystemB[101]->getLinkSurf(5);
     voidTmp =  System.splitObject(voidTmp,-splitIndex);

    splitIndex =  E02SteelInsertSystemB[101]->getLinkSurf(4);
     voidTmp =  System.splitObject(voidTmp,-splitIndex);
     System.removeCell(voidTmp);


     
           buildCave(&System,*E02UpperSystem[101],voidCave,voidE1LowL,voidE1LowR);
     return voidCave;
    
}



void 
BIFROST::buildCave(Simulation* SimPtr,
		       //		       const attachSystem::FixedComp& FTA,
		       //                       const long int thermalIndex,
		       //		       const CompBInsert& BInsert,
		   const constructSystem::D03UpperBlock ShieldInterface,
		   const int voidCell, const int voidE01BaseL, const int voidE01BaseR)
  /*!

makeCave::build(Simulation* SimPtr,
		       const mainSystem::inputParam&)
  Carry out the full build
  \param SimPtr :: Simulation system
  \param :: Input parameters
*/
{
  // For output stream
  ELog::RegMethod RControl("makeCave","build");
  // FuncDataBase& Control=SimPtr->getDataBase();

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  FuncDataBase& Control=SimPtr->getDataBase();

  //     Control.addVariable("LayerFlag",1);
  
  //  int voidCell(74123);
  int voidRight, voidLeft, voidFront,voidTop,voidRear;
  //  int splitIndex = SampleLabyrinth->getLinkSurf(2);
  //    SimPtr->findObject(voidCell)->setMaterial(60); // air around bunker
  
  
  ACave->addInsertCell(voidCell);
  ACave->createAll(*SimPtr,ShieldInterface,1);
  std::vector<int>iR = ACave->getCells("Floor");
  for(size_t i=0;i<iR.size();i++){
    SimPtr->removeCell(iR[i]);
  }
  SimPtr->removeCell(ACave->getCell("InnerSpace"));
  CaveLining->createAll(*SimPtr,*ACave,0);
  // ACave->addInsertCell(voidCell);
  
  int splitIndex = ACave->getKey("Outer").getLinkSurf(4);
      voidRight =  SimPtr->splitObject(voidCell,splitIndex);
     splitIndex = ACave->getKey("Outer").getLinkSurf(3);
      voidLeft =  SimPtr->splitObject(voidCell,splitIndex);







     
      splitIndex = ACave->getKey("Outer").getLinkSurf(2);
      voidFront =  SimPtr->splitObject(voidCell,splitIndex);
     
       splitIndex = ACave->getKey("Outer").getLinkSurf(6);
      voidTop =  SimPtr->splitObject(voidCell,splitIndex);

      /*try{
      splitIndex = ACave->getKey("Outer").getLinkSurf(1);
      voidRear =  SimPtr->splitObject(voidCell,splitIndex);
      SimPtr->removeCell(voidRear);}
      catch(...){std::cout << "No cell found" << std::endl;}
      */
      
      
  SampleLabyrinth->addInsertCell(voidRight);
  SampleLabyrinth->createAll(*SimPtr,ACave->getKey("Outer"),4);
 
  SampleCorridor->addInsertCell(voidRight);
  SampleCorridor->addInsertCell(voidTop);
  SampleCorridor->setBack(ACave->getKey("Outer"),-1);
  SampleCorridor->createAll(*SimPtr,*SampleLabyrinth,4);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*SampleCorridor,*ACave);

  SampleLabyrinthB4C->addInsertCell(SampleLabyrinth->getCell("Void"));
  SampleLabyrinthB4C->addInsertCell(SampleCorridor->getCell("RoofLining"));
  SampleLabyrinthB4C->addInsertCell(SampleCorridor->getCells("WallLining")[0]);
  SampleLabyrinthB4C->addInsertCell(SampleCorridor->getCells("WallLining")[1]);
  SampleLabyrinthB4C->addInsertCell(SampleCorridor->getCells("CornerB4C")[0]);
  SampleLabyrinthB4C->addInsertCell(SampleCorridor->getCells("CornerB4C")[1]);
  SampleLabyrinthB4C->addInsertCell(SampleCorridor->getCell("Void"));
  SampleLabyrinthB4C->createAll(*SimPtr,ACave->getKey("Outer"),4);
 

  
  
  DetectorLabyrinth->addInsertCell(voidLeft);
  DetectorLabyrinth->setFront(ACave->getKey("Outer"),3);
  DetectorLabyrinth->createAll(*SimPtr,ACave->getKey("Outer"),3);

  //  DetectorLabyrinthB4C->addInsertCell(DetectorLabyrinth->getCell("Void"));

  DetectorLabyrinthB4C->setFront(ACave->getKey("Outer"),3);
  DetectorLabyrinthB4C->createAll(*SimPtr,ACave->getKey("Outer"),3);

  SimPtr->removeCell(SimPtr->splitObject(DetectorLabyrinth->getCell("Void"),
					   -DetectorLabyrinthB4C->getLinkSurf(3)));

  
  DetectorAccess->addInsertCell(ACave->getCells("LeftWall"));  
  DetectorAccess->addInsertCell(ACave->getCells("CornerD"));
  //  DetectorAccess->addInsertCell(ACave->getCells("InnerSpace"));  
  DetectorAccess->addInsertCell(CaveLining->getCells("LeftWall"));  
  //  DetectorAccess->setFront(ACave->getKey("Outer"),-3);
  //  DetectorAccess->addInsertCell(DetectorLabyrinth->getCells("FrontLining"));  
  // DetectorAccess->addInsertCell(DetectorLabyrinth->getCells("Void"));  
 
  DetectorAccess->setBack(ACave->getKey("Outer"),-3);
  DetectorAccess->setFront(ACave->getKey("Inner"),3);
  DetectorAccess->
    createAll(*SimPtr,ACave->getKey("Inner"),3);


  DetectorAccessOuter->setFront(DetectorLabyrinth->getKey("Inner"),-2);
  DetectorAccessOuter->setBack(DetectorLabyrinth->getKey("Outer"),-2);
    DetectorAccessOuter->addInsertCell(voidLeft);
  DetectorAccessOuter->
    createAll(*SimPtr,
	DetectorLabyrinth->getKey("Inner"),2,DetectorLabyrinth->getKey("Inner"),2);
   attachSystem::addToInsertSurfCtrl(*SimPtr,*DetectorLabyrinth,*DetectorAccessOuter);


  DetectorLabyrinthB4Cexit->createAll(*SimPtr,DetectorLabyrinth->getKey("Inner"),-2);
      SimPtr->findObject(DetectorLabyrinthB4Cexit->getCells("Roof")[0])->setMaterial(60);
  SimPtr->setMaterialDensity(DetectorLabyrinthB4Cexit->getCells("Roof")[0]);

  
  
  SimPtr->removeCell(SimPtr->splitObject(DetectorLabyrinth->getCell("Void"),
					   -DetectorLabyrinthB4Cexit->getLinkSurf(3)));

   std::cout <<"Removing unneeded cells" << std::endl;
   
   try{
iR = DetectorLabyrinth->getCells("Floor");
  for(size_t i=0;i<iR.size();i++){
    SimPtr->removeCell(iR[i]);
  }}catch(...){}
   
   try{
iR = DetectorAccess->getCells("Floor");
  for(size_t i=0;i<iR.size();i++){
    SimPtr->removeCell(iR[i]);
  }}catch(...){}
   try{
  
iR = DetectorAccessOuter->getCells("Floor");
  for(size_t i=0;i<iR.size();i++){
    SimPtr->removeCell(iR[i]);
  }}catch(...){}
   try{


  iR = SampleLabyrinth->getCells("Floor");
  for(size_t i=0;i<iR.size();i++){
    SimPtr->removeCell(iR[i]);
  }}catch(...){}
   try{

 iR = SampleCorridor->getCells("Floor");
  for(size_t i=0;i<iR.size();i++){
    SimPtr->removeCell(iR[i]);
  }
 }catch(...){}


   
   //Control.addVariable("BIFROSTCaveHatch3DefMat","Stainless304");

    Hatch3->addInsertCell(ACave->getCells("RoofInner"));
    Hatch3->createAll(*SimPtr,ACave->getKey("Outer"),-6);

    Hatch->setBack(CaveLining->getKey("Inner"),6);
    Hatch->setFront(ACave->getKey("Outer"),-6);
    Hatch->addInsertCell(ACave->getCells("RoofInner"));
    //   Hatch->addInsertCell(ACave->getCells("InnerSpace"));
    Hatch->addInsertCell(CaveLining->getCells("RoofInner"));
    Hatch->addInsertCell(CaveLining->getCells("RoofOuter"));
    Hatch->createAll(*SimPtr,*Hatch3,-1);

    //Control.addVariable("BIFROSTHatch2DefMat","Stainless304");

    Hatch2->addInsertCell(ACave->getCells("RoofInner"));
    Hatch2->setBack(CaveLining->getKey("Inner"),6);
    Hatch2->setFront(*Hatch3,2);
    Hatch2->createAll(*SimPtr,*Hatch3,2);

    
    
  
    attachSystem::addToInsertForced(*SimPtr,*Hatch,*Hatch2);
      attachSystem::addToInsertForced(*SimPtr,*Hatch,*Hatch3);
    
    //    UpperBlockWall->setFront(ACave->getKey("Outer"),-4);
    //        UpperBlockWall->setBack(ACave->getKey("Inner"),4);
  
    
    
//  UpperBlockWall->createAll(*SimPtr,ACave->getKey("Outer"),6);
  UpperBlockWall->addInsertCell(voidTop);
  UpperBlockWall->setBack(ACave->getKey("Inner"),4);
  UpperBlockWall->addInsertCell(ACave->getCell("RoofOuter"));
  UpperBlockWall->addInsertCell(CaveLining->getCell("RoofOuter"));
  UpperBlockWall->addInsertCell(ACave->getCell("RightWall"));
  UpperBlockWall->addInsertCell(CaveLining->getCell("RightWall"));
  UpperBlockWall->createAll(*SimPtr,ACave->getKey("Outer"),-4);
  //  attachSystem::addToInsertSurfCtrl(*SimPtr,*ACave,*UpperBlockWall);
  //  attachSystem::addToInsertSurfCtrl(*SimPtr,*Hatch,*UpperBlockWall);
  //attachSystem::addToInsertSurfCtrl(*SimPtr,*Hatch2,*UpperBlockWall);
  //attachSystem::addToInsertSurfCtrl(*SimPtr,*Hatch3,*UpperBlockWall);

  /*
  GallerySteel->addInsertCell(UpperBlockWall->getCells("Void"));
  GallerySteel->createAll(*SimPtr,ACave->getKey("Outer"),-4);
 
  */

  /*GalleryDoor->addInsertCell(UpperBlockWall->getCells("Void"));
  GalleryDoor->createAll(*SimPtr,ACave->getKey("Outer"),-4);
  */
     splitIndex = UpperBlockWall->getLinkSurf(2);
     int voidTop1 =  SimPtr->splitObject(voidTop,splitIndex);
  
  
     //   UpperBlockRoof->setFront(*UpperBlockWall,2);
  
     
//  UpperBlockWall->createAll(*SimPtr,ACave->getKey("Outer"),6);
  UpperBlockRoof1->addInsertCell(voidTop1);
  UpperBlockRoof1->addInsertCell(ACave->getCell("RoofOuter"));
  UpperBlockRoof1->addInsertCell(ACave->getCell("RoofInner"));
  UpperBlockRoof1->addInsertCell(ACave->getCell("RightWall"));
  //  UpperBlockRoof1->addInsertCell(ACave->getCell("InnerSpace"));
  UpperBlockRoof1->addInsertCell(CaveLining->getCell("RoofOuter"));
  UpperBlockRoof1->addInsertCell(CaveLining->getCell("RoofInner"));
  UpperBlockRoof1->addInsertCell(CaveLining->getCell("RightWall"));
  //  UpperBlockRoof1->setBack(ACave->getKey("Inner"),4);
  UpperBlockRoof1->createAll(*SimPtr,*UpperBlockWall,2);
  //  attachSystem::addToInsertSurfCtrl(*SimPtr,*ACave,*UpperBlockWall);
  //  attachSystem::addToInsertSurfCtrl(*SimPtr,*Hatch,*UpperBlockWall);
  
   

     std::vector<int>
     voidCells1= Hatch->getCells("Void");
    
     for (size_t i=0;i<voidCells1.size(); i++)
      {
      int nextCell= SimPtr->splitObject(voidCells1[i],-splitIndex);
      SimPtr->removeCell(nextCell);
      }

     

     std::vector<int>
    wallCells1= Hatch->getCells("Wall");
 
    for (size_t i=0;i<wallCells1.size(); i++)
      {
      int nextCell= SimPtr->splitObject(wallCells1[i],-splitIndex);
      SimPtr->removeCell(nextCell);
      }

   
    std::vector<int> liningCells1;



    



    try
      {
	liningCells1= Hatch->getCells("CornerB4C");
     
     for (size_t i=0;i<liningCells1.size(); i++)
      {
	//      int nextCell= SimPtr->splitObject(liningCells1[i],-splitIndex);
      SimPtr->removeCell(liningCells1[i]);
      };

	liningCells1= Hatch->getCells("RoofLining");
     
     for (size_t i=0;i<liningCells1.size(); i++)
      {
	//      int nextCell= SimPtr->splitObject(liningCells1[i],-splitIndex);
      SimPtr->removeCell(liningCells1[i]);
      };

     	liningCells1= Hatch->getCells("WallLining");
     
     for (size_t i=0;i<liningCells1.size(); i++)
      {
	//      int nextCell= SimPtr->splitObject(liningCells1[i],-splitIndex);
	SimPtr->removeCell(SimPtr->splitObject(liningCells1[i],-Hatch2->getLinkSurf(5)));
      };
      }catch(ColErr::InContainerError< std::string >){};
    

    
    try
      {
	liningCells1= Hatch->getCells("WallLining");
     
     for (size_t i=0;i<liningCells1.size(); i++)
      {
      int nextCell= SimPtr->splitObject(liningCells1[i],-splitIndex);
      SimPtr->removeCell(nextCell);
      };
      }catch(ColErr::InContainerError< std::string >){};


    
    
     /*  attachSystem::addToInsertSurfCtrl(*SimPtr,*Hatch,*UpperBlockRoof1);
attachSystem::addToInsertSurfCtrl(*SimPtr,*Hatch2,*UpperBlockRoof1);
attachSystem::addToInsertSurfCtrl(*SimPtr,*Hatch3,*UpperBlockRoof1);
     */

 splitIndex = UpperBlockRoof1->getLinkSurf(2);
    int voidTop2 =  SimPtr->splitObject(voidTop1,splitIndex);

     

  UpperBlockRoof2->addInsertCell(voidTop2);
    UpperBlockRoof2->createAll(*SimPtr,*UpperBlockRoof1,2);
    //  UpperBlockRoof2->createAll(*SimPtr,*UpperBlockWall,2);
    /*
   UpperCableRouteR->addInsertCell(voidTop2);
      UpperCableRouteR->createAll(*SimPtr,*UpperBlockRoof1,2);
      //  UpperCableRouteR->createAll(*SimPtr,*UpperBlockWall,2);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*UpperBlockRoof2,*UpperCableRouteR);

   UpperCableRouteL->addInsertCell(voidTop2);
    UpperCableRouteL->createAll(*SimPtr,*UpperBlockRoof1,2);
    //  UpperCableRouteL->createAll(*SimPtr,*UpperBlockWall,2);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*UpperBlockRoof2,*UpperCableRouteL);

    */
  //  UpperBlockHatch->setFront(*UpperBlockRoof,2);
  


     splitIndex = UpperBlockRoof2->getLinkSurf(2);
     int voidTop3 =  SimPtr->splitObject(voidTop2,splitIndex);

  
//  UpperBlockWall->createAll(*SimPtr,ACave->getKey("Outer"),6);
  UpperBlockHatch->addInsertCell(voidTop3);
  UpperBlockHatch->createAll(*SimPtr,*UpperBlockRoof2,2);
  UpperBlockHatch2->addInsertCell(voidTop1);
  UpperBlockHatch2->addInsertCell(voidTop2);
  UpperBlockHatch2->addInsertCell(voidTop3);
  UpperBlockHatch2->createAll(*SimPtr,*UpperBlockWall,2);


  
  //  RoofCableB4C1->addInsertCell(UpperBlockHatch->getCell("Void"));
  //  RoofCableB4C1->addInsertCell(UpperBlockRoof2->getCell("Void"));
  RoofCableB4C1->createAll(*SimPtr,*UpperBlockHatch,-2);
  SimPtr->removeCell(SimPtr->splitObject(UpperBlockHatch->getCell("Void"),
					 -RoofCableB4C1->getLinkSurf(4)));

  SimPtr->removeCell(SimPtr->splitObject(UpperBlockRoof2->getCell("Void"),
  					 -RoofCableB4C1->getLinkSurf(4)));
  RoofCableB4C2->createAll(*SimPtr,*UpperBlockHatch,-2);
  SimPtr->removeCell(SimPtr->splitObject(UpperBlockHatch->getCell("Void"),
					 -RoofCableB4C2->getLinkSurf(3)));
  //  RoofCableB4C1->addInsertCell(UpperBlockHatch->getCell("Void"));


  //    std::vector<int> liningCells1;

    try
      {
	liningCells1= UpperBlockHatch->getCells("WallLining");
     
     for (size_t i=0;i<liningCells1.size(); i++)
      {
	//      int nextCell= SimPtr->splitObject(liningCells1[i],-splitIndex);
      SimPtr->removeCell(liningCells1[i]);
      };
       SimPtr->removeCell(SimPtr->splitObject(UpperBlockHatch->getCell("RoofLining"),
					 -RoofCableB4C1->getLinkSurf(4)));
  SimPtr->removeCell(SimPtr->splitObject(UpperBlockHatch->getCell("RoofLining"),
					 -RoofCableB4C2->getLinkSurf(3)));

      }catch(ColErr::InContainerError< std::string >){};

        try
      {
	liningCells1= UpperBlockHatch->getCells("CornerB4C");
     
     for (size_t i=0;i<liningCells1.size(); i++)
      {
	//      int nextCell= SimPtr->splitObject(liningCells1[i],-splitIndex);
      SimPtr->removeCell(liningCells1[i]);
      };
      }catch(ColErr::InContainerError< std::string >){};

	

	
    try
      {
	liningCells1= UpperBlockRoof2->getCells("WallLining");
	//      int nextCell= SimPtr->splitObject(liningCells1[i],-splitIndex);
      SimPtr->removeCell(liningCells1[1]);
      
       SimPtr->removeCell(SimPtr->splitObject(UpperBlockRoof2->getCell("RoofLining"),
      				 -RoofCableB4C1->getLinkSurf(4)));
      }catch(ColErr::InContainerError< std::string >){};
	
        try
      {
	liningCells1= UpperBlockRoof2->getCells("CornerB4C");
      SimPtr->removeCell(liningCells1[1]);
      }catch(ColErr::InContainerError< std::string >){};



	
  splitIndex = UpperBlockHatch->getLinkSurf(2);
     int voidTop4 =  SimPtr->splitObject(voidTop3,splitIndex);

     //     std::cout << "4" << std::endl;
     //  Control.addVariable("BIFROSTCaveHatchRailsDefMat","Stainless304");

     HatchRails->addInsertCell(voidTop4);
    HatchRails->createAll(*SimPtr,*Hatch3,1);
     splitIndex = HatchRails->getLinkSurf(2);
     int voidTop5 =  SimPtr->splitObject(voidTop4,splitIndex);
     //   std::cout << "5" << std::endl;
    

  HatchLock->addInsertCell(voidTop3);
  HatchLock->addInsertCell(voidTop4);
  HatchLock->addInsertCell(voidTop5);
  HatchLock->addInsertCell(UpperBlockHatch->getCells("Void"));
 
  // HatchLock->addInsertCell(HatchRails->getCells("Void"));
  HatchLock->createAll(*SimPtr,ACave->getKey("Outer"),6);
 
   
  int voidRails=HatchRails->getCell("Void");
  
   

     
   HatchLock1->addInsertCell(voidRails);
    
   HatchLock1->addInsertCell(voidTop5);

   HatchLock1->createAll(*SimPtr,ACave->getKey("Outer"),6);


  splitIndex = HatchLock1->getLinkSurf(4);


  
  int voidRails1 = SimPtr->splitObject(voidRails,splitIndex);
   
   
 HatchLock2->addInsertCell(voidRails1);

 HatchLock2->addInsertCell(voidTop5);
 HatchLock2->createAll(*SimPtr,ACave->getKey("Outer"),6);
   
 HatchStreamBlocker->addInsertCell(voidRails);

 HatchStreamBlocker->createAll(*SimPtr,ACave->getKey("Outer"),6);
 

 
 

 /*
  LabyrinthPlate->addInsertCell(SampleLabyrinth->getCell("Void"));
  LabyrinthPlate->addInsertCell(SampleLabyrinth->getCell("FrontLining"));
  LabyrinthPlate->addInsertCell(UpperBlockWall->getCell("Void"));
  LabyrinthPlate->createAll(*SimPtr,*UpperBlockWall, 1);
 */
   
  BeamstopDoughnut->addInsertCell(voidFront);
  BeamstopDoughnut->createAll(*SimPtr,ACave->getKey("Outer"),2);
  
  
  BeamstopDoughnut2->addInsertCell(voidFront);
  BeamstopDoughnut2->createAll(*SimPtr,*BeamstopDoughnut,2);

  BeamstopPlug->addInsertCell(BeamstopDoughnut2->getCells("Void"));
  BeamstopPlug->createAll(*SimPtr,*BeamstopDoughnut2,-1);


  if(SimPtr->getDataBase().EvalVar<size_t>("ConstructBricks")>0){
    //   std::cout <<"Making bricks" <<std::endl;
  BrickShapeIn->setOffset(0.0,30.0,-20.0);
  BrickShapeIn->createAll(*SimPtr,ACave->getKey("Inner"),4);
  
    attachSystem::addToInsertSurfCtrl(*SimPtr,*ACave,*BrickShapeIn);
  // attachSystem::addToInsertForced(*SimPtr,*ACave,*BrickShapeIn);

  BrickShapeOut->setOffset(0.0,-30.0,-20.0);
  BrickShapeOut->createAll(*SimPtr,*ACave,0);
  
  attachSystem::addToInsertSurfCtrl(*SimPtr,*ACave,*BrickShapeOut);
  }
  
  //Setting Front and Rear surfaces of the CaveCut:
  //using LinkPoint groups named "Outer" and "Inner"
  //link point 1 is on the entry wall of the cave
  //CaveCut->addInsertCell(ACave->getCells("Walls"));
  //   CaveCut->setFaces(ACave->getKey("Outer").getFullRule(-1),
  //                 ACave->getKey("Inner").getFullRule(1));
  // CaveCut->createAll(System,*ShieldB,2);
  //  CaveCut->createAll(*SimPtr,ACave->getKey("Outer"),1);
    //std::cout << "done " << std::endl;
  // attachSystem::addToInsertForced(*SimPtr,*ACave,*CaveCut);


  InletCut->addInsertCell(ACave->getCells("RearWall"));  
  //  InletCut->addInsertCell(ACave->getCells("InnerSpace"));  
  InletCut->addInsertCell(CaveLining->getCells("RearWall"));  

   InletCut->setBack(ACave->getKey("Outer"),-1);
  InletCut->setFront(CaveLining->getKey("Inner"),1);
  //  InletCut->createAll(*SimPtr,ACave->getKey("Inner"),-1,ACave->getKey("Inner"),-1);
    InletCut->createAll(*SimPtr,CaveLining->getKey("Inner"),1);

    InletCut2->addInsertCell(InletCut->getCell("Void")); 
  //  InletCut->createAll(*SimPtr,ACave->getKey("Inner"),-1,ACave->getKey("Inner"),-1);
    InletCut2->createAll(*SimPtr,*InletCut,-1);


    
  CableLow1->addInsertCell(voidE01BaseL);
  CableLow1->addInsertCell(ACave->getCells("RearWall"));  
  //  CableLow->addInsertCell(ACave->getCells("InnerSpace"));  
  CableLow1->addInsertCell(CaveLining->getCells("RearWall"));  
  CableLow1->addInsertCell(CaveLining->getCells("Floor"));  

  //CableLow1->setBack(ACave->getKey("Outer"),-1);
  CableLow1->setFront(CaveLining->getKey("Inner"),1);
  //  CableLow->createAll(*SimPtr,ACave->getKey("Inner"),-1,ACave->getKey("Inner"),-1);
  CableLow1->createAll(*SimPtr,CaveLining->getKey("Inner"),1);
  CableLowBlock1->addInsertCell(voidE01BaseL);
    CableLowBlock1->createAll(*SimPtr,*CableLow1,2);
   CableLowBlock11->addInsertCell(voidE01BaseL);
    CableLowBlock11->createAll(*SimPtr,*CableLowBlock1,2);




  CableLow3->addInsertCell(ACave->getCells("RearWall"));  
  //  CableLow->addInsertCell(ACave->getCells("InnerSpace"));  
  CableLow3->addInsertCell(CaveLining->getCells("RearWall"));  
  CableLow3->addInsertCell(CaveLining->getCells("Floor"));  

  CableLow3->setBack(ACave->getKey("Outer"),-1);
  CableLow3->setFront(CaveLining->getKey("Inner"),1);
  //  CableLow->createAll(*SimPtr,ACave->getKey("Inner"),-1,ACave->getKey("Inner"),-1);
  CableLow3->createAll(*SimPtr,CaveLining->getKey("Inner"),1);

  CableLowBlock3->addInsertCell(voidE01BaseR);

  CableLowBlock3->createAll(*SimPtr,*CableLow3,2);

  std::vector<int> iReplace=CableLowBlock3->getCells("FloorLining");
  for (size_t i=0; i<iReplace.size();i++){
    SimPtr->findObject(iReplace[i])->setMaterial(60);
  SimPtr->setMaterialDensity(iReplace[i]);
  }
  if (iReplace.size()>3){
   iReplace=CableLowBlock3->getCells("WallLining");
   for (size_t i=0; i<iReplace.size()-3; i++){
    SimPtr->findObject(iReplace[i])->setMaterial(60);
  SimPtr->setMaterialDensity(iReplace[i]);
  }
   iReplace=CableLowBlock3->getCells("CornerB4C");
   for (size_t i=0;i<iReplace.size()-3;i++){
    SimPtr->findObject(iReplace[i])->setMaterial(60);
  SimPtr->setMaterialDensity(iReplace[i]);
  }
   iReplace=CableLowBlock3->getCells("RoofLining");
   for (size_t i=0;i<iReplace.size()-3;i++){
    SimPtr->findObject(iReplace[i])->setMaterial(60);
  SimPtr->setMaterialDensity(iReplace[i]);
  }
  }
    /*      ShieldGap1->addInsertCell(voidRear);
      ShieldGap1->createAll(*SimPtr,ACave->getKey("Outer"),1);

      ShieldGap2->addInsertCell(voidRear);
      ShieldGap2->addInsertCell(ShieldGap1->getCells("Void"));
      ShieldGap2->createAll(*SimPtr,ACave->getKey("Outer"),1);
    */

  //   attachSystem::addToInsertSurfCtrl(*SimPtr,*ACave,*InletCut);
    //    BeamstopCut->addInsertCell(ACave->getCells("RearWall")[0]);
    /*
    std::vector<int> iWall=ACave->getCells("FrontWall"); 
  std::cout << iWall.size() << std::endl;
  for (int i=0; i<iWall.size();i++){
    std::cout << iWall[i] << std::endl;
    BeamstopCut->addInsertCell(iWall[i]);
    }
    */
    BeamstopCut->setBack(CaveLining->getKey("Inner"),2);
    BeamstopCut->setFront(ACave->getKey("Outer"),-2);
    BeamstopCut->addInsertCell(ACave->getCells("FrontWall"));  
    //    BeamstopCut->addInsertCell(ACave->getCells("InnerSpace"));  
    BeamstopCut->addInsertCell(CaveLining->getCells("FrontWall"));  
  BeamstopCut->createAll(*SimPtr,ACave->getKey("Inner"),-2,ACave->getKey("Inner"),-2);
    CableLow2->setFront(CaveLining->getKey("Inner"),2);
    //CableLow2->setBack(ACave->getKey("Outer"),-2);
    CableLow2->addInsertCell(ACave->getCells("FrontWall"));  
    CableLow2->addInsertCell(voidFront);
    //    CableLow2->addInsertCell(ACave->getCells("InnerSpace"));  
    CableLow2->addInsertCell(CaveLining->getCells("FrontWall"));  
    CableLow2->addInsertCell(CaveLining->getCells("Floor"));  
    CableLow2->createAll(*SimPtr,CaveLining->getKey("Inner"),2);

    CableLowBlock2->addInsertCell(voidFront);
    CableLowBlock2->createAll(*SimPtr,*CableLow2,2);

    
    CableLowBlock21->addInsertCell(voidFront);
    CableLowBlock21->createAll(*SimPtr,*CableLowBlock2,2);


  BeamstopTshape->addInsertCell(voidFront);
  BeamstopTshape->addInsertCell(BeamstopCut->getCells("Guide0Void"));
  BeamstopTshape->addInsertCell(BeamstopDoughnut->getCells("Void"));

  //  BeamstopTshape->createAll(*SimPtr,ACave->getKey("Outer"),-2,ACave->getKey("Outer"),-2);
  BeamstopTshape->createAll(*SimPtr,ACave->getKey("Outer"),2);

  //  attachSystem::addToInsertSurfCtrl(*SimPtr,*BeamstopDoughnut2,*BeamstopTshape);


            Control.addVariable ("bifrostRoof0XYAngle",90);
    	   Control.addVariable ("bifrostRoof0PreZAngle",180);
     Control.addVariable ("bifrostRoof0ZStep",-400);
     Control.addVariable ("bifrostRoof0XStep",308);
      	  Control.addVariable ("bifrostRoof0YStep",90);
	  //  	  Control.addVariable ("bifrostRoof3YStep",70);
		  Control.addVariable ("bifrostRoof3Length1Edge",28);
		  Control.addVariable ("bifrostRoof3Length1Mid",38);

		
    std::array<std::shared_ptr<constructSystem::MicoRoof>,10> RoofSlabs;
      for (int i=0; i<10; i++){
        RoofSlabs[i]=
       std::shared_ptr<constructSystem::MicoRoof>
(new constructSystem::MicoRoof("bifrostRoof"+std::to_string(i)));
    OR.addObject ("bifrostRoof"+std::to_string(i),
		  RoofSlabs[i]);
    //       WallSlabs[i]->addInsertCell(ACave->getCell("LeftWall"));
   }
      //       RoofSlabs[0]->addInsertCell(ACave->getCell("RoofInner"));
        RoofSlabs[0]->setFront(ACave->getKey("Inner"),-1);
       RoofSlabs[0]->createAll(*SimPtr, *Hatch,1);
       RoofSlabs[1]->createAll(*SimPtr, *RoofSlabs[0],2);
      RoofSlabs[2]->setBack(*Hatch3,3);
       RoofSlabs[2]->createAll(*SimPtr, *RoofSlabs[1],2);

       RoofSlabs[3]->setFront(*Hatch3,-3);
       RoofSlabs[3]->createAll(*SimPtr, *RoofSlabs[2],2);


       // cutting inner roof 
       std::vector<int> irr=ACave->getCells("RoofInner");
       for (size_t i=0; i<irr.size();i++){
	 SimPtr->removeCell(SimPtr->splitObject(irr[i],-RoofSlabs[3]->getLinkSurf(2)));
       }

       for (int j=0; j< 4; j++){
       std::vector<int> irr=RoofSlabs[j]->getCells();
       for (size_t i=0; i<irr.size();i++){
	 SimPtr->removeCell(SimPtr->splitObject(irr[i],
						ACave->getKey("Inner").getLinkSurf(3)));
       }

       }




       for (int j=0; j< 3; j++){
       std::vector<int> irr=RoofSlabs[j]->getCells();
       for (size_t i=0; i<irr.size();i++){
	 SimPtr->removeCell(SimPtr->splitObject(irr[i],
						ACave->getKey("Inner").getLinkSurf(4)));
       }
       }


       irr=RoofSlabs[3]->getCells();
       for (size_t i=0; i<irr.size();i++){
	 SimPtr->removeCell(SimPtr->splitObject(irr[i],
						-Hatch3->getLinkSurf(6)));
       }




            Control.addVariable ("bifrostRoof4XYAngle",90);
	    //  Control.addVariable ("bifrostRoof0PreZAngle",180);
     Control.addVariable ("bifrostRoof4ZStep",-300);
     Control.addVariable ("bifrostRoof4XStep",-350);
      	  Control.addVariable ("bifrostRoof4YStep",0);
        	  Control.addVariable ("bifrostRoof5XStep",90);

		  Control.addVariable ("bifrostRoof9Length1Edge",28);
		  Control.addVariable ("bifrostRoof9Length1Mid",38);

      //       RoofSlabs[0]->addInsertCell(ACave->getCell("RoofInner"));
       RoofSlabs[4]->setBack(ACave->getKey("Inner"),-2);
       RoofSlabs[4]->createAll(*SimPtr, *Hatch,1);

      
       
       RoofSlabs[5]->createAll(*SimPtr, *RoofSlabs[4],1);
       // RoofSlabs[2]->setBack(*Hatch3,3);
       RoofSlabs[6]->createAll(*SimPtr, *RoofSlabs[5],2);
       RoofSlabs[7]->createAll(*SimPtr, *RoofSlabs[6],2);
        RoofSlabs[8]->setBack(*Hatch3,4);
       RoofSlabs[8]->createAll(*SimPtr, *RoofSlabs[7],2);
       RoofSlabs[9]->createAll(*SimPtr, *RoofSlabs[8],2);
       

       // cutting inner roof 
       irr=ACave->getCells("RoofInner");
       for (size_t i=0; i<irr.size();i++){
	 SimPtr->removeCell(SimPtr->splitObject(irr[i],-RoofSlabs[9]->getLinkSurf(2)));
       }
      
       for (int j=4; j< 10; j++){
       std::vector<int> irr=RoofSlabs[j]->getCells();
       for (size_t i=0; i<irr.size();i++){
	 SimPtr->removeCell(SimPtr->splitObject(irr[i],
						ACave->getKey("Inner").getLinkSurf(3)));
       }

       }




       for (int j=4; j< 9; j++){
       std::vector<int> irr=RoofSlabs[j]->getCells();
       for (size_t i=0; i<irr.size();i++){
	 SimPtr->removeCell(SimPtr->splitObject(irr[i],
						ACave->getKey("Inner").getLinkSurf(4)));
       }
       }
     

       irr=RoofSlabs[9]->getCells();
       for (size_t i=0; i<irr.size();i++){
	 SimPtr->removeCell(SimPtr->splitObject(irr[i],
						-Hatch3->getLinkSurf(6)));
       }


       
              

  ACave->layerProcess(*SimPtr);
  

  
    // Elliptic 6m section
    VPipeCave->addInsertCell(CaveLining->getCell("InnerSpace"));
    VPipeCave->addInsertCell(InletCut->getCells("Void"));
    VPipeCave->addInsertCell(InletCut2->getCells("Void"));
    VPipeCave->setBack(ACave->getKey("Outer"),-1);
    VPipeCave->createAll(*SimPtr,ACave->getKey("Inner"),0);

   
     splitIndex = VPipeCave->getLinkSurf(1);

   int voidCave1 =  SimPtr->splitObject(CaveLining->getCell("InnerSpace"),splitIndex);

     //    VBeamstopPipe->addInsertCell(ACave->getCell("InnerSpace"));
    VGetLostPipe->addInsertCell(voidCave1);
    //    VGetLostPipe->addInsertCell(BeamstopCut->getCells("Guide0Void"));
    VGetLostPipe->createAll(*SimPtr,ACave->getKey("Inner"),0);
     splitIndex = VGetLostPipe->getLinkSurf(2);
  int voidCave2= SimPtr->splitObject(voidCave1,splitIndex);

    VBeamstopPipe->addInsertCell(voidCave2);
    VBeamstopPipe->addInsertCell(BeamstopCut->getCells("Guide0Void"));
    VBeamstopPipe->createAll(*SimPtr,*VGetLostPipe,2);

    /*
      BeamstopInnerDoughnut->addInsertCell(BeamstopCut->getCells("Guide0Void"));
      BeamstopInnerDoughnut->createAll(*SimPtr,*VBeamstopPipe,2,*VBeamstopPipe,2);
  
    */
    
    //  Beamstop->addInsertCell(BeamstopTshape->getCells("Guide0Void"));
  Beamstop->addInsertCell(BeamstopCut->getCells("Guide0Void"));
  Beamstop->createAll(*SimPtr,ACave->getKey("Inner"),2);

 
  //  attachSystem::addToInsertSurfCtrl(*SimPtr,*ACave,*BeamstopCut);

  
  //  attachSystem::addToInsertLineCtrl(*SimPtr,*ACave,*VPipeCave);
  //  attachSystem::addToInsertSurfCtrl(*SimPtr,*CaveCut,*VPipeCave);

   if( SimPtr->getDataBase().EvalVar<size_t>("ConstructOptics")>0){
   
  FocusCave1->addInsertCell(VPipeCave->getCells("Void"));
  Slits->addInsertCell(VPipeCave->getCells("Void"));
  FocusCave2->addInsertCell(VPipeCave->getCells("Void"));
  FocusCave1->createAll(*SimPtr,EllFocus[nEllSection-1]->getKey("Guide0"),2,
			EllFocus[nEllSection-1]->getKey("Guide0"),2);
  Slits->createAll(*SimPtr,FocusCave1->getKey("Guide0"),2);
  FocusCave2->createAll(*SimPtr,FocusCave1->getKey("Guide0"),2,
			FocusCave1->getKey("Guide0"),2);
   }

   if( SimPtr->getDataBase().EvalVar<std::string>("SampleEnvironment")=="Magnet"){
  AMagnetBase->addInsertCell(voidCave1);
  AMagnetBase->createAll(*SimPtr,*ACave,0);
  ASample->addInsertCell(voidCave1);
  //Doing it like above (inserting to the surrounding volume) requires cutting the       sample holder out of the MagnetBase after createAll 
  //ASample->addInsertCell(AMagnetBase->getCell("InnerSpace"));
  //Doing as above ^^^^ will cut out the sample holder only from the cell "InnerSpace"
  //But will do the cutting out
   ASample->createAll(*SimPtr,*AMagnetBase,0);
   //Arguments for createAll: Simulation, FixedComponent, LinkPointNo of the FixedCompone nt
    attachSystem::addToInsertLineCtrl(*SimPtr,*AMagnetBase,*ASample);
  
   }
   else
     {
   SampleSheet->addInsertCell(voidCave1);
  SampleSheet->createAll(*SimPtr,*ACave,0);
   }
     
      CavePlatformB4C->addInsertCell(CaveLining->getCell("InnerSpace"));
      CavePlatformB4C->addInsertCell(voidCave1);
  
    CavePlatformB4C->createAll(*SimPtr,*ACave,0);
    
    CavePlatform->createAll(*SimPtr,*ACave,0);

    SimPtr->removeCell(SimPtr->splitObject(
		      SimPtr->splitObject(CavePlatformB4C->getCell("Void"),
					  -CavePlatform->getLinkSurf(2)),
		      -CavePlatform->getLinkSurf(1)));


   CavePlatformFence->addInsertCell(CaveLining->getCell("InnerSpace"));
   CavePlatformFence->addInsertCell(voidCave1);
   CavePlatformFence->createAll(*SimPtr,*CavePlatformB4C,2);

   std::vector<int> iVoid=CavePlatformFence->getCells("Roof");
	for(int i=0;i<iVoid.size();i++)	SimPtr->removeCell(iVoid[i]);
        iVoid=CavePlatformFence->getCells("Corner");
	for(int i=0;i<iVoid.size();i++)	SimPtr->removeCell(iVoid[i]);


       
	
  Control.addVariable ("bifrostWall0XYAngle",90);
  Control.addVariable ("bifrostWall0YStep",60);
  Control.addVariable ("bifrostWall0XStep",120+25);
  Control.addVariable ("bifrostWall0ZStep",3.5+50+120+2.8);
  Control.addVariable("bifrostWall0HeightUp",49.5+10-55);
  Control.addVariable("bifrostWall0HeightLow",50.5+10+55);
  
  
     Control.addVariable ("bifrostWall1ZAngle",180);
     Control.addVariable ("bifrostWall1ZStep",-240);
  Control.addVariable("bifrostWall1HeightUp",49.5+10+55);
  Control.addVariable("bifrostWall1HeightLow",50.5+10-55);

  Control.addVariable ("bifrostWall2ZAngle",180);
  Control.addVariable ("bifrostWall2YStep",120);
   Control.addVariable ("bifrostWall2ZStep",-120);
  Control.addVariable("bifrostWall2HeightUp",49.5+10+55);
  Control.addVariable("bifrostWall2HeightLow",50.5+10-55);
     
     Control.addVariable ("bifrostWall3ZAngle",180);
     //       Control.addVariable ("bifrostWall3YStep",125);
     //   Control.addVariable ("bifrostWall3ZStep",120);
  Control.addVariable("bifrostWall3HeightUp",49.5+10-55);
  Control.addVariable("bifrostWall3HeightLow",50.5+10+55);


  Control.addVariable ("bifrostWall4XYAngle",90);
  Control.addVariable ("bifrostWall4YStep",60);
 Control.addVariable ("bifrostWall4XStep",240+120+25);
  Control.addVariable ("bifrostWall4ZStep",3.5+50+120+2.8);
  Control.addVariable("bifrostWall4HeightUp",49.5+10-55);
  Control.addVariable("bifrostWall4HeightLow",50.5+10+55);

     Control.addVariable ("bifrostWall5ZAngle",180);
          Control.addVariable ("bifrostWall5ZStep",-240);
  Control.addVariable("bifrostWall5HeightUp",49.5+10+55);
  Control.addVariable("bifrostWall5HeightLow",50.5+10-55);

  Control.addVariable ("bifrostWall6ZAngle",180);
  Control.addVariable ("bifrostWall6YStep",120);
  Control.addVariable ("bifrostWall6Length1Mid",115.5);
   Control.addVariable ("bifrostWall6ZStep",-120);
  Control.addVariable("bifrostWall6HeightUp",49.5+10+55);
  Control.addVariable("bifrostWall6HeightLow",50.5+10-55);
     
   //     Control.addVariable ("bifrostWall7ZAngle",180);
                Control.addVariable ("bifrostWall7YStep",-240);
     //    Control.addVariable ("bifrostWall7ZStep",-230);
  Control.addVariable("bifrostWall7HeightUp",49.5+10-55);
  Control.addVariable("bifrostWall7HeightLow",50.5+10+55);



       Control.addVariable ("bifrostWall8ZAngle",180);
     //       Control.addVariable ("bifrostWall3YStep",125);
        Control.addVariable ("bifrostWall8ZStep",-120);
  Control.addVariable("bifrostWall8HeightUp",49.5+10-55);
  Control.addVariable("bifrostWall8HeightLow",50.5+10+55);

  //  Control.addVariable ("bifrostWall8ZAngle",180);
  //          Control.addVariable ("bifrostWall9YStep",120);
       Control.addVariable ("bifrostWall9ZStep",-120);
  Control.addVariable("bifrostWall9HeightUp",49.5+10+55);
  Control.addVariable("bifrostWall9HeightLow",50.5+10-55);



  
       Control.addVariable ("bifrostWall10ZAngle",180);
     //       Control.addVariable ("bifrostWall3YStep",125);
        Control.addVariable ("bifrostWall10ZStep",-120);
  Control.addVariable("bifrostWall10HeightUp",49.5+10-55);
  Control.addVariable("bifrostWall10HeightLow",50.5+10+55);

    Control.addVariable ("bifrostWall11ZAngle",180);
  //          Control.addVariable ("bifrostWall9YStep",120);
       Control.addVariable ("bifrostWall11ZStep",-120);
  Control.addVariable("bifrostWall11HeightUp",49.5+10+55);
  Control.addVariable("bifrostWall11HeightLow",50.5+10-55);






    Control.addVariable ("bifrostWall12ZAngle",180);
     //       Control.addVariable ("bifrostWall3YStep",125);
        Control.addVariable ("bifrostWall12ZStep",-120);
  Control.addVariable("bifrostWall12HeightUp",49.5+10-55);
  Control.addVariable("bifrostWall12HeightLow",50.5+10+55);
  Control.addVariable ("bifrostWall12Length1Mid",107.5);
  Control.addVariable ("bifrostWall12Length1Edge",107.5);
 Control.addVariable ("bifrostWall12Length1Low",106.5);

  //  Control.addVariable ("bifrostWall8ZAngle",180);
  //          Control.addVariable ("bifrostWall9YStep",120);
       Control.addVariable ("bifrostWall13ZStep",-120);
  Control.addVariable("bifrostWall13HeightUp",49.5+10+55);
  Control.addVariable("bifrostWall13HeightLow",50.5+10-55);
  Control.addVariable ("bifrostWall13Length1Mid",107.5);
  Control.addVariable ("bifrostWall13Length1Edge",107.5);
 Control.addVariable ("bifrostWall13Length1Low",106.5);



  
  
     int iLayers=Control.EvalDefVar("LayerFlag",1);

    if ((0)&&(not(iLayers))){ 

      std::array<std::shared_ptr<constructSystem::CaveMicoBlock>,14> WallSlabs;
      for (int i=0; i<14; i++){
        WallSlabs[i]=
       std::shared_ptr<constructSystem::CaveMicoBlock>
(new constructSystem::CaveMicoBlock("bifrostWall"+std::to_string(i)));
    OR.addObject ("bifrostWall"+std::to_string(i),
		  WallSlabs[i]);
    //       WallSlabs[i]->addInsertCell(ACave->getCell("LeftWall"));
   }
      

         WallSlabs[0]->createAll(*SimPtr,ACave->getKey("Inner"),3);
       WallSlabs[1]->createAll(*SimPtr,*WallSlabs[0],1);
       WallSlabs[2]->createAll(*SimPtr,*WallSlabs[0],1);
        WallSlabs[3]->createAll(*SimPtr,*WallSlabs[2],1);
	//std::cout <<"4" <<std::endl;

      WallSlabs[4]->createAll(*SimPtr,ACave->getKey("Inner"),3);

          WallSlabs[5]->createAll(*SimPtr,*WallSlabs[4],1);
    WallSlabs[6]->setFront(ACave->getKey("Outer"), -2);
    WallSlabs[6]->createAll(*SimPtr,*WallSlabs[4],1);
        WallSlabs[7]->setFront(ACave->getKey("Outer"), -2);
         WallSlabs[7]->createAll(*SimPtr,*WallSlabs[3],-1);
         WallSlabs[8]->createAll(*SimPtr,*WallSlabs[1],-2);
         WallSlabs[9]->createAll(*SimPtr,*WallSlabs[1],2);
         WallSlabs[10]->createAll(*SimPtr,*WallSlabs[9],-2);
         WallSlabs[11]->createAll(*SimPtr,*WallSlabs[8],-2);
	 WallSlabs[12]->setBack(ACave->getKey("Outer"), -1);
	 
         WallSlabs[13]->setBack(ACave->getKey("Outer"), -1);
	 
	 WallSlabs[12]->createAll(*SimPtr,*WallSlabs[11],-2);
         WallSlabs[13]->createAll(*SimPtr,*WallSlabs[11],2);
  SimPtr->removeCell(SimPtr->splitObject(ACave->getCell("LeftWall"),
					- WallSlabs[12]->getLinkSurf(5)));
  SimPtr->removeCell(SimPtr->splitObject(ACave->getCell("CornerD"),
					- WallSlabs[12]->getLinkSurf(5)));
  SimPtr->removeCell(SimPtr->splitObject(ACave->getCell("CornerC"),
					- WallSlabs[12]->getLinkSurf(5)));
 
    }
       
    if (iLayers){

    //B4C to air replacement in some cells
    irr = UpperBlockWall->getCells("WallLining");
    SimPtr->findObject(irr[0])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[0]);
    SimPtr->findObject(irr[2])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[2]);
    SimPtr->findObject(irr[4])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[4]);
    SimPtr->findObject(irr[6])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[6]);

    irr = UpperBlockWall->getCells("RoofLining");
    SimPtr->findObject(irr[0])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[0]);
    SimPtr->findObject(irr[1])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[1]);
    SimPtr->findObject(irr[2])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[2]);
    SimPtr->findObject(irr[3])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[3]);
  
    irr = Hatch->getCells("WallLining");
    for (size_t i =0; i<irr.size(); i+=2){
    SimPtr->findObject(irr[i])->setMaterial(60);
    SimPtr->setMaterialDensity(irr[i]);}

  irr = UpperBlockRoof1->getCells("WallLining");
    SimPtr->findObject(irr[0])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[0]);
  irr = UpperBlockRoof1->getCells("RoofLining");
    SimPtr->findObject(irr[0])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[0]);

  irr = UpperBlockRoof2->getCells("WallLining");
    SimPtr->findObject(irr[0])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[0]);
  irr = UpperBlockRoof2->getCells("RoofLining");
    SimPtr->findObject(irr[0])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[0]);

  irr = UpperBlockHatch->getCells("RoofLining");
    SimPtr->findObject(irr[0])->setMaterial(60);
  SimPtr->setMaterialDensity(irr[0]);
  

  SimPtr->findObject(SampleLabyrinthB4C->getCells("Roof")[0])->setMaterial(60);
  SimPtr->setMaterialDensity(SampleLabyrinthB4C->getCells("Roof")[0]);


    }

  
    if( SimPtr->getDataBase().EvalVar<size_t>("ConstructDetectorTank")>0){

    Detector->addInsertCell(CaveLining->getCell("InnerSpace"));
    Detector->addInsertCell(voidCave1);

    if( SimPtr->getDataBase().EvalVar<std::string>("SampleEnvironment")=="Magnet"){
    Detector->createAll(*SimPtr,*AMagnetBase,0);
    //    Collimator->addInsertCell(Detector->getCell("DetectorCutOut"));
    Collimator->createAll(*SimPtr,*AMagnetBase,0);
    }else{
    Detector->createAll(*SimPtr,*SampleSheet,0);
    //    Collimator->addInsertCell(Detector->getCell("DetectorCutOut"));
    Collimator->createAll(*SimPtr,*SampleSheet,0);
    }
    attachSystem::addToInsertSurfCtrl(*SimPtr,*Detector,*Collimator);
    }
    return;
}


  
  
void
BIFROST::buildOutGuide(Simulation& System,
		       //		       const attachSystem::FixedComp& FTA,
		       //                       const long int thermalIndex,
		       //		       const CompBInsert& BInsert,
		       const Bunker& bunkerObj,
		       const int voidCell)
  /*!
Build all the components between bunker and cave
    \param System :: simulation                    
    \param FTA :: FixedComp for thermal guide      
    \param thermalIndex :: FixedComp link point for thermal
    \param BInsert :: Bunker insert which has to be excluded from shielding volume 
    \param Bunker :: Bunker object to link the gap to
    \param voidCell :: cell to place objects in            

  */
{
    ELog::RegMethod RegA("BIFROST","buildOutGuide");

  /* Temporary fix 
  // First put pit into the main void
  OutPitA->addInsertCell(voidCell);
  OutPitA->createAll(System,FocusWall->getKey("Shield"),2);
  */


   // Gap between lateral concrete shielding and bunker wall
    //   ShieldGap->addInsertCell(voidCell);
    //   ShieldGap->setFront(bunkerObj,2);
     //    ShieldGap->createAll(System,*WallCut,1);
     // attachSystem::addToInsertSurfCtrl(System,*ShieldGap,"Void",*BInsert);  

  

   ShieldGap1->addInsertCell(voidCell);
   ShieldGap2->addInsertCell(voidCell);

   
      ShieldGap1->setFront(bunkerObj,2);
      //  ShieldGap2->setFront(bunkerObj,2);
 

   ShieldGap1->createAll(System,FocusWall2->getKey("Guide0"),2);
   ShieldGap2->addInsertCell(ShieldGap1->getCells("Void"));

   ShieldGap2->createAll(System,FocusWall2->getKey("Guide0"),2);


   
   //   InnerShieldGap1->setFront(*ShieldGap1,-1);
   //   InnerShieldGap1->setBack(bunkerObj,-2);

   InnerShieldGap2->setFront(*ShieldGap2,-1);
   InnerShieldGap2->setBack(*ShieldGap2,-2);
 
   //   InnerShieldGap1->createAll(System,FocusWall2->getKey("Guide0"),2);
   InnerShieldGap2->addInsertCell(ShieldGap2->getCell("Void"));
   InnerShieldGap2->createAll(System,*ShieldGap2,-1);

  
   
   //   attachSystem::addToInsertSurfCtrl(System,*ShieldGap1,*InnerShieldGap1);  
    attachSystem::addToInsertSurfCtrl(System,*ShieldGap2,*InnerShieldGap2);  

 

   

    //  ShieldA->createAll(System,*BInsert,2);
    
    //      attachSystem::addToInsertSurfCtrl(System,*ShieldGap1,"Void",*TPipeWall3);  


   int   splitIndex= ShieldGap2->getLinkSurf(2);
 
   int   voidCellThis=voidCell;
   int   voidCellNext =  System.splitObject(voidCellThis,splitIndex);


  
   ShieldA->addInsertCell(voidCellNext);
   // ShieldA->addInsertCell(WallCut->getCells("Guide0Void"));
   //  ShieldA->setFront(*ShieldGap,2);
  //  ShieldA->addInsertCell(OutPitA->getCells("Outer"));
  // ShieldA->addInsertCell(OutPitA->getCells("MidLayer"));
  // ShieldA->setBack(OutPitA->getKey("Mid"),1);
      ShieldA->createAll(System,*ShieldGap2,2);
   //  ShieldA->createAll(System,*BInsert,2);
    

      splitIndex= ShieldA->getLinkSurf(2);
 
      voidCellThis=voidCellNext;
      voidCellNext =  System.splitObject(voidCellThis,splitIndex);
    

   ShutterFrontWall->addInsertCell(voidCellNext);
    ShutterFrontWall->createAll(System,*ShieldA,2);
    

    /*ShutterFrontB4C->createAll(System,*ShutterFrontWall, -2);
    attachSystem::addToInsertSurfCtrl(System,*ShutterFrontHorseshoe,*ShutterFrontB4C);
    */
         splitIndex= ShutterFrontWall->getLinkSurf(2);
 
      voidCellThis=voidCellNext;
      voidCellNext =  System.splitObject(voidCellThis,splitIndex);



   ShutterExpansion->addInsertCell(voidCellNext);
    ShutterExpansion->createAll(System,*ShutterFrontWall,2);

      splitIndex= ShutterExpansion->getLinkSurf(2);
 
      voidCellThis=voidCellNext;
      voidCellNext =  System.splitObject(voidCellThis,splitIndex);


    ShutterRearWall->addInsertCell(voidCellNext); 
    ShutterRearWall->createAll(System,*ShutterExpansion,2);

      splitIndex= ShutterRearWall->getLinkSurf(2);
 
      voidCellThis=voidCellNext;
      voidCellNext =  System.splitObject(voidCellThis,splitIndex);
    

   InnerShieldA->setFront(*ShieldA,-1);
   InnerShieldA->setBack(*ShutterFrontWall,-2);
 

    InnerShieldA->createAll(System,*ShieldGap2,2);
    attachSystem::addToInsertSurfCtrl(System,*ShieldA,*InnerShieldA);  
    attachSystem::addToInsertSurfCtrl(System,*ShutterFrontWall,*InnerShieldA);  
  


   ShutterInnerShield->setFront(*ShutterExpansion,-1);
   ShutterInnerShield->setBack(*ShutterExpansion,-2);
 

    ShutterInnerShield->createAll(System,*ShutterFrontWall,2);
    attachSystem::addToInsertSurfCtrl(System,*ShutterExpansion,*ShutterInnerShield);  


    //    ShutterFrontHorseshoe->setFront(*ShutterFrontWall, 2);
    ShutterFrontHorseshoe->createAll(System,*ShutterInnerShield, -1);
    attachSystem::addToInsertSurfCtrl(System,*ShutterInnerShield,*ShutterFrontHorseshoe);


    ShutterRearHorseshoe->createAll(System,*ShutterInnerShield, -2);
    attachSystem::addToInsertSurfCtrl(System,*ShutterInnerShield,*ShutterRearHorseshoe);

    
  
   /*Original:
   // Gap between lateral concrete shielding and bunker wall
   ShieldGap->addInsertCell(voidCell);
   ShieldGap->setFront(bunkerObj,2);
   ShieldGap->createAll(System,FocusWall->getKey("Guide0"),2);
   attachSystem::addToInsertSurfCtrl(System,*ShieldGap,"Void",*BInsert);  
    
  
  ShieldA->addInsertCell(voidCell);
  ShieldA->setFront(*BInsert,2);
  ShieldA->addInsertCell(OutPitA->getCells("Outer"));
  ShieldA->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldA->setBack(OutPitA->getKey("Mid"),1);
  ShieldA->createAll(System,FocusWall->getKey("Shield"),2);
  */
     

   
  // Elliptic 25 -3.6m section
  VPipeOutA->addInsertCell(InnerShieldGap2->getCell("Void"));
  VPipeOutA->addInsertCell(InnerShieldA->getCell("Void"));
  //  VPipeOutA->addInsertCell(ShutterInnerShield->getCell("Void"));
  VPipeOutA->createAll(System,FocusWall2->getKey("Guide0"),2);

  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

    
  //     splitIndex= FocusOutA->getKey("Guide0").getLinkSurf(-2);

  splitIndex = VPipeOutA->getLinkSurf(2);
  int voidThis=InnerShieldA->getCell("Void");
    int voidNext =  System.splitObject(voidThis,splitIndex);

  
 
  
  // Elliptic 25-3.6m section
    //  VPipeOutB->addInsertCell(ShieldA->getCell("Void"));
     VPipeOutB->addInsertCell(voidNext);
  VPipeOutB->createAll(System,FocusOutA->getKey("Guide0"),2);

  FocusOutB->addInsertCell(VPipeOutB->getCells("Void"));
  FocusOutB->createAll(System,*VPipeOutB,0,*VPipeOutB,0);
  /*
  // Elliptic 25-3.6 m section
  VPipeOutC->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutC->createAll(System,FocusOutB->getKey("Guide0"),2);

  FocusOutC->addInsertCell(VPipeOutC->getCells("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0,*VPipeOutC,0);
  */
  
  // Elliptic 25-3.6 m section, last part
  // VPipeOutC->addInsertCell(ShieldA->getCell("Void"));
   VPipeOutC->addInsertCell(voidNext);
   VPipeOutC->addInsertCell(ShutterInnerShield->getCells("Void"));
   VPipeOutC->addInsertCell(ShutterFrontHorseshoe->getCells("Void"));
  VPipeOutC->createAll(System,FocusOutB->getKey("Guide0"),2);

  
  FocusOutC->addInsertCell(VPipeOutC->getCells("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0,*VPipeOutC,0);

  
  
   splitIndex = VPipeOutC->getLinkSurf(2);
   voidThis=ShutterInnerShield->getCell("Void");
   voidNext =  System.splitObject(voidThis,splitIndex);

  ShutterAFrame->addInsertCell(voidNext);
  ShutterAFrame->createAll(System,FocusOutC->getKey("Guide0"),2);

  ShutterAB4C->addInsertCell(voidNext);
  
  ShutterAB4C->createAll(System,*ShutterAFrame,0);
  attachSystem::addToInsertSurfCtrl(System,*ShutterAFrame,*ShutterAB4C);  

  
  ShieldB->addInsertCell(voidNext);
  ShieldB->setFront(*ShutterRearWall,2);
  ShieldB->addInsertCell(voidCellNext);
  ShieldB->createAll(System,*ShutterRearWall,2);
  InnerShieldB->setFront(*ShutterRearWall,-1);
  InnerShieldB->setBack(*ShieldB,-2);
  InnerShieldB->addInsertCell(ShutterRearWall->getCells("Void"));
  InnerShieldB->addInsertCell(ShieldB->getCells("Void"));
  InnerShieldB->createAll(System,*ShutterRearWall,-1);
 

  
    return;

  
  const attachSystem::FixedComp* LinkPtr= &FocusOutC->getKey("Guide0");
  for(size_t i=0;i<nGuideSection;i++)
    {
      // Elliptic 6m section
      RecPipe[i]->addInsertCell(ShieldA->getCell("Void"));
      RecPipe[i]->createAll(System,*LinkPtr,2);
      
      RecFocus[i]->addInsertCell(RecPipe[i]->getCells("Void"));
      RecFocus[i]->createAll(System,*RecPipe[i],0,*RecPipe[i],0);

      LinkPtr= &RecFocus[i]->getKey("Guide0");
    }
  
    return;
  
  OutACutFront->addInsertCell(OutPitA->getCells("MidLayerFront"));
  OutACutFront->setFaces(OutPitA->getKey("Mid").getFullRule(-1),
                         OutPitA->getKey("Inner").getFullRule(1));
  OutACutFront->createAll(System,OutPitA->getKey("Inner"),-1);

  OutACutBack->addInsertCell(OutPitA->getCells("MidLayerBack"));
  OutACutBack->addInsertCell(OutPitA->getCells("Collet"));
  OutACutBack->setFaces(OutPitA->getKey("Inner").getFullRule(2),
                        OutPitA->getKey("Mid").getFullRule(-2));
  OutACutBack->createAll(System,OutPitA->getKey("Inner"),2);
  

  // FOC chopper out of bunker
  ChopperOutA->addInsertCell(OutPitA->getCell("Void"));
  ChopperOutA->createAll(System,*LinkPtr,2);    // copied from last of
                                                // guide array
  // Single disk FOC-OutA
  FOCDiskOutA->addInsertCell(ChopperOutA->getCell("Void"));
  FOCDiskOutA->setOffsetFlag(1);  // Z direction
  FOCDiskOutA->createAll(System,ChopperOutA->getKey("Main"),0);
  ChopperOutA->insertAxle(System,*FOCDiskOutA);
  /*
  ShieldB->addInsertCell(voidCell);
  ShieldB->setFront(OutPitA->getKey("Mid"),2);
  ShieldB->addInsertCell(OutPitA->getCells("Outer"));
  ShieldB->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldB->createAll(System,*LinkPtr,2);
  */
  LinkPtr= &ChopperOutA->getKey("Beam");

  for(size_t i=0;i<nSndSection;i++)
    {
      // Rectangle 6m section
      SndPipe[i]->addInsertCell(ShieldB->getCell("Void"));
      SndPipe[i]->createAll(System, *LinkPtr, 2);     

      SndFocus[i]->addInsertCell(SndPipe[i]->getCells("Void"));
      SndFocus[i]->createAll(System,*SndPipe[i],0,*SndPipe[i],0);

      LinkPtr= &SndFocus[i]->getKey("Guide0");
    }

  for(size_t i=0;i<nEllSection;i++)
    {
      // Elliptic 6m sections
      EllPipe[i]->addInsertCell(ShieldB->getCell("Void"));
      EllPipe[i]->createAll(System,*LinkPtr,2);
      
      EllFocus[i]->addInsertCell(EllPipe[i]->getCells("Void"));
      EllFocus[i]->createAll(System,*EllPipe[i],0,*EllPipe[i],0);

      LinkPtr= &EllFocus[i]->getKey("Guide0");
    }

  Cave->addInsertCell(voidCell);
  Cave->createAll(System,*ShieldB,2);

  ShieldB->addInsertCell(Cave->getCells("FrontWall"));
  ShieldB->insertObjects(System);

  CaveCut->addInsertCell(Cave->getCells("IronFront"));
  CaveCut->setFaces(Cave->getKey("Mid").getFullRule(-1),
                    Cave->getKey("Inner").getFullRule(1));
  CaveCut->createAll(System,*ShieldB,2);

  // Elliptic 6m section
  VPipeCave->addInsertCell(Cave->getCell("Void"));
  VPipeCave->addInsertCell(CaveCut->getCell("Void"));
  VPipeCave->addInsertCell(ShieldB->getCell("Void"));
 
  //  VPipeBAave->createAll(System,*CaveCut,2);
  VPipeCave->createAll(System,*LinkPtr,2);

  FocusCave1->addInsertCell(VPipeCave->getCells("Void"));
  FocusCave1->createAll(System,*VPipeCave,0,*VPipeCave,0);

  /* Commenting out sample environment for now
  SampleMagnetBase->addInsertCell(Cave->getCell("Void"));
  SampleMagnetBase->createAll(System,FocusCave->getKey("Guide0"),2);
  ASample->addInsertCell(Cave->getCell("Void"));
  //Doing it like above (inserting to the surrounding volume) requires cutting out the sample holder out of the MagnetBase after createAll
  //ASample->addInsertCell(SampleMagnetBase->getCell("InnerSpace"));
  //Doing as above ^^^^ will cut out the sample holder only from the cell "InnerSpace"
  //But will do the cutting out
  ASample->createAll(System,*SampleMagnetBase,0);
  //Arguments for createAll: Simulation, FixedComponent, LinkPointNo of the FixedCompon
  attachSystem::addToInsertControl(System,*SampleMagnetBase,*ASample);
  */
  
  return; 
}



int
BIFROST::buildBunkerUnits(Simulation& System,
                        const attachSystem::FixedComp& FA,
                        const long int guideIndex,
                        const int bunkerVoid)
  /*!
    Build all the components in the bunker space
    \param System :: simulation
    \param FTA :: FixedComp for thermal guide
    \param thermalIndex :: FixedComp link point for thermal
    \param bunkerVoid :: cell to place objects in
   */
{
  ELog::RegMethod RegA("BIFROST","buildBunkerUnits");



  
  /**Ugly way of cutting a sector out of the bunker void**/
  
 ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

 /* //Code doesn't work, getObject removed from the ObjectRegister definition 
 attachSystem::FixedComp* CPtr=OR.getObject<attachSystem::FixedComp>("ABunkerWall");
  const BunkerWall* WllPtr= dynamic_cast<const BunkerWall*>(CPtr);
 if (!WllPtr)
   throw ColErr::InContainerError<std::string>
     (WllPtr->getKeyName(),"Component not found");
      
 std::vector<int> sIndex=WllPtr->getCellsHR(System,"Sector4").getSurfaceNumbers();
 std::vector<int> dIndex=WllPtr->getCellsHR(System,"Sector3").getSurfaceNumbers();
 
    int   bunkerVoidNext=
   System.splitObject(bunkerVoid,sIndex[3]);
    int bunkerVoidNext2 =
   System.splitObject(bunkerVoid,-dIndex[2]);
 */
  
 int bunkerVoidThis=bunkerVoid, bunkerVoidNext;
 
   //Light shutter, VPipeB + FocusB  
    // VPipeB->addInsertCell(bunkerVoid);
  //  VPipeB->createAll(System,GItem.getKey("Beam"),2);
  //  VPipeB->createAll(System,FA->getKey("Guide0"),guideIndex);
    VPipeB->createAll(System,FA,guideIndex);

   int   splitIndex = VPipeB->getLinkSurf(1);

   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,splitIndex);
//VPipeB->addInsertCell(bunkerVoidNext);
    VPipeB->addInsertCell(bunkerVoidThis); 
    attachSystem::addToInsertForced(System,std::vector<int>{bunkerVoidNext},*VPipeB);
  // Offset from VPipeB center

   splitIndex = VPipeB->getLinkSurf(2);
   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,splitIndex);

  
   // FocusB->addInsertCell(VPipeB->getCells("Void"));
  //Need to find surfaces defining the Void cell in the VPipeB
  //Give them as arguments to the setFront and setBack methods
  /*
  //Some ugly hard-coded stuff, works for round pipe, crashes for rect pipe
  std::vector<int> vacIndex = VPipeB->getSurfs();
  FocusB->setFront(vacIndex[1]);
  FocusB->setBack(-vacIndex[3]);
  //  FocusB->createAll(System,*VPipeB,0,*VPipeB,0);
  */
  //Works for rect pipe, copied from heimdal
  FocusB->setFront(*VPipeB,-1);
  FocusB->setBack(*VPipeB,-2);
  FocusB->addInsertCell(VPipeB->getCells("Void"));
  
  // FocusB->createAll(System,FA,guideIndex,FA,guideIndex);//Copied from Heimdal
  
  FocusB->createAll(System,*bifrostAxis,-3,*bifrostAxis,-3); 

  

  
 
     VPipeC->addInsertCell(bunkerVoidNext);
     VPipeC->createAll(System,FocusB->getKey("Guide0"),2);
        
  
  //Some ugly hard-coded stuff, works for round pipe, crashes for rect pipe
  //  FocusB->createAll(System,*VPipeB,0,*VPipeB,0);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->setFront(-VPipeC->getLinkSurf(1));
  
    FocusC->createAll(System,*bifrostAxis,-3,*bifrostAxis,-3); 
   

 splitIndex = VPipeC->getLinkSurf(2);
 bunkerVoidThis=bunkerVoidNext;
   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,splitIndex);
  
  
   
    //     std::vector<int> vacIndex = VPipeC->getSurfs();
    //  std::cout << vacIndex.size()<< std::endl; 
  
    //      FocusC->setBack(-vacIndex[3]);
      

   
    
  // First chopper
  ChopperA->addInsertCell(bunkerVoidNext);
  //  ChopperA->createAll(System,FocusC->getKey("Guide0"),2);
  ChopperA->createAll(System,*VPipeC,2);

  attachSystem::addToInsertOuterSurfCtrl(System,*ChopperA,*FocusC);

  
  
  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());

  if(Control.EvalDefVar<int>("bifrostAppAConstruct",0)){ 
  AppA->addInsertCell(ChopperA->getCell("Void"));
  AppA->createAll(System,FocusC->getKey("Guide0"),-2);
    
  attachSystem::addToInsertOuterSurfCtrl(System,*AppA,*FocusC);
  }
  /*
  // Double disk chopper
  DDisk->addInsertCell(ChopperA->getCell("Void"));
  DDisk->setOffsetFlag(1);  // Z direction
  DDisk->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*DDisk); 
    */

  
  // Elliptic 4m section --> Replaced by bender
  //   VPipeBA->addInsertCell(bunkerVoidNext);

  //  return 0;
  
   BenderA->createAll(System,FocusC->getKey("Guide0"),2,FocusC->getKey("Guide0"),2);
   BenderA->addInsertCell(ChopperA->getCell("Void"));
 
   
   VPipeBA->setBack(BenderA->getKey("Guide0"),-2,false);
   //   VPipeBA->setBack(ChopperB->getKey("Main"),1, false);
   VPipeBA->createAll(System,ChopperA->getKey("Beam"),2);   

   splitIndex = VPipeBA->getLinkSurf(1);
   bunkerVoidThis=bunkerVoidNext;
   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,splitIndex);
   VPipeBA->addInsertCell(bunkerVoidThis);
       
   attachSystem::addToInsertForced(System,std::vector<int>{bunkerVoidNext},*VPipeBA);
   attachSystem::addToInsertOuterSurfCtrl(System,*VPipeBA,*BenderA);     

 

   splitIndex = VPipeBA->getLinkSurf(2);
  bunkerVoidNext=
     System.splitObject(bunkerVoidThis,splitIndex);

 

   // 10.5m FOC chopper
   ChopperB->addInsertCell(bunkerVoidNext);
   //   ChopperB->createAll(System,BenderA->getKey("Guide0"),2);
      ChopperB->createAll(System,*VPipeBA,2);
      attachSystem::addToInsertOuterSurfCtrl(System,*ChopperA,*BenderA);

     
           
  /*
  // Single disk FOC
  FOCDiskB->addInsertCell(ChopperB->getCell("Void"));
  FOCDiskB->setOffsetFlag(1);  // Z direction
  FOCDiskB->createAll(System,ChopperB->getKey("Main"),0);
  ChopperB->insertAxle(System,*FOCDiskB);
  */
  // Rectangle 6m section
      //  VPipeBB->addInsertCell(bunkerVoidNext);
  VPipeBB->setFront(ChopperB->getKey("Main"),2,false);
  
  BenderB->setFront(ChopperB->getKey("Main"),2);

  //   VPipeBB->addInsertCell(bunkerVoidNext);    
  VPipeBB->createAll(System,ChopperB->getKey("Beam"),2);
   //  attachSystem::addToInsertForced(System,bunkerVoidNext,bunkerVoidNext,*VPipeBB);


  splitIndex = VPipeBB->getLinkSurf(1);
      bunkerVoidThis=bunkerVoidNext;
   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,-splitIndex);
   bunkerVoidThis=bunkerVoidNext; 
  splitIndex = VPipeBB->getLinkSurf(2);
   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,splitIndex);
   VPipeBB->addInsertCell(bunkerVoidThis);
   attachSystem::addToInsertForced(System,std::vector<int>{bunkerVoidThis},*VPipeBB);

  
  BenderB->setBack(*VPipeBB,-2);
 
  BenderB->addInsertCell(VPipeBB->getCells("Void"));
  BenderB->createAll(System,FocusC->getKey("Guide0"),2,FocusC->getKey("Guide0"),2);

  // Rectangle 4m section
  
 
   VPipeBC->setFront(BenderB->getKey("Guide0"),2,false);
   VPipeBC->createAll(System,*VPipeBB,2);
  
   BenderC->setFront(*VPipeBB,2);
   BenderC->setBack(*VPipeBC,-2);

   BenderC->addInsertCell(VPipeBC->getCells("Void"));
   BenderC->createAll(System,FocusC->getKey("Guide0"),2,FocusC->getKey("Guide0"),2);

  // 
  //     BenderC->createAll(System,*VPipeBC,0,*VPipeBC,0);


  splitIndex = VPipeBC->getLinkSurf(2);
  bunkerVoidThis=bunkerVoidNext;
   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,splitIndex);
  VPipeBC->addInsertCell(bunkerVoidThis);
  attachSystem::addToInsertForced(System,std::vector<int>{bunkerVoidThis},*VPipeBC);

  // 20.5m FOC chopper-2
   ChopperC->addInsertCell(bunkerVoidNext);
  //  ChopperC->setFront(*VPipeBC,2,false);
  //  ChopperC->createAll(System,BenderC->getKey("Guide0"),2);
    ChopperC->createAll(System,*VPipeBC,2);

  
    // Single disk FOC-2
  /*
  FOCDiskC->addInsertCell(ChopperC->getCell("Void"));
  FOCDiskC->setOffsetFlag(1);  // Z direction
  FOCDiskC->createAll(System,ChopperC->getKey("Main"),0);
  ChopperC->insertAxle(System,*FOCDiskC);
  */
  
  // Bender continued
    //  VPipeBD->addInsertCell(bunkerVoidNext);
  VPipeBD->setFront(ChopperC->getKey("Main"),2,false);
  VPipeBD->createAll(System,ChopperC->getKey("Beam"),2);




  
  splitIndex = VPipeBD->getLinkSurf(1);
      bunkerVoidThis=bunkerVoidNext;
   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,-splitIndex);
   bunkerVoidThis=bunkerVoidNext; 
  splitIndex = VPipeBD->getLinkSurf(2);
   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,splitIndex);
   VPipeBD->addInsertCell(bunkerVoidThis);
   attachSystem::addToInsertForced(System,std::vector<int>{bunkerVoidThis},*VPipeBD);

   /*
   splitIndex = VPipeBD->getLinkSurf(1);
   bunkerVoidThis=bunkerVoidNext;
   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,splitIndex);
   VPipeBD->addInsertCell(bunkerVoidThis);       
   attachSystem::addToInsertForced(System,bunkerVoidNext,bunkerVoidNext,*VPipeBD);
   */
  BenderD->addInsertCell(VPipeBD->getCells("Void"));
  BenderD->setFront(ChopperC->getKey("Main"),2);
  BenderD->setBack(*VPipeBD,-2);
  //    BenderD->createAll(System,*VPipeBD,0,*VPipeBD,0);
  BenderD->createAll(System,FocusC->getKey("Guide0"),2,FocusC->getKey("Guide0"),2);
 

  //  BenderE->addInsertCell(VPipeBE->getCells("Void"));
  BenderE->setFront(*VPipeBD,2);
     //     BenderE->createAll(System,*VPipeBE,0,*VPipeBE,0);
  BenderE->createAll(System,FocusC->getKey("Guide0"),2,FocusC->getKey("Guide0"),2);
 
  VPipeBE->addInsertCell(bunkerVoidNext);
  VPipeBE->setFront(*VPipeBD,2,false);
  VPipeBE->setBack(BenderE->getKey("Guide0"),-2,false);
  VPipeBE->createAll(System,BenderD->getKey("Guide0"),2); 
   attachSystem::addToInsertOuterSurfCtrl(System,*VPipeBE,*BenderE);

 bunkerVoidThis=bunkerVoidNext; 
  splitIndex = VPipeBE->getLinkSurf(2);
   bunkerVoidNext=
     System.splitObject(bunkerVoidThis,splitIndex);
  
   //  AppB->addInsertCell(bunkerVoidNext);
 //  AppB->createAll(System,BenderE->getKey("Guide0"),2);



   //Cutting geometry


 {


   
   std::array<std::shared_ptr<beamlineSystem::GuideLine>,59> BPhantom;
    for (size_t i=0;i<59;i++){  
      BPhantom[i]=std::shared_ptr<beamlineSystem::GuideLine>
	(new beamlineSystem::GuideLine("bifrostPhantom"+std::to_string(i)));
       OR.addObject ("bifrostPhantom"+std::to_string(i),BPhantom[i]);
    //std::cout << i <<"aaa" << std::endl;
    
     BPhantom[i]->createAll(System,FocusC->getKey("Guide0"),2,
				   FocusC->getKey("Guide0"),2);
     }


    //    int icell=BenderA->getCells()[0]; //voids
    std::vector<int> icellB=BenderA->getCells(); //substrate
  for (size_t j=0; j< icellB.size(); j++)
    {  //   std::cout <<"icellB " <<  icellB.size() << std::endl;
  for (size_t i=0;i<7;i++){  
  int inew=System.splitObject(icellB[j],
				 BPhantom[i]->getKey("Guide0").getLinkSurf(2));
  icellB[j]=inew;
       }
    }

  
  int icell1=VPipeBA->getCell("Void");
  for (size_t i=0;i<7;i++){  
  int inew=System.splitObject(icell1,
				 BPhantom[i]->getKey("Guide0").getLinkSurf(2));
  icell1=inew;
	    }

  

  //  icell=BenderB->getCells()[0]; //voids
  icellB =BenderB->getCells(); //substrate
  for (size_t j=0; j< icellB.size(); j++)
    {     
  for (size_t i=8;i<18;i++){  
  int inew=System.splitObject(icellB[j],
				 BPhantom[i]->getKey("Guide0").getLinkSurf(2));
  icellB[j]=inew;
	    }

    }
  icell1=VPipeBB->getCell("Void");
  for (size_t i=8;i<18;i++){  
  int inew=System.splitObject(icell1,
				 BPhantom[i]->getKey("Guide0").getLinkSurf(2));
  icell1=inew;
	    }



  
  //  icell=BenderC->getCells()[0]; //voids
  icellB =BenderC->getCells(); //substrate
    for (size_t j=0; j< icellB.size(); j++)
    {     
    for (size_t i=18;i<28;i++){  
  int inew=System.splitObject(icellB[j],
				 BPhantom[i]->getKey("Guide0").getLinkSurf(2));
  icellB[j]=inew;
	    }
    }
  icell1=VPipeBC->getCell("Void");
  for (size_t i=18;i<28;i++){  
  int inew=System.splitObject(icell1,
				 BPhantom[i]->getKey("Guide0").getLinkSurf(2));
  icell1=inew;
	    }

  
  // icell=BenderD->getCells()[0]; //voids
  icellB=BenderD->getCells(); //substrates
    for (size_t j=0; j< icellB.size(); j++)
    {     
for (size_t i=29;i<43;i++){  
  int inew=System.splitObject(icellB[j],
				 BPhantom[i]->getKey("Guide0").getLinkSurf(2));
  icellB[j]=inew;
	    }
    }
    
  icell1=VPipeBD->getCell("Void");
  for (size_t i=29;i<43;i++){  
  int inew=System.splitObject(icell1,
				 BPhantom[i]->getKey("Guide0").getLinkSurf(2));
  icell1=inew;
	    }

  
  //  icell=BenderE->getCells()[0]; //voids
  icellB=BenderE->getCells(); //substrates
    for (size_t j=0; j< icellB.size(); j++)
    {     
for (size_t i=44;i<59;i++){  
  int inew=System.splitObject(icellB[j],
				 BPhantom[i]->getKey("Guide0").getLinkSurf(2));
  icellB[j]=inew;
	    }
    }
  icell1=VPipeBE->getCell("Void");
  for (size_t i=44;i<59;i++){  
  int inew=System.splitObject(icell1,
				 BPhantom[i]->getKey("Guide0").getLinkSurf(2));
  icell1=inew;
	    }

 

    
   for (size_t i=0;i<59;i++){  
      std::vector<int> iFsplit= BPhantom[i]->getCells();
      //      std::cout << iFsplit.size() << std::endl;
     for (int j=0;j<iFsplit.size();j++){
      System.removeCell(iFsplit[j]);
    }      
   }
 }


   
  return bunkerVoidNext;
}
  

}   // NAMESPACE essSystem

