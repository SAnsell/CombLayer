/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/chipIRHutch.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "generateSurf.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "createDivide.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SpecialSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ExternalCut.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "GeneralShutter.h"
#include "HoleUnit.h"
#include "InnerWall.h"
#include "PreCollimator.h"
#include "Jaws.h"
#include "ColBox.h"
#include "Table.h"
#include "beamBlock.h"
#include "BeamStop.h"
#include "ChipSample.h"

#include "chipIRHutch.h"

#include "surfDBase.h"
#include "mergeTemplate.h"

namespace hutchSystem
{

chipIRHutch::chipIRHutch(const std::string& Key)  : 
  attachSystem::FixedGroup(Key,"Main",4,"Beam",2),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  PreColObj(new PreCollimator("chipPre")),
  Jaw(new constructSystem::Jaws("chipJaw")),
  ColB(new ColBox("chipColBox")),
  Trimmer(new InnerWall("chipHutTrim")),
  FTable(new Table(0,"chipHutTable1")),
  BTable(new Table(1,"chipHutTable2")),
  BStop(new BeamStop("chipBS")),
  collimatorVoid(0),collActiveFlag(7)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Keyname for variables
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  OR.addObject(PreColObj);
  OR.addObject(Jaw);
  OR.addObject(ColB);
  OR.addObject(Trimmer);
  OR.addObject(FTable);
  OR.addObject(BTable);
  OR.addObject(BStop);
}

chipIRHutch::chipIRHutch(const chipIRHutch& A) : 
  attachSystem::FixedGroup(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  PreColObj(new PreCollimator(*A.PreColObj)),
  Jaw(new constructSystem::Jaws(*A.Jaw)),

  ColB(new ColBox(*A.ColB)),
  Trimmer(new InnerWall(*A.Trimmer)),
  FTable(new Table(*A.FTable)),BTable(new Table(*A.BTable)),
  BStop(new BeamStop(*A.BStop)),SampleItems(A.SampleItems),
  BeamCentPoint(A.BeamCentPoint),
  xStep(A.xStep),yStep(A.yStep),zLine(A.zLine),
  beamAngle(A.beamAngle),screenY(A.screenY),
  hGuideWidth(A.hGuideWidth),hFLWidth(A.hFLWidth),
  hFRWidth(A.hFRWidth),hFullLWidth(A.hFullLWidth),
  hPartRWidth(A.hPartRWidth),hFullRWidth(A.hFullRWidth),
  hMainLen(A.hMainLen),hLSlopeLen(A.hLSlopeLen),
  hRSlopeLen(A.hRSlopeLen),hPartLen(A.hPartLen),
  hExtLen(A.hExtLen),hRoof(A.hRoof),hFloor(A.hFloor),
  hSurfFloor(A.hSurfFloor),hVoidFloor(A.hVoidFloor),
  hRoofThick(A.hRoofThick),hFloorDepth(A.hFloorDepth),
  hRWallThick(A.hRWallThick),hLWallThick(A.hLWallThick),
  hFWallThick(A.hFWallThick),hBWallThick(A.hBWallThick),
  hEdgeVoid(A.hEdgeVoid),wDoor(A.wDoor),wInWall(A.wInWall),
  wOutWall(A.wOutWall),wBlockExtent(A.wBlockExtent),
  wBlockWidth(A.wBlockWidth),mBlockYBeg(A.mBlockYBeg),
  mBlockYEnd(A.mBlockYEnd),mBlockOut(A.mBlockOut),
  mBlockSndLen(A.mBlockSndLen),bsXStep(A.bsXStep),
  bsZStep(A.bsZStep),bsWidth(A.bsWidth),bsHeight(A.bsHeight),
  fbLength(A.fbLength),roofMat(A.roofMat),
  floorMat(A.floorMat),falseFloorMat(A.falseFloorMat),
  wallMat(A.wallMat),innerWallMat(A.innerWallMat),
  rearVoidMat(A.rearVoidMat),screenMat(A.screenMat),
  fbMat(A.fbMat),collimatorVoid(A.collimatorVoid),
  tailVoid(A.tailVoid),beamStopCent(A.beamStopCent),
  serverCent(A.serverCent),nLWallDivide(A.nLWallDivide),
  nRWallDivide(A.nRWallDivide),nRoofDivide(A.nRoofDivide),
  nFloorDivide(A.nFloorDivide),nTrimDivide(A.nTrimDivide),
  lWallFrac(A.lWallFrac),lWallMatList(A.lWallMatList),
  rWallFrac(A.rWallFrac),rWallMatList(A.rWallMatList),
  roofFrac(A.roofFrac),roofMatList(A.roofMatList),
  floorFrac(A.floorFrac),floorMatList(A.floorMatList),
  trimFrac(A.trimFrac),trimMatList(A.trimMatList),
  collActiveFlag(A.collActiveFlag)
  /*!
    Copy constructor
    \param A :: chipIRHutch to copy
  */
{}

chipIRHutch&
chipIRHutch::operator=(const chipIRHutch& A)
  /*!
    Assignment operator
    \param A :: chipIRHutch to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      *PreColObj = *A.PreColObj;
      *Jaw = *A.Jaw;
      *ColB = *A.ColB;
      *Trimmer = *A.Trimmer;
      *FTable = *A.FTable;
      *BTable = *A.BTable;
      *BStop = *A.BStop;
      SampleItems=A.SampleItems;
      BeamCentPoint=A.BeamCentPoint;
      xStep=A.xStep;
      yStep=A.yStep;
      zLine=A.zLine;
      beamAngle=A.beamAngle;
      screenY=A.screenY;
      hGuideWidth=A.hGuideWidth;
      hFLWidth=A.hFLWidth;
      hFRWidth=A.hFRWidth;
      hFullLWidth=A.hFullLWidth;
      hPartRWidth=A.hPartRWidth;
      hFullRWidth=A.hFullRWidth;
      hMainLen=A.hMainLen;
      hLSlopeLen=A.hLSlopeLen;
      hRSlopeLen=A.hRSlopeLen;
      hPartLen=A.hPartLen;
      hExtLen=A.hExtLen;
      hRoof=A.hRoof;
      hFloor=A.hFloor;
      hSurfFloor=A.hSurfFloor;
      hVoidFloor=A.hVoidFloor;
      hRoofThick=A.hRoofThick;
      hFloorDepth=A.hFloorDepth;
      hRWallThick=A.hRWallThick;
      hLWallThick=A.hLWallThick;
      hFWallThick=A.hFWallThick;
      hBWallThick=A.hBWallThick;
      hEdgeVoid=A.hEdgeVoid;
      wDoor=A.wDoor;
      wInWall=A.wInWall;
      wOutWall=A.wOutWall;
      wBlockExtent=A.wBlockExtent;
      wBlockWidth=A.wBlockWidth;
      mBlockYBeg=A.mBlockYBeg;
      mBlockYEnd=A.mBlockYEnd;
      mBlockOut=A.mBlockOut;
      mBlockSndLen=A.mBlockSndLen;
      bsXStep=A.bsXStep;
      bsZStep=A.bsZStep;
      bsWidth=A.bsWidth;
      bsHeight=A.bsHeight;
      fbLength=A.fbLength;
      roofMat=A.roofMat;
      floorMat=A.floorMat;
      falseFloorMat=A.falseFloorMat;
      wallMat=A.wallMat;
      innerWallMat=A.innerWallMat;
      rearVoidMat=A.rearVoidMat;
      screenMat=A.screenMat;
      fbMat=A.fbMat;
      collimatorVoid=A.collimatorVoid;
      tailVoid=A.tailVoid;
      beamStopCent=A.beamStopCent;
      serverCent=A.serverCent;
      nLWallDivide=A.nLWallDivide;
      nRWallDivide=A.nRWallDivide;
      nRoofDivide=A.nRoofDivide;
      nFloorDivide=A.nFloorDivide;
      nTrimDivide=A.nTrimDivide;
      lWallFrac=A.lWallFrac;
      lWallMatList=A.lWallMatList;
      rWallFrac=A.rWallFrac;
      rWallMatList=A.rWallMatList;
      roofFrac=A.roofFrac;
      roofMatList=A.roofMatList;
      floorFrac=A.floorFrac;
      floorMatList=A.floorMatList;
      trimFrac=A.trimFrac;
      trimMatList=A.trimMatList;
      collActiveFlag=A.collActiveFlag;
    }
  return *this;
}

chipIRHutch::~chipIRHutch() 
  /*!
    Destructor
  */
{}

void
chipIRHutch::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database
  */
{
  ELog::RegMethod RegA("Hutch","populate");

  beamAngle=Control.EvalVar<double>("chipSndAngle");
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zLine=Control.EvalVar<double>(keyName+"ZLine");

  screenY=Control.EvalVar<double>(keyName+"ScreenY");
  hGuideWidth=Control.EvalVar<double>(keyName+"GuideWidth");
  hFLWidth=Control.EvalVar<double>(keyName+"FLWidth");
  hFRWidth=Control.EvalVar<double>(keyName+"FRWidth");
  hFullLWidth=Control.EvalVar<double>(keyName+"FullLWidth");
  hPartRWidth=Control.EvalVar<double>(keyName+"PartRWidth");
  hFullRWidth=Control.EvalVar<double>(keyName+"FullRWidth");
  hMainLen=Control.EvalVar<double>(keyName+"MainLen");
  hPartLen=Control.EvalVar<double>(keyName+"PartLen");
  hExtLen=Control.EvalVar<double>(keyName+"ExtLen");
  hLSlopeLen=Control.EvalVar<double>(keyName+"LSlopeLen");
  hRSlopeLen=Control.EvalVar<double>(keyName+"RSlopeLen");
  hRoof=Control.EvalVar<double>(keyName+"Roof");
  hFloor=Control.EvalVar<double>(keyName+"Floor");
  hSurfFloor=Control.EvalVar<double>(keyName+"FalseFloorSurf");
  hVoidFloor=Control.EvalVar<double>(keyName+"FalseFloorVoid");

  hRoofThick=Control.EvalVar<double>(keyName+"RoofThick");
  hFloorDepth=Control.EvalVar<double>(keyName+"FloorDepth");
  hRWallThick=Control.EvalVar<double>(keyName+"RWallThick");
  hLWallThick=Control.EvalVar<double>(keyName+"LWallThick");
  hFWallThick=Control.EvalVar<double>(keyName+"FWallThick");
  hBWallThick=Control.EvalVar<double>(keyName+"BWallThick");

  hEdgeVoid=Control.EvalVar<double>(keyName+"EdgeVoid");

  // Walkway
  wInWall=Control.EvalVar<double>(keyName+"WalkInWall");
  wOutWall=Control.EvalVar<double>(keyName+"WalkOutWall");
  wDoor=Control.EvalVar<double>(keyName+"ExtDoor");

  // Block unit
  mBlockYBeg=Control.EvalVar<double>(keyName+"BlockYBeg");
  mBlockYEnd=Control.EvalVar<double>(keyName+"BlockYEnd");
  mBlockOut=Control.EvalVar<double>(keyName+"BlockOut");
  mBlockSndLen=Control.EvalVar<double>(keyName+"BlockSndLen");

  // BeamStop
  bsXStep=Control.EvalVar<double>(keyName+"BSOpenXStep");
  bsZStep=Control.EvalVar<double>(keyName+"BSOpenZStep");
  bsWidth=Control.EvalVar<double>(keyName+"BSOpenWidth");
  bsHeight=Control.EvalVar<double>(keyName+"BSOpenHeight");

  // EXTRA WALL
  westExtraThick=Control.EvalVar<double>(keyName+"WestExtraThick");
  westExtraLength=Control.EvalVar<double>(keyName+"WestExtraLength");
  
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");  
  innerWallMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerWallMat");  
  rearVoidMat=ModelSupport::EvalMat<int>(Control,keyName+"RearVoidMat");  
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");
  falseFloorMat=ModelSupport::EvalMat<int>(Control,keyName+"FalseFloorMat");
  screenMat=ModelSupport::EvalMat<int>(Control,keyName+"ScreenMat");

  fbLength=Control.EvalVar<double>(keyName+"FBLen");
  fbMat=ModelSupport::EvalMat<int>(Control,keyName+"FBmat");

  // Samples:
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const size_t nSamples=Control.EvalVar<size_t>(keyName+"NSamples");
  for(size_t i=0;i<nSamples;i++)
    {
      SampleItems.push_back(std::shared_ptr<ChipSample>
			    (new ChipSample("chipSample",i+1)));
      OR.addObject(SampleItems.back());
    }


  // Wall dividers:
  nLWallDivide=Control.EvalVar<size_t>(keyName+"LWallNDivide");
  nRWallDivide=Control.EvalVar<size_t>(keyName+"RWallNDivide");
  nFloorDivide=Control.EvalVar<size_t>(keyName+"FloorNDivide");
  nRoofDivide=Control.EvalVar<size_t>(keyName+"RoofNDivide");
  nWestDivide=Control.EvalVar<size_t>(keyName+"WestNDivide");


  ModelSupport::populateDivide(Control,nLWallDivide,
			       keyName+"LWallMat_",wallMat,lWallMatList);
  ModelSupport::populateDivide(Control,nRWallDivide,
			       keyName+"RWallMat_",wallMat,rWallMatList);
  ModelSupport::populateDivide(Control,nFloorDivide,
			       keyName+"FloorMat_",floorMat,floorMatList);
  ModelSupport::populateDivide(Control,nRoofDivide,
			       keyName+"RoofMat_",roofMat,roofMatList);
  ModelSupport::populateDivide(Control,nWestDivide,
			       keyName+"WestMat_",wallMat,westMatList);
  
  ModelSupport::populateDivideLen(Control,nLWallDivide,
				  keyName+"LWallFrac_",hLWallThick,
				  lWallFrac);
  ModelSupport::populateDivideLen(Control,nRWallDivide,
				  keyName+"RWallFrac_",hRWallThick,
				  rWallFrac);
  ModelSupport::populateDivideLen(Control,nFloorDivide,
				  keyName+"FloorFrac_",hFloorDepth,floorFrac);
  ModelSupport::populateDivideLen(Control,nRoofDivide,
			       keyName+"RoofFrac_",hRoofThick,roofFrac);
  ModelSupport::populateDivideLen(Control,nWestDivide,
			       keyName+"WestLen_",westExtraThick,westFrac);
  
  return;
}

void
chipIRHutch::createUnitVector(const attachSystem::FixedComp& FG,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param shutterFC :: shutter direction 
    \param xyAxis :: xyAngle
    \param LC :: connecting linear component [ChipIRGuide]
  */
{
  ELog::RegMethod RegA("Hutch","createUnitVector");

  const attachSystem::FixedGroup* FGPtr=
    dynamic_cast<const attachSystem::FixedGroup*>(&FG);
  if (FGPtr)
    {
    
      attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
      attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
      
      mainFC.createUnitVector(FGPtr->getKey("Main"),sideIndex);
      mainFC.setCentre(FGPtr->getKey("Main").getLinkPt(7));
      beamFC.createUnitVector(FGPtr->getKey("Beam"),sideIndex);
      beamFC.setCentre(FGPtr->getKey("Main").getLinkPt(7));
      
      FixedGroup::setDefault("Main","Beam");
      
      // remove old Z component and replace:
      
      mainFC.applyShift(xStep,yStep,0);
      const double zStep(zLine-mainFC.getZ().dotProd(Origin));
      mainFC.applyShift(0,0,zStep);
      beamFC.applyAngleRotate(0,beamAngle);
    }
  else
    FixedGroup::createUnitVector(FG,sideIndex);

  FixedGroup::setDefault("Main","Beam");
  
	    
  return;
}

void 
chipIRHutch::createWallObjects(Simulation& System,
			       const attachSystem::ContainedComp& IC)
/*!
    Create the walls for the chipIR hutch
    \param System :: Simulation
    \param IC :: Inner Insert object of Wall from the guide
   */
{
  ELog::RegMethod RegA("Hutch","createWallObjects");
  // Create outer virtual void
  std::string Out;

  // -------------
  // INNER SPACE:
  // -------------
  // Front void
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -101 33 -34 15 -16 -84");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  addCell("FrontVoid",cellIndex-1);
  collimatorVoid=cellIndex-1;
  
  // tail void
  Out=ModelSupport::getComposite(SMap,buildIndex,"102 -32 33 (-44 : -82) "
				 "(-84 : 82) 43 35 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  addCell("TailVoid",cellIndex-1);
  tailVoid=cellIndex-1;

  // -------------------------
  //       OUTER WALLS
  // -------------------------
  // Surround walls Main [LEFT]
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -2 3 13 15 -16 "
				 "(-33 : -43 )");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  layerCells["leftWall"]=cellIndex-1;

  // Out=ModelSupport::getComposite(SMap,buildIndex,
  // 				 "1 -2

  // Surround walls Main front [RIGHT+FRONT]
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -4 11 3 15 -8 -16 (-1:34) ");
  Out+=IC.getExclude();
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  layerCells["rightFWall"]=cellIndex-1;

  // EDGE VOID:
  // Out=ModelSupport::getComposite(SMap,buildIndex,
  // 				 "11 -22 (-3:-13) 73 15 -16 ");
  // Out+=WC.getExclude();
  // System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  // Surround walls Right back
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "8 15 -16 84 -14 -2 (-24 : 62) (-82 : 44) ");
  Out+=IC.getExclude();
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  layerCells["rightBWall"]=cellIndex-1;
  
  // Right Extension 
  Out=ModelSupport::getComposite(SMap,buildIndex,"2 -22 -14 44 15 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  layerCells["walkWallCell"]=cellIndex-1;  

  // MAIN ROOF: ALL
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -22 3 (-2 : 64) 13 (-4 : 62) "
                                 "-14 (-24 : 62) 16 -6");
  Out+=IC.getExclude();
  System.addCell(MonteCarlo::Object(cellIndex++,roofMat,0.0,Out));
  layerCells["roof"]=cellIndex-1;  
  BStop->addInsertCell(cellIndex-1);

  // FLOOR: ALL
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -22 3 13 (-4 : 62) -14 (-24 : 62) -15 5");
  Out+=IC.getExclude();
  System.addCell(MonteCarlo::Object(cellIndex++,floorMat,0.0,Out));
  layerCells["floor"]=cellIndex-1;
  // FALSE FLOOR:
  Out=ModelSupport::getComposite(SMap,buildIndex,"102 -32 33 "
				 "(-44 : -82) "
				 "(-84 : 82) 43 25 -35");
  System.addCell(MonteCarlo::Object(cellIndex++,falseFloorMat,0.0,Out));
  // FALSE FLOOR [void]:
  Out=ModelSupport::getComposite(SMap,buildIndex,"102 -32 33 "
				 "(-44 : -82) "
				 "(-84 : 82) 43 15 -25");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  
  // ----------------------------------------------------
  // CREATE WALKWAY
  // ----------------------------------------------------
  //  door:
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "54 -44 42 -22 15 -16 42");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "64 -54 2 -22 15 -16 ");
  System.addCell(MonteCarlo::Object(cellIndex++,innerWallMat,0.0,Out));
  BStop->addInsertCell(cellIndex-1);

  // void in walkway [above floor]
  Out=ModelSupport::getComposite(SMap,buildIndex,"54 -44 32 -42 35 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  // floor in walkway
  Out=ModelSupport::getComposite(SMap,buildIndex,"54 -44 32 -42 25 -35");
  System.addCell(MonteCarlo::Object(cellIndex++,falseFloorMat,0.0,Out));

  // Concrete feedthrough
  Out=ModelSupport::getComposite(SMap,buildIndex,"54 -44 902 -42 15 -25");
  System.addCell(MonteCarlo::Object(cellIndex++,fbMat,0.0,Out));
  // void in walkway
  Out=ModelSupport::getComposite(SMap,buildIndex,"54 -44 32 -902 15 -25");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  // REAR WALL [WITH Void for BEAMSTOP]
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "32 -54 33 43 -2 15 -16"
				 " (-503 : 504 : -505 : 506)");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  layerCells["backWall"]=cellIndex-1;
  BStop->addInsertCell(cellIndex-1);

  // Void for rear wall
  Out=ModelSupport::getComposite(SMap,buildIndex,"32 -2 503  -504 505 -506");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  BStop->addInsertCell(cellIndex-1);

  // Void [Goes into roof space]
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "2 -22 -64 13 15 -6");
  System.addCell(MonteCarlo::Object(cellIndex++,rearVoidMat,0.0,Out));
  BStop->addInsertCell(cellIndex-1);

  // Block unit : On forward corner 
  if ((mBlockYEnd-mBlockYBeg)>0.0)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "402 -412 -14 -44 (-84 : 82) 35 -16 404");
      System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
      layerCells["blockWall"]=cellIndex-1;

      // Add to inner void
      MonteCarlo::Object* QH=System.findObject(tailVoid);
      Out=ModelSupport::getComposite(SMap,buildIndex,"(-402:412:-404)");
      QH->addSurfString(Out);
    }
    
  //  serverCent=calcCentroid(302,503,505,22,504,506);
  return;
}

void
chipIRHutch::addOuterVoid()
  /*!\
    Create outer virtual space that includes the beamstop etc
  */
{
  ELog::RegMethod RegA("chipIRHutch","addOuterVoid");
  // 73 : replaced 3 13
  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex,
			       "11 -22 3 13 -14 (-4 : 62) (-24 : 62) 5 -6 ");
  addOuterUnionSurf(Out);
  return;
}

  
void 
chipIRHutch::createWallSurfaces(const attachSystem::FixedComp& Guide)
  /*!
    Create the walls for the hutch
    \param Guide :: Object this is joined to
  */
{
  ELog::RegMethod RegA("chipIRHutch","createWallSurface");

  Geometry::Plane* PX;
  // ---------------------------------------------------
  // OUTER WALLS:
  // ---------------------------------------------------
  SMap.addMatch(buildIndex+1,Guide.getLinkSurf(2));

  ELog::EM<<"MAIN == "<<Origin<<ELog::endDiag;
  ELog::EM<<"MXYZ == "<<X<<" : "<<Y<<" : "<<Z<<ELog::endDiag;
    
  // CENTRE LINE [normal on x axis]:
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,X); 
  
  SMap.addMatch(buildIndex+11,Guide.getLinkSurf(7));
  //  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*hFWallThick,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*hMainLen,Y);

  // Extend Back Wall
  ModelSupport::buildPlane(SMap,buildIndex+62,Origin+Y*hPartLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*hExtLen,Y);

   // Left outer flat
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*hFullLWidth,X);  
  
   // Right outer flat
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*hFullRWidth,X);  

  // Right outer close
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*hPartRWidth,X);  

  // Roof  
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*hRoof,Z);  
  // Floor  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*hFloor,Z);  

  // Left SIDE ANGLE:
  Geometry::Vec3D ALPt=Origin-X*hFLWidth-Y*hFWallThick;;
  Geometry::Vec3D BLPt=Origin-X*hFullLWidth+Y*(hLSlopeLen-hFWallThick);
  ModelSupport::buildPlane(SMap,buildIndex+3,ALPt,BLPt,BLPt+Z,X);
  // LEFT EXTRA VOID
  ModelSupport::buildPlane(SMap,buildIndex+73,
			   Origin-X*(hFullLWidth+hEdgeVoid),X);  
  // Right SIDE ANGLE:
  Geometry::Vec3D ARPt=Origin+X*hFRWidth-Y*hFWallThick;
  Geometry::Vec3D BRPt=Origin+X*hPartRWidth+Y*(hRSlopeLen-hFWallThick);
  ModelSupport::buildPlane(SMap,buildIndex+4,ARPt,BRPt,BRPt+Z,X);
  

  // ----------------------------------------------------
  // INNER Surfaces 
  // ----------------------------------------------------

  // Inner Floor
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(hFloor-hFloorDepth),Z);  

  // Inner Roof
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(hRoof-hRoofThick),Z);  

  // Inner Back wall
  ModelSupport::buildPlane(SMap,buildIndex+32,
			   Origin+Y*(hMainLen-hBWallThick),Y);

  // Left flat
  ModelSupport::buildPlane(SMap,buildIndex+43,
			   Origin-X*(hFullLWidth-hLWallThick),X);  

   // R-part Inner-F
  ModelSupport::buildPlane(SMap,buildIndex+84,
			   Origin+X*(hPartRWidth-hRWallThick),X);    

  // Right inner flat
  ModelSupport::buildPlane(SMap,buildIndex+44,
			   Origin+X*(hFullRWidth-hRWallThick),X);  

  // Left side Inner: (make from previous -normal)
  PX=SMap.realPtr<Geometry::Plane>(buildIndex+3);
  ALPt+=PX->getNormal()*hLWallThick;
  BLPt+=PX->getNormal()*hLWallThick;
  ModelSupport::buildPlane(SMap,buildIndex+33,ALPt,BLPt,BLPt+Z,X);
  //  PX=SurI.createUniqSurf<Geometry::Plane>(buildIndex+33);  

  // Right side Inner: (make from previous -normal)
  PX=SMap.realPtr<Geometry::Plane>(buildIndex+4);
  ARPt-=PX->getNormal()*hRWallThick;
  BRPt-=PX->getNormal()*hRWallThick;
  ModelSupport::buildPlane(SMap,buildIndex+34,ARPt,BRPt,BRPt+Z,X);
  
  // Right Inner step
  ModelSupport::buildPlane(SMap,buildIndex+82,
			   Origin+Y*(hPartLen+hRWallThick),Y);  
  
  // ----------------------------------------------------
  // FALSE FLOOR
  // ----------------------------------------------------
   // Al Inner Floor
  ModelSupport::buildPlane(SMap,buildIndex+35,
			   Origin+Z*(hFloorDepth+hVoidFloor+hSurfFloor-hFloor),Z);  

  // void
  ModelSupport::buildPlane(SMap,buildIndex+25,
			   Origin+Z*(hFloorDepth+hVoidFloor-hFloor),Z);  

  // Create Concrete layer [feedthrough]:
  ModelSupport::buildPlane(SMap,buildIndex+902,
			   Origin+Y*(hExtLen-fbLength),Y);  

  // ----------------------------------------------------
  // CREATE WALKWAY
  // ----------------------------------------------------
  ModelSupport::buildPlane(SMap,buildIndex+42,
			   Origin+Y*(hExtLen-wDoor),Y);  

  // wall touching walk passage
  ModelSupport::buildPlane(SMap,buildIndex+54,
			   Origin+X*wInWall,X);  

  // wall touching outside/beamstop
  ModelSupport::buildPlane(SMap,buildIndex+64,
			   Origin+X*wOutWall,X);  
    
  //-----------------------------------------------------
  // CREATE INNER STEPS
  //----------------------------------------------------

  ModelSupport::buildPlane(SMap,buildIndex+402,
			   Origin+Y*(mBlockYBeg),Y);  

  ModelSupport::buildPlane(SMap,buildIndex+412,
			   Origin+Y*(mBlockYEnd),Y);  

  // Y Mid layer
  ModelSupport::buildPlane(SMap,buildIndex+418,
			   Origin+Y*((mBlockYEnd+mBlockYBeg)/2.0),Y);  

  // Y back Step 
  ModelSupport::buildPlane(SMap,buildIndex+404,
			   Origin+X*(mBlockOut),X);  

   // Second block
  ModelSupport::buildPlane(SMap,buildIndex+422,
			   Origin+Y*(mBlockSndLen),Y);  

  // BEAMSTOP VOID

  // left/right
  ModelSupport::buildPlane(SMap,buildIndex+503,
			   Origin+X*(bsXStep-bsWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+504,
			   Origin+X*(bsXStep+bsWidth/2.0),X);
  // up/down
  ModelSupport::buildPlane(SMap,buildIndex+505,
			   Origin+Z*(bsZStep-bsHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+506,
			   Origin+Z*(bsZStep+bsHeight/2.0),Z);
  
  // SPECIAL 8 divider [To split wall]
  PX=ModelSupport::createDividerSurf(buildIndex+8,-Z,
     SMap.realPtr<Geometry::Plane>(buildIndex+34),
     SMap.realPtr<Geometry::Plane>(buildIndex+84),
     SMap.realPtr<Geometry::Plane>(buildIndex+4),
     SMap.realPtr<Geometry::Plane>(buildIndex+24));
  SMap.registerSurf(buildIndex+8,PX);

  // Use trimmers attached objects
  SMap.addMatch(buildIndex+101,Trimmer->getLinkSurf(1));
  SMap.addMatch(buildIndex+102,Trimmer->getLinkSurf(-2));

  // Attach back containment:
  Trimmer->addBoundarySurf(SMap.realSurf(buildIndex+33));
  Trimmer->addBoundarySurf(-SMap.realSurf(buildIndex+34));
  Trimmer->addBoundarySurf(-SMap.realSurf(buildIndex+84));
  Trimmer->addBoundarySurf(SMap.realSurf(buildIndex+15));
  Trimmer->addBoundarySurf(-SMap.realSurf(buildIndex+16));

  // EXTRA WALL:
  if (westExtraThick>Geometry::zeroTol)
    {
      // Front point is Origin : Extend 11
      ModelSupport::buildShiftedPlane(SMap,buildIndex+1011,
			      SMap.realPtr<Geometry::Plane>(buildIndex+11),
				      -westExtraThick);
      ModelSupport::buildShiftedPlane(SMap,buildIndex+1004,
			      SMap.realPtr<Geometry::Plane>(buildIndex+4),
			      westExtraThick);
      ModelSupport::buildShiftedPlane(SMap,buildIndex+1024,
      			      SMap.realPtr<Geometry::Plane>(buildIndex+24),
				      westExtraThick);
      ModelSupport::buildPlane(SMap,buildIndex+1002,
			       Origin+Y*westExtraLength,Y);
    }
  
  return;
}

void
chipIRHutch::writeMasterPoints() const
  /*!
    Write out the master points
  */
{
  ELog::RegMethod RegA("chipIRHutch","writeMasterPoints");

  const masterRotate& MR=masterRotate::Instance();
  //  const attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  const attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();
  //.setCNum(chipIRDatum::shutterAxis,Saxis);

  CS.setCNum(chipIRDatum::tubeAxis,MR.calcAxisRotate(beamFC.getY()));
  CS.setCNum(chipIRDatum::table1Cent,MR.calcRotate(calcIndexPosition(0)));
  CS.setCNum(chipIRDatum::table2Cent,MR.calcRotate(calcIndexPosition(1)));
  CS.setCNum(chipIRDatum::beamStop,MR.calcRotate(BStop->getCentre()));
  //const Geometry::Vec3D BStop=Origin+Y*bsYStart;
  //  CS.setCNum(chipIRDatum::beamStop,MR.calcRotate(BStop));

  // Line direction
  Geometry::Vec3D O=MR.reverseRotate(Geometry::Vec3D(0,4.475,0));
  Geometry::Vec3D D=
    MR.reverseRotate(Geometry::Vec3D(sin(46.0*M_PI/180.0),
				     cos(46.0*M_PI/180.0),0));
  Geometry::Line LX(O,D);
  Geometry::Vec3D Pt(Origin-X*hFullLWidth);
  Pt-= Z*Pt.dotProd(Z);

  //  ELog::EM<<"Distance of surf   == "<<LX.distance(Pt)<<ELog::endCrit;

  // Calculate the back step:
  std::vector<Geometry::Vec3D> Inter;
  ModelSupport::calcVertex(SMap.realSurf(buildIndex+11),
			   SMap.realSurf(buildIndex+6),
			   SMap.realSurf(buildIndex+3),
			   Inter,0);
  Geometry::Vec3D ApexPt=Inter.back();
  // remove Z : 
  ApexPt-= Z*ApexPt.dotProd(Z);

  return;
  // Calculate the points that the beamstop holds:
  const int aPln=SMap.realSurf(buildIndex+301);  
  int sPln[4];
  for(int i=0;i<4;i++)
    sPln[i]=SMap.realSurf(buildIndex+303+i);  

  //  std::vector<Geometry::Vec3D> Inter;

  ModelSupport::calcVertex(aPln,sPln[0],sPln[2],Inter,0);
  CS.setDNum(chipIRDatum::flood1,MR.calcRotate(Inter.back()));
  Inter.clear();

  ModelSupport::calcVertex(aPln,sPln[0],sPln[3],Inter,0);
  CS.setDNum(chipIRDatum::flood2,MR.calcRotate(Inter.back()));
  Inter.clear();

  ModelSupport::calcVertex(aPln,sPln[1],sPln[2],Inter,0);
  CS.setDNum(chipIRDatum::flood3,MR.calcRotate(Inter.back()));
  Inter.clear();

  ModelSupport::calcVertex(aPln,sPln[1],sPln[3],Inter,0);
  CS.setDNum(chipIRDatum::flood4,MR.calcRotate(Inter.back()));
  
  return;
}

Geometry::Vec3D
chipIRHutch::calcIndexPosition(const int Index) const
  /*!
    Caclulate the position based on a number
     - 1 :: Table 1
     - 2 :: Table 2
     - 3 :: Before Precollimator
     - 4 :: Before V-collimator
     - 5 :: Before H-collimator
     - 6 :: Before Separator wall
     - 7 :: Middle of the filter set
    \param Index :: index of position
    \return Point of the table
  */
{
  ELog::RegMethod RegA("chipIRHutch","calcIndexPosition");

  switch (Index) 
    {
    case 0:              // Table 1
      return FTable->getBeamPos();
    case 1:              // Table 2
      return BTable->getBeamPos();
    case 2:              // Guide exit point+small step
      return Origin+Y;
    case 3:              // beamstop ??? -- this is not correct
      return Origin+Y;
    }
  throw ColErr::IndexError<int>(Index,4,"Index");
} 

void 
chipIRHutch::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    Currently three:
      -- Roof
      -- Wall
      -- Trimmer 
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("Hutch","LayerProcess");

  return;
  
  if (nRoofDivide>1)
    {
      ModelSupport::surfDivide DA;
      // Generic
      for(size_t i=0;i<static_cast<size_t>(nRoofDivide)-1;i++)
	{
	  DA.addFrac(roofFrac[i]);
	  DA.addMaterial(roofMatList[i]);
	}
      DA.addMaterial(roofMatList.back());

      DA.init();
      
      DA.setCellN(layerCells["roof"]);
      DA.setOutNum(cellIndex,buildIndex+801);
      DA.makeTemplate<Geometry::Plane>(SMap.realSurf(buildIndex+16),
				   SMap.realSurf(buildIndex+6));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }

  if (nFloorDivide>1)
    {
      ModelSupport::surfDivide DA;
      // Generic
      for(size_t i=0;i<static_cast<size_t>(nFloorDivide)-1;i++)
	{
	  DA.addFrac(floorFrac[i]);
	  DA.addMaterial(floorMatList[i]);
	}
      DA.addMaterial(floorMatList.back());

      DA.init();
      
      DA.setCellN(layerCells["floor"]);
      DA.setOutNum(cellIndex,buildIndex+801);
      DA.makeTemplate<Geometry::Plane>(-SMap.realSurf(buildIndex+15),
				   SMap.realSurf(buildIndex+5));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  if (nLWallDivide>1)
    {
      // PROCESS WALLS:
      ModelSupport::surfDivide DA;
      for(size_t i=0;i<static_cast<size_t>(nLWallDivide)-1;i++)
	{
	  DA.addFrac(lWallFrac[i]);
	  DA.addMaterial(lWallMatList[i]);
	}
      DA.addMaterial(lWallMatList.back());

      // Cell Specific:
      // BACK WALL:
      DA.setOutNum(cellIndex,buildIndex+801);
      if (layerCells.find("backWall")!=layerCells.end())
        {
	  DA.setCellN(layerCells["backWall"]);
	  DA.makeTemplate<Geometry::Plane>(SMap.realSurf(buildIndex+32),
				       SMap.realSurf(buildIndex+2));
	  DA.activeDivide(System);
	  cellIndex=DA.getCellNum();
	}
      if (layerCells.find("leftWall")!=layerCells.end())
        {
	  DA.init();

	  cellIndex=DA.procSurfDivide(System,SMap,layerCells["leftWall"],
			    buildIndex,cellIndex,0,
				      {{33,3},{43,13}},
				      "(-33 : -43)","3 13");
	}
    }

  if (nRWallDivide>1)
    {
      // PROCESS WALLS:
      ModelSupport::surfDivide DA;
      DA.addLayers(nRWallDivide,rWallFrac,rWallMatList);
      DA.setOutNum(cellIndex,buildIndex+801);

      // RIGHT WALL [Front]
      if (layerCells.find("rightFWall")!=layerCells.end())
        {
	  DA.procSurfDivide(System,SMap,layerCells["rightFWall"],
			    buildIndex,cellIndex,0,
			    {{34,4},{1,11}},"(-1:34)","-4 11");
	}
      if (layerCells.find("walkWall")!=layerCells.end())
	{
	  DA.procSurfDivide(System,SMap,layerCells["walkWall"],
			    buildIndex,cellIndex,0,
			    {{44,14}},"-14","44");
	}
      // RIGHT WALL [Front]
      if (layerCells.find("rightBWall")!=layerCells.end())
        {

	  DA.procSimpleDivide(System,SMap,layerCells["rightBWall"],
			      buildIndex,cellIndex,0,
			      {{84,-24},{-82,62},{44,-14}});
	}
    }

  if (westExtraThick>Geometry::zeroTol && nWestDivide>1)
    {
      ModelSupport::surfDivide DA;
      DA.addLayers(nWestDivide,westFrac,westMatList);
      DA.procSurfDivide(System,SMap,layerCells["westOuter"],
			buildIndex,cellIndex,1501,
			{{24,1024}},"24","-1024");

      DA.procSurfDivide(System,SMap,layerCells["westFront"],
      			buildIndex,cellIndex,1601,
      			{{4,1004},{11,1011}}," 4:-11 ","1011 -1004");
    }

  return;
}

 
int
chipIRHutch::exitWindow(const double,std::vector<int>&,
			Geometry::Vec3D&) const
  /*!
    Determine the exit window from the object :
    since 
    \return Nothing [throws]
  */
{
  throw ColErr::AbsObjMethod("chipIRHutch::exitWindow");
}

void
chipIRHutch::addExtraWalls(Simulation& System,
			   const attachSystem::FixedComp& Guide)
  /*!
    Build the annoying extra walls
    \param System :: Simulation
    \param Guide :: for fixed unit
   */
{
  ELog::RegMethod RegA("chipIRHutch","addExtraWalls");

  if (westExtraThick>Geometry::zeroTol)
    {
      std::string Out;
      
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "-1004 1011 (4:-11) -8 15 -6 ");
      Out+=Guide.getLinkString(9);
      System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
      addOuterUnionSurf(Out);
      
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "-1024 24 -1002 8 15 -6 ");
      System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out)); 
      addOuterUnionSurf(Out);

      layerCells["westFront"]=cellIndex-2;
      layerCells["westOuter"]=cellIndex-1;
    }

  return;
}
    
void
chipIRHutch::addCollimators(Simulation& System,
			    const attachSystem::FixedGroup& GI)
  /*!
    Add collimator objects if defined
    \param System :: Simulation item to build
    \param GI :: Guide Item. To use for positioning
  */
{
  ELog::RegMethod RegA("chipIRHutch","addCollimators");

  PreColObj->addInsertCell(collimatorVoid);
  PreColObj->addBoundarySurf(SMap.realSurf(buildIndex+33));
  PreColObj->addBoundarySurf(-SMap.realSurf(buildIndex+34));

  if (collActiveFlag & 1)
    PreColObj->createAll(System,GI.getKey("Beam"),2);
  else
    PreColObj->createPartial(System,GI.getKey("Beam"),2);


  // Stuff for collimator box:
  if (collActiveFlag & 8)
    {
      //      ColB->addInsertCell(collimatorVoid);
      //      ColB->setMidFace(GI.getExit());
      //      ColB->createAll(System,*this);
    }
  Jaw->addInsertCell(collimatorVoid);
  Jaw->addBoundarySurf(SMap.realSurf(buildIndex+33));
  // Jaw->addBoundarySurf(-SMap.realSurf(buildIndex+34));
  // Jaw->addBoundarySurf(-SMap.realSurf(buildIndex+16));
  // Jaw->addBoundarySurf(SMap.realSurf(buildIndex+15));
  
  if (collActiveFlag & 2)
    Jaw->createAll(System,getKey("Beam"),-1);

  return;
}
  
Geometry::Vec3D
chipIRHutch::calcCentroid(const int pA,const int pB,const int pC,
			  const int pX,const int pY,const int pZ) const
  /*!
    Calculate the centroid between the two intersecting corners
    \param pA :: Plane on first corner
    \param pB :: Plane on first corner
    \param pC :: Plane on first corner
    \param pX :: Plane on second corner
    \param pY :: Plane on second corner
    \param pZ :: Plane on second corner
    \return Centroid
   */
{
  ELog::RegMethod RegA("chipIRHutch","calcCentroid");

  return SurInter::calcCentroid(SMap.realSurfPtr(buildIndex+pA),
				SMap.realSurfPtr(buildIndex+pB),
				SMap.realSurfPtr(buildIndex+pC),
				SMap.realSurfPtr(buildIndex+pX),
				SMap.realSurfPtr(buildIndex+pY),
				SMap.realSurfPtr(buildIndex+pZ));
}

Geometry::Vec3D
chipIRHutch::calcSideIntercept(const int sideIndex,
			       const Geometry::Vec3D& Pt,
			       const Geometry::Vec3D& Norm) const
  /*!
    Given a side: Calculate the intercept point on that surface
    \param sideIndex :: Side index [0-6]
    \param Pt :: Start point of line
    \param Norm :: Normal of line
    \return The point that the vector impacts the outside
  */
{
  ELog::RegMethod RegA("chipIRHutch","calcSideIntercept");
  
  const Geometry::Plane* PX=
    SMap.realPtr<Geometry::Plane>(buildIndex+sideIndex);

  if (!PX)
    throw ColErr::InContainerError<int>(sideIndex,"No surface "+RegA.getFull());
    
  Geometry::Line Lx;
  std::vector<Geometry::Vec3D> Out;
  Lx.setLine(Pt,Norm);
  if (!Lx.intersect(Out,*PX))
    {
      ELog::EM<<"Point doesnt intercept:"<<Pt<<" "<<Norm<<" "<<*PX<<ELog::endErr;
      return Pt;
    }
  return Out.front();
}

Geometry::Vec3D
chipIRHutch::calcSurfNormal(const int surfIndex) const
  /*!
    Nasty function to get the outer surface normal
    \param surfIndex :: Side to use
    \return The Normal from the surface
   */
{
  ELog::RegMethod RegA("chipIRHutch","getSurfNormal");

  const Geometry::Plane* PX=
    SMap.realPtr<Geometry::Plane>(buildIndex+surfIndex);

  if (!PX)
    throw ColErr::InContainerError<int>(surfIndex,
					"No surface "+RegA.getFull());
  return PX->getNormal();
}

Geometry::Vec3D
chipIRHutch::calcDoorPoint() const
  /*!
    Calculate the point at the back of the door
    \return Back Plane point
   */
{
  ELog::RegMethod RegA("chipIRHutch","calcBackPlane");
  
  // Combination of surf: 22/44/25
  Geometry::Vec3D Offset=Origin+
    X*(hFullRWidth-hRWallThick)+
    Y*(hExtLen+2.5)+
    Z*(hFloorDepth+hVoidFloor-hFloor);   
  return Offset;
}

void
chipIRHutch::createLinks()
  /*!
    Create all the link points
  */
{
  ELog::RegMethod RegA("chipIRHutch","createLinks");
  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.setConnect(0,Origin,-Y);
  mainFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  mainFC.setConnect(1,Origin+Y*(hMainLen-hBWallThick),Y);
  mainFC.setLinkSurf(1,-SMap.realSurf(buildIndex+102));

  mainFC.setConnect(3,Origin+Y*hMainLen,Y);
  mainFC.setLinkSurf(3,-SMap.realSurf(buildIndex+32));

  beamFC.setConnect(0,Origin,-Y);
  beamFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  beamFC.setConnect(1,Origin+Y*(hMainLen-hBWallThick),Y);
  beamFC.setLinkSurf(1,-SMap.realSurf(buildIndex+102));
  
  return;
}

void
chipIRHutch::createCommonAll(Simulation& System,
			     const attachSystem::FixedGroup& Guide,
			     const attachSystem::ContainedComp& IC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param Guide :: Linear Comp to beam from [chipGuide]
    \param IC :: Internal Component that contains is own external perimeter
  */
{
  ELog::RegMethod RegA("Hutch","createCommonAll");

  Trimmer->createOnlySurfaces(System,Guide.getKey("Main"),2);
  createWallSurfaces(Guide.getKey("Main"));
  
  createWallObjects(System,IC);
  addExtraWalls(System,Guide.getKey("Main"));
  createLinks();

  Trimmer->createOnlyObjects(System);

  addOuterVoid();
  return;
  addCollimators(System,Guide);
  BStop->createAll(System,getKey("Main"),4);

  FTable->setFloor(SMap.realSurf(buildIndex+35));
  FTable->addInsertCell(tailVoid);
  FTable->createAll(System,Guide,0);
  BTable->setFloor(SMap.realSurf(buildIndex+35));
  BTable->addInsertCell(tailVoid);
  BTable->createAll(System,Guide,0);

  for(std::shared_ptr<ChipSample>& SI : SampleItems)
    {
      SI->addInsertCell(tailVoid);
      SI->createAll(System,*FTable,0);
    }
  layerProcess(System);

  insertObjects(System);       
  
  writeMasterPoints();
  return;
}


void
chipIRHutch::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const attachSystem::FixedGroup& Guide,
		       const attachSystem::ContainedComp& IC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param Guide :: Linear Comp to beam from [chipGuide]
    \param IC :: Internal Component that contains is own external perimeter
  */
{
  ELog::RegMethod RegA("Hutch","createAll(Fixed)");

  populate(System.getDataBase());
  //  createUnitVector(FC,FC.getY(),Guide.getKey("Main"));
  createUnitVector(Guide,2);
  createCommonAll(System,Guide,IC);
  return;
}

void
chipIRHutch::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Guide unit
    \param sideIndex :: lin point
  */
{
  ELog::RegMethod RegA("chipIRHutch","createAll(ShutterPort)");

  populate(System.getDataBase());

  //  ELog::EM<<"Shutter Port == "<<ShutterPort.getXYAxis()<<ELog::endDiag;

  createUnitVector(FC,sideIndex);
  const attachSystem::FixedGroup* FGPtr=
    dynamic_cast<const attachSystem::FixedGroup*>(&FC);
  if (FGPtr)
    {
      ELog::EM<<"FC == "<<FGPtr->getKey("Main").getLinkPt(sideIndex)
	      <<ELog::endDiag;
      ELog::EM<<"FC == "<<FGPtr->getKey("Main").getLinkAxis(sideIndex)
	      <<ELog::endDiag;
    }

  //  createUnitVector(FC,sideIndex);
  ELog::EM<<"Y == "<<Origin<<" "<<Y<<" "<<Z<<ELog::endDiag;
  //  createCommonAll(System,Guide,IC);
  return;
}

}  // NAMESPACE chipSystem

