/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/Hutch.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
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
#include "Object.h"
#include "Qhull.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "createDivide.h"
#include "Simulation.h"
#include "SpecialSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "InsertComp.h"
#include "ContainedComp.h"
#include "GeneralShutter.h"
#include "HoleUnit.h"
#include "InnerWall.h"
#include "PreCollimator.h"
#include "Collimator.h"
#include "ColBox.h"
#include "Table.h"
#include "beamBlock.h"
#include "BeamStop.h"
#include "ChipSample.h"
#include "Hutch.h"

namespace hutchSystem
{

ChipIRHutch::ChipIRHutch(const std::string& Key)  : 
  attachSystem::TwinComp(Key,3),attachSystem::ContainedComp(),
  hutchIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hutchIndex+1),populated(0),
  PreColObj(new PreCollimator("chipPre")),
  ColObjV(new Collimator("chipColV")),
  ColObjH(new Collimator("chipColH")),
  ColB(new ColBox("chipColBox")),
  Trimmer(new InnerWall("chipHutTrim")),
  FTable(new Table(0,"chipHutTable1")),
  BTable(new Table(1,"chipHutTable2")),
  BStop(new BeamStop("chipBS")),
  collimatorVoid(0),leftWallCell(0),
  rightFWallCell(0),rightBWallCell(0),
  backWallCell(0),roofCell(0),blockCell(0),floorCell(0),
  walkWallCell(0),collActiveFlag(7)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Keyname for variables
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  OR.addObject(PreColObj);
  OR.addObject(ColObjV);
  OR.addObject(ColObjH);
  OR.addObject(ColB);
  OR.addObject(Trimmer);
  OR.addObject(FTable);
  OR.addObject(BTable);
  OR.addObject(BStop);
}

ChipIRHutch::ChipIRHutch(const ChipIRHutch& A) : 
  attachSystem::TwinComp(A),attachSystem::ContainedComp(A),
  hutchIndex(A.hutchIndex),cellIndex(A.cellIndex),
  populated(A.populated),
  PreColObj(new PreCollimator(*A.PreColObj)),
  ColObjV(new Collimator(*A.ColObjV)),
  ColObjH(new Collimator(*A.ColObjH)),
  ColB(new ColBox(*A.ColB)),
  Trimmer(new InnerWall(*A.Trimmer)),
  FTable(new Table(*A.FTable)),BTable(new Table(*A.BTable)),
  BStop(new BeamStop(*A.BStop)),SampleItems(A.SampleItems),
  BeamCentPoint(A.BeamCentPoint),ImpactPoint(A.ImpactPoint),
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
  leftWallCell(A.leftWallCell),
  rightFWallCell(A.rightFWallCell),
  rightBWallCell(A.rightBWallCell),
  backWallCell(A.backWallCell),roofCell(A.roofCell),
  blockCell(A.blockCell),
  floorCell(A.floorCell),walkWallCell(A.walkWallCell),
  collActiveFlag(A.collActiveFlag)
  /*!
    Copy constructor
    \param A :: ChipIRHutch to copy
  */
{}

ChipIRHutch&
ChipIRHutch::operator=(const ChipIRHutch& A)
  /*!
    Assignment operator
    \param A :: ChipIRHutch to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::TwinComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      *PreColObj = *A.PreColObj;
      *ColObjV = *A.ColObjV;
      *ColObjH = *A.ColObjH;
      *ColB = *A.ColB;
      *Trimmer = *A.Trimmer;
      *FTable = *A.FTable;
      *BTable = *A.BTable;
      *BStop = *A.BStop;
      SampleItems=A.SampleItems;
      BeamCentPoint=A.BeamCentPoint;
      ImpactPoint=A.ImpactPoint;
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
      leftWallCell=A.leftWallCell;
      rightFWallCell=A.rightFWallCell;
      rightBWallCell=A.rightBWallCell;
      backWallCell=A.backWallCell;
      roofCell=A.roofCell;
      blockCell=A.blockCell;
      floorCell=A.floorCell;
      walkWallCell=A.walkWallCell;
      collActiveFlag=A.collActiveFlag;
    }
  return *this;
}

ChipIRHutch::~ChipIRHutch() 
  /*!
    Destructor
  */
{}

void
ChipIRHutch::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ChipIRHutch","populate");

  const FuncDataBase& Control=System.getDataBase();

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
  const int nSamples=Control.EvalVar<int>(keyName+"NSamples");
  for(int i=0;i<nSamples;i++)
    SampleItems.push_back(ChipSample("chipSample",i+1));


  // Wall dividers:
  nLWallDivide=Control.EvalVar<size_t>(keyName+"LWallNDivide");
  nRWallDivide=Control.EvalVar<size_t>(keyName+"RWallNDivide");
  nFloorDivide=Control.EvalVar<size_t>(keyName+"FloorNDivide");
  nRoofDivide=Control.EvalVar<size_t>(keyName+"RoofNDivide");


  ModelSupport::populateDivide(Control,nLWallDivide,
			      keyName+"LWallMat_",wallMat,lWallMatList);
  ModelSupport::populateDivide(Control,nRWallDivide,
			      keyName+"RWallMat_",wallMat,rWallMatList);
  ModelSupport::populateDivide(Control,nFloorDivide,
			      keyName+"FloorMat_",floorMat,floorMatList);
  ModelSupport::populateDivide(Control,nRoofDivide,
			      keyName+"RoofMat_",roofMat,roofMatList);
  
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
  
  populated=1;
  return;
}

void
ChipIRHutch::createUnitVector(const shutterSystem::GeneralShutter& GI,
			      const attachSystem::FixedComp& LC)
  /*!
    Create the unit vectors
    \param GI :: shutter direction
    \param LC :: connectin linear component [ChipIRGuide]
  */
{
  ELog::RegMethod RegA("ChipIRHutch","createUnitVector");

  const masterRotate& MR=masterRotate::Instance();
  attachSystem::FixedComp::createUnitVector(LC);

  Origin=LC.getLinkPt(6);                 // [Guide Position]
  BeamCentPoint=LC.getExit();             // [NOT USED YET]
  bY=Y;

  Origin+=X*xStep+Y*yStep;
  // remove old Z component and replace:
  Origin+=Z*(zLine-Z.dotProd(Origin));
  
  Geometry::Quaternion::calcQRotDeg(beamAngle,X).rotate(bY);
  bX=X;
  bZ=bY*X;

  ImpactPoint= GI.getFrontPt();
  setExit(Origin+Y*hMainLen,Y);
  SecondTrack::setBeamExit(BeamCentPoint,bY);
  
  //  Geometry::Quaternion::calcQRotDeg(beamAngle,X).rotate(Z);
  //  Geometry::Quaternion::calcQRotDeg(beamAngle,X).rotate(Y);

  const Geometry::Vec3D Saxis=MR.calcAxisRotate(GI.getXYAxis());

  chipIRDatum::chipDataStore::Instance().
    setCNum(chipIRDatum::shutterAxis,Saxis);

  chipIRDatum::chipDataStore::Instance().
    setCNum(chipIRDatum::secScatImpact,
	     MR.calcRotate(GI.getFrontPt()));
  
  return;
}

void 
ChipIRHutch::createWallObjects(Simulation& System,
			       const attachSystem::ContainedComp& IC)
/*!
    Create the walls for the chipIR hutch
    \param System :: Simulation
    \param IC :: Inner Insert object of Wall from the guide
   */
{
  ELog::RegMethod RegA("ChipIRHutch","createWallObjects");
  // Create outer virtual void
  std::string Out;

  // -------------
  // INNER SPACE:
  // -------------
  // Front void
  Out=ModelSupport::getComposite(SMap,hutchIndex,"1 -101 33 -34 15 -16 -84");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  collimatorVoid=cellIndex-1;
  
  // tail void
  Out=ModelSupport::getComposite(SMap,hutchIndex,"102 -32 33 (-44 : -82) "
				 "(-84 : 82) 43 35 -16");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  tailVoid=cellIndex-1;

  // -------------------------
  //       OUTER WALLS
  // -------------------------
  // Surround walls Main [LEFT]
  Out=ModelSupport::getComposite(SMap,hutchIndex,
				 "-100 1 -2 3 13 15 -16 "
				 "(-33 : -43 )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  leftWallCell=cellIndex-1;

  // Out=ModelSupport::getComposite(SMap,hutchIndex,
  // 				 "1 -2

  // Surround walls Main front [RIGHT+FRONT]
  Out=ModelSupport::getComposite(SMap,hutchIndex,
				 " -4 11 3 15 -8 -16 (-1:34) ");
  Out+=IC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  rightFWallCell=cellIndex-1;

  // EDGE VOID:
  // Out=ModelSupport::getComposite(SMap,hutchIndex,
  // 				 "11 -22 (-3:-13) 73 15 -16 ");
  // Out+=WC.getExclude();
  // System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Surround walls Right back
  Out=ModelSupport::getComposite(SMap,hutchIndex,
				 "8 15 -16 84 -14 -2 (-24 : 62) (-82 : 44) ");
  Out+=IC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  rightBWallCell=cellIndex-1;
  
  // Right Extension 
  Out=ModelSupport::getComposite(SMap,hutchIndex,"2 -22 -14 44 15 -16");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  walkWallCell=cellIndex-1;

  // MAIN ROOF: ALL
  Out=ModelSupport::getComposite(SMap,hutchIndex,
				 "11 -22 3 (-2 : 64) 13 (-4 : 62) "
                                 "-14 (-24 : 62) 16 -6");
  Out+=IC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,roofMat,0.0,Out));
  roofCell=cellIndex-1;
  BStop->addInsertCell(roofCell);

  // FLOOR: ALL
  Out=ModelSupport::getComposite(SMap,hutchIndex,
				 "11 -22 3 13 (-4 : 62) -14 (-24 : 62) -15 5");
  Out+=IC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,floorMat,0.0,Out));
  floorCell=cellIndex-1;
  // FALSE FLOOR:
  Out=ModelSupport::getComposite(SMap,hutchIndex,"102 -32 33 "
				 "(-44 : -82) "
				 "(-84 : 82) 43 25 -35");
  System.addCell(MonteCarlo::Qhull(cellIndex++,falseFloorMat,0.0,Out));
  // FALSE FLOOR [void]:
  Out=ModelSupport::getComposite(SMap,hutchIndex,"102 -32 33 "
				 "(-44 : -82) "
				 "(-84 : 82) 43 15 -25");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  // ----------------------------------------------------
  // CREATE WALKWAY
  // ----------------------------------------------------
  //  door:
  Out=ModelSupport::getComposite(SMap,hutchIndex,
				 "54 -44 42 -22 15 -16 42");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,hutchIndex,
				 "64 -54 2 -22 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,0.0,Out));
  BStop->addInsertCell(cellIndex-1);

  // void in walkway [above floor]
  Out=ModelSupport::getComposite(SMap,hutchIndex,"54 -44 32 -42 35 -16");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // floor in walkway
  Out=ModelSupport::getComposite(SMap,hutchIndex,"54 -44 32 -42 25 -35");
  System.addCell(MonteCarlo::Qhull(cellIndex++,falseFloorMat,0.0,Out));

  // Concrete feedthrough
  Out=ModelSupport::getComposite(SMap,hutchIndex,"54 -44 902 -42 15 -25");
  System.addCell(MonteCarlo::Qhull(cellIndex++,fbMat,0.0,Out));
  // void in walkway
  Out=ModelSupport::getComposite(SMap,hutchIndex,"54 -44 32 -902 15 -25");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // REAR WALL [WITH Void for BEAMSTOP]
  Out=ModelSupport::getComposite(SMap,hutchIndex,
				 "32 -54 33 43 -2 15 -16"
				 " (-503 : 504 : -505 : 506)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  backWallCell=cellIndex-1;
  BStop->addInsertCell(backWallCell);

  // Void for rear wall
  Out=ModelSupport::getComposite(SMap,hutchIndex,"32 -2 503  -504 505 -506");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  BStop->addInsertCell(cellIndex-1);

  // Void [Goes into roof space]
  Out=ModelSupport::getComposite(SMap,hutchIndex,
				 "2 -22 -64 13 15 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,rearVoidMat,0.0,Out));
  BStop->addInsertCell(cellIndex-1);

  // Block unit : On forward corner 
  if ((mBlockYEnd-mBlockYBeg)>0.0)
    {
      Out=ModelSupport::getComposite(SMap,hutchIndex,
				     "402 -412 -14 -44 (-84 : 82) 35 -16 404");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      blockCell=cellIndex-1;

      // Add to inner void
      MonteCarlo::Qhull* QH=System.findQhull(tailVoid);
      Out=ModelSupport::getComposite(SMap,hutchIndex,"(-402:412:-404)");
      QH->addSurfString(Out);
    }
    
  //  serverCent=calcCentroid(302,503,505,22,504,506);
  return;
}

void
ChipIRHutch::addOuterVoid()
  /*!\
    Create outer virtual space that includes the beamstop etc
  */
{
  ELog::RegMethod RegA("ChipIRHutch","addOuterVoid");
  // 73 : replaced 3 13
  const std::string Out=
    ModelSupport::getComposite(SMap,hutchIndex,
			       "11 -22 3 13 -14 (-4 : 62) (-24 : 62) 5 -6 ");
  addOuterSurf(Out);
  return;
}

void 
ChipIRHutch::createWallSurfaces(const attachSystem::FixedComp& Guide)
  /*!
    Create the walls for the hutch
    \param Guide :: Object this is joined to
  */
{
  ELog::RegMethod RegA("ChipIRHutch","createWallSurface");

  Geometry::Plane* PX;
  // ---------------------------------------------------
  // OUTER WALLS:
  // ---------------------------------------------------
  SMap.addMatch(hutchIndex+1,Guide.getLinkSurf(1));

  // CENTRE LINE [normal on x axis]:
  ModelSupport::buildPlane(SMap,hutchIndex+100,Origin,X); 
  
  SMap.addMatch(hutchIndex+11,Guide.getLinkSurf(6));
  //  ModelSupport::buildPlane(SMap,hutchIndex+11,Origin-Y*hFWallThick,Y);
  ModelSupport::buildPlane(SMap,hutchIndex+2,Origin+Y*hMainLen,Y);

  // Extend Back Wall
  ModelSupport::buildPlane(SMap,hutchIndex+62,Origin+Y*hPartLen,Y);
  ModelSupport::buildPlane(SMap,hutchIndex+22,Origin+Y*hExtLen,Y);

   // Left outer flat
  ModelSupport::buildPlane(SMap,hutchIndex+13,Origin-X*hFullLWidth,X);  
  
   // Right outer flat
  ModelSupport::buildPlane(SMap,hutchIndex+14,Origin+X*hFullRWidth,X);  

  // Right outer close
  ModelSupport::buildPlane(SMap,hutchIndex+24,Origin+X*hPartRWidth,X);  

  // Roof  
  ModelSupport::buildPlane(SMap,hutchIndex+6,Origin+Z*hRoof,Z);  
  // Floor  
  ModelSupport::buildPlane(SMap,hutchIndex+5,Origin-Z*hFloor,Z);  

  // Left SIDE ANGLE:
  Geometry::Vec3D ALPt=Origin-X*hFLWidth-Y*hFWallThick;;
  Geometry::Vec3D BLPt=Origin-X*hFullLWidth+Y*(hLSlopeLen-hFWallThick);
  ModelSupport::buildPlane(SMap,hutchIndex+3,ALPt,BLPt,BLPt+Z,X);
  // LEFT EXTRA VOID
  ModelSupport::buildPlane(SMap,hutchIndex+73,
			   Origin-X*(hFullLWidth+hEdgeVoid),X);  
  // Right SIDE ANGLE:
  Geometry::Vec3D ARPt=Origin+X*hFRWidth-Y*hFWallThick;
  Geometry::Vec3D BRPt=Origin+X*hPartRWidth+Y*(hRSlopeLen-hFWallThick);
  ModelSupport::buildPlane(SMap,hutchIndex+4,ARPt,BRPt,BRPt+Z,X);
  

  // ----------------------------------------------------
  // INNER Surfaces 
  // ----------------------------------------------------

  // Inner Floor
  ModelSupport::buildPlane(SMap,hutchIndex+15,
			   Origin-Z*(hFloor-hFloorDepth),Z);  

  // Inner Roof
  ModelSupport::buildPlane(SMap,hutchIndex+16,
			   Origin+Z*(hRoof-hRoofThick),Z);  

  // Inner Back wall
  ModelSupport::buildPlane(SMap,hutchIndex+32,
			   Origin+Y*(hMainLen-hBWallThick),Y);

  // Left flat
  ModelSupport::buildPlane(SMap,hutchIndex+43,
			   Origin-X*(hFullLWidth-hLWallThick),X);  

   // R-part Inner-F
  ModelSupport::buildPlane(SMap,hutchIndex+84,
			   Origin+X*(hPartRWidth-hRWallThick),X);    

  // Right inner flat
  ModelSupport::buildPlane(SMap,hutchIndex+44,
			   Origin+X*(hFullRWidth-hRWallThick),X);  

  // Left side Inner: (make from previous -normal)
  PX=SMap.realPtr<Geometry::Plane>(hutchIndex+3);
  ALPt+=PX->getNormal()*hLWallThick;
  BLPt+=PX->getNormal()*hLWallThick;
  ModelSupport::buildPlane(SMap,hutchIndex+33,ALPt,BLPt,BLPt+Z,X);
  //  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+33);  

  // Right side Inner: (make from previous -normal)
  PX=SMap.realPtr<Geometry::Plane>(hutchIndex+4);
  ARPt-=PX->getNormal()*hRWallThick;
  BRPt-=PX->getNormal()*hRWallThick;
  ModelSupport::buildPlane(SMap,hutchIndex+34,ARPt,BRPt,BRPt+Z,X);
  
  // Right Inner step
  ModelSupport::buildPlane(SMap,hutchIndex+82,
			   Origin+Y*(hPartLen+hRWallThick),Y);  
  
  // ----------------------------------------------------
  // FALSE FLOOR
  // ----------------------------------------------------
   // Al Inner Floor
  ModelSupport::buildPlane(SMap,hutchIndex+35,
			   Origin+Z*(hFloorDepth+hVoidFloor+hSurfFloor-hFloor),Z);  

  // void
  ModelSupport::buildPlane(SMap,hutchIndex+25,
			   Origin+Z*(hFloorDepth+hVoidFloor-hFloor),Z);  

  // Create Concrete layer [feedthrough]:
  ModelSupport::buildPlane(SMap,hutchIndex+902,
			   Origin+Y*(hExtLen-fbLength),Y);  

  // ----------------------------------------------------
  // CREATE WALKWAY
  // ----------------------------------------------------
  ModelSupport::buildPlane(SMap,hutchIndex+42,
			   Origin+Y*(hExtLen-wDoor),Y);  

  // wall touching walk passage
  ModelSupport::buildPlane(SMap,hutchIndex+54,
			   Origin+X*wInWall,X);  

  // wall touching outside/beamstop
  ModelSupport::buildPlane(SMap,hutchIndex+64,
			   Origin+X*wOutWall,X);  
    
  //-----------------------------------------------------
  // CREATE INNER STEPS
  //----------------------------------------------------

  ModelSupport::buildPlane(SMap,hutchIndex+402,
			   Origin+Y*(mBlockYBeg),Y);  

  ModelSupport::buildPlane(SMap,hutchIndex+412,
			   Origin+Y*(mBlockYEnd),Y);  

  // Y Mid layer
  ModelSupport::buildPlane(SMap,hutchIndex+418,
			   Origin+Y*((mBlockYEnd+mBlockYBeg)/2.0),Y);  

  // Y back Step 
  ModelSupport::buildPlane(SMap,hutchIndex+404,
			   Origin+X*(mBlockOut),X);  

   // Second block
  ModelSupport::buildPlane(SMap,hutchIndex+422,
			   Origin+Y*(mBlockSndLen),Y);  

  // BEAMSTOP VOID

  // left/right
  ModelSupport::buildPlane(SMap,hutchIndex+503,
			   Origin+X*(bsXStep-bsWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutchIndex+504,
			   Origin+X*(bsXStep+bsWidth/2.0),X);
  // up/down
  ModelSupport::buildPlane(SMap,hutchIndex+505,
			   Origin+Z*(bsZStep-bsHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,hutchIndex+506,
			   Origin+Z*(bsZStep+bsHeight/2.0),Z);
  
  // SPECIAL 8 divider [To split wall]
  PX=ModelSupport::createDividerSurf(hutchIndex+8,-Z,
     SMap.realPtr<Geometry::Plane>(hutchIndex+34),
     SMap.realPtr<Geometry::Plane>(hutchIndex+84),
     SMap.realPtr<Geometry::Plane>(hutchIndex+4),
     SMap.realPtr<Geometry::Plane>(hutchIndex+24));
  SMap.registerSurf(hutchIndex+8,PX);

  // Use trimmers attached objects
  SMap.addMatch(hutchIndex+101,Trimmer->getLU(0).getLinkSurf());
  SMap.addMatch(hutchIndex+102,-Trimmer->getLU(1).getLinkSurf());

  // Attach back containment:
  Trimmer->addBoundarySurf(SMap.realSurf(hutchIndex+33));
  Trimmer->addBoundarySurf(-SMap.realSurf(hutchIndex+34));
  Trimmer->addBoundarySurf(-SMap.realSurf(hutchIndex+84));
  Trimmer->addBoundarySurf(SMap.realSurf(hutchIndex+15));
  Trimmer->addBoundarySurf(-SMap.realSurf(hutchIndex+16));
      
  return;
}

void
ChipIRHutch::writeMasterPoints() const
  /*!
    Write out the master points
  */
{
  ELog::RegMethod RegA("ChipIRHutch","writeMasterPoints");

  const masterRotate& MR=masterRotate::Instance();
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();
  //.setCNum(chipIRDatum::shutterAxis,Saxis);

  CS.setCNum(chipIRDatum::tubeAxis,MR.calcAxisRotate(bY));
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
  ModelSupport::calcVertex(SMap.realSurf(hutchIndex+11),
			   SMap.realSurf(hutchIndex+6),
			   SMap.realSurf(hutchIndex+3),
			   Inter,0);
  Geometry::Vec3D ApexPt=Inter.back();
  // remove Z : 
  ApexPt-= Z*ApexPt.dotProd(Z);

  return;
  // Calculate the points that the beamstop holds:
  const int aPln=SMap.realSurf(hutchIndex+301);  
  int sPln[4];
  for(int i=0;i<4;i++)
    sPln[i]=SMap.realSurf(hutchIndex+303+i);  

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

int
ChipIRHutch::isObjectContained(Simulation& System,
			       const int PrimaryObj,
			       const int SecondaryObj)
  /*!
    Compare two object to find if they intersect.
    \param System :: Simulation to use
    \param PrimaryObj :: Object to calculate if extents to Secondary
    \param SecondaryObj :: Object to check points exist in
    \return 1 on intersection / 0 on no intersection
  */
{
  ELog::RegMethod RegA("ChipIRHutch","isObjectContained");

  MonteCarlo::Qhull* POptr=System.findQhull(PrimaryObj);
  MonteCarlo::Qhull* SOptr=System.findQhull(SecondaryObj);
  if (!POptr || !SOptr)
    {
      ELog::EM<<"Failed to find object "<<PrimaryObj<<" "
	      <<SecondaryObj<<ELog::endErr;
      return 0;
    }
  
  if (!POptr->hasIntersections())
    POptr->calcIntersections();

  return 0;
}

Geometry::Vec3D
ChipIRHutch::calcIndexPosition(const int Index) const
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
  ELog::RegMethod RegA("ChipIRHutch","calcIndexPosition");

  switch (Index) 
    {
    case 0:              // Table 1
      return FTable->getBeamPos();
    case 1:              // Table 2
      return BTable->getBeamPos();
    case 2:              // Guide exit point+small step
      return Origin+bY;
    case 3:              // beamstop ??? -- this is not correct
      return Origin+bY;
    }
  throw ColErr::IndexError<int>(Index,4,RegA.getFull());
} 

void 
ChipIRHutch::layerProcess(Simulation& System)
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
      
      DA.setCellN(roofCell);
      DA.setOutNum(cellIndex,hutchIndex+801);
      DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+16),
				   SMap.realSurf(hutchIndex+6));
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
      
      DA.setCellN(floorCell);
      DA.setOutNum(cellIndex,hutchIndex+801);
      DA.makePair<Geometry::Plane>(-SMap.realSurf(hutchIndex+15),
				   SMap.realSurf(hutchIndex+5));
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
      DA.setOutNum(cellIndex,hutchIndex+801);	  
      if (backWallCell)
        {
	  DA.setCellN(backWallCell);
	  DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+32),
				       SMap.realSurf(hutchIndex+2));
	  DA.activeDivide(System);
	  cellIndex=DA.getCellNum();
	}
      if (leftWallCell)
        {
	  DA.init();
	  DA.setCellN(leftWallCell);
	  DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+33),
				       -SMap.realSurf(hutchIndex+3));
	  DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+43),
				       -SMap.realSurf(hutchIndex+13));
	  DA.activeDivide(System);
	  cellIndex=DA.getCellNum();
	}
    }
  if (nRWallDivide>1)
    {
      // PROCESS WALLS:
      ModelSupport::surfDivide DA;
      for(size_t i=0;i<static_cast<size_t>(nRWallDivide)-1;i++)
	{
	  DA.addFrac(rWallFrac[i]);
	  DA.addMaterial(rWallMatList[i]);
	}
      DA.addMaterial(rWallMatList.back());
      DA.setOutNum(cellIndex,hutchIndex+801);	  
      // RIGHT WALL [Front]
      if (rightFWallCell)
        {
	  DA.init();
	  DA.setCellN(rightFWallCell);
	  DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+34),
				       SMap.realSurf(hutchIndex+4));
	  DA.makeSignPair<Geometry::Plane>(SMap.realSurf(hutchIndex+1),
					   SMap.realSurf(hutchIndex+11),1);
	  DA.activeDivide(System);
	}
      if (walkWallCell)
	{
	  DA.init();
	  DA.setCellN(walkWallCell);
	  DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+44),
				       SMap.realSurf(hutchIndex+14));
	  DA.activeDivide(System);
	}
      // RIGHT WALL [Front]
      if (rightBWallCell)
        {
	  DA.init();
	  DA.setCellN(rightBWallCell);
	  DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+84),
				       SMap.realSurf(hutchIndex+24)); 
	  DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+82),
				       -SMap.realSurf(hutchIndex+62));
	  DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+44),
				       SMap.realSurf(hutchIndex+14));
	  DA.activeDivide(System);
	}
    }
  return;
}

int
ChipIRHutch::exitWindow(const double,std::vector<int>&,
			Geometry::Vec3D&) const
  /*!
    Determine the exit window from the object :
    since 
    \return Nothing [throws]
  */
{
  throw ColErr::AbsObjMethod("ChipIRHutch::exitWindow");
}

void
ChipIRHutch::addCollimators(Simulation& System,
			    const attachSystem::TwinComp& GI)
  /*!
    Add collimator objects if defined
    \param System :: Simulation item to build
    \param GI :: Guide Item. To use for positioning
  */
{
  ELog::RegMethod RegA("ChipIRHutch","addCollimators");

  const masterRotate& MR=masterRotate::Instance();
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  PreColObj->addInsertCell(collimatorVoid);
  PreColObj->addBoundarySurf(SMap.realSurf(hutchIndex+33));
  PreColObj->addBoundarySurf(-SMap.realSurf(hutchIndex+34));
  
  if (collActiveFlag & 1)
    PreColObj->createAll(System,GI);
  else
    PreColObj->createPartial(System,GI);
  

  // Stuff for collimator box:
  if (collActiveFlag & 8)
    {
      ColB->addInsertCell(collimatorVoid);
      ColB->setMidFace(GI.getExit());
      ColB->createAll(System,*this);
    }
  
  ColObjV->addInsertCell(collimatorVoid);
  ColObjV->setAxis(Z);
  ColObjV->setMidFace(GI.getExit());
  // Note Vertial needs both sides:
  ColObjV->addBoundarySurf(SMap.realSurf(hutchIndex+33));
  ColObjV->addBoundarySurf(-SMap.realSurf(hutchIndex+34));
  ColObjV->addBoundarySurf(-SMap.realSurf(hutchIndex+16));
  ColObjV->addBoundarySurf(SMap.realSurf(hutchIndex+15));
  
  if (collActiveFlag & 2)
    ColObjV->createAll(System,*this);
  else
    ColObjV->createPartial(System,*this);

  ColObjH->addInsertCell(collimatorVoid);
  ColObjH->setAxis(X);
  ColObjH->setMidFace(GI.getExit());
  ColObjH->addBoundarySurf(SMap.realSurf(hutchIndex+33));
  ColObjH->addBoundarySurf(-SMap.realSurf(hutchIndex+34));
  ColObjH->addBoundarySurf(-SMap.realSurf(hutchIndex+16));
  ColObjH->addBoundarySurf(SMap.realSurf(hutchIndex+15));
  
  if (collActiveFlag & 4)
    ColObjH->createAll(System,*this);
  else
    ColObjH->createPartial(System,*this);

  CS.setCNum(chipIRDatum::preCollOrigin,
	     MR.calcRotate(PreColObj->getCentre()));
  CS.setCNum(chipIRDatum::preCollAxis,
	     MR.calcAxisRotate(PreColObj->getBeamAxis()));
  CS.setCNum(chipIRDatum::preCollHoleCent,
	     MR.calcRotate(PreColObj->getHoleCentre()));
  CS.setCNum(chipIRDatum::collVOrigin,
	     MR.calcRotate(ColObjV->getCentre()));
  CS.setCNum(chipIRDatum::collVAxis,
	     MR.calcAxisRotate(ColObjV->getBeamAxis()));
  CS.setCNum(chipIRDatum::collHOrigin,
	     MR.calcRotate(ColObjH->getCentre()));
  CS.setCNum(chipIRDatum::collHAxis,
	     MR.calcAxisRotate(ColObjH->getBeamAxis()));

  return;
}
  
Geometry::Vec3D
ChipIRHutch::calcCentroid(const int pA,const int pB,const int pC,
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
  ELog::RegMethod RegA("ChipIRHutch","calcCentroid");

  return SurInter::calcCentroid(SMap.realSurfPtr(hutchIndex+pA),
				SMap.realSurfPtr(hutchIndex+pB),
				SMap.realSurfPtr(hutchIndex+pC),
				SMap.realSurfPtr(hutchIndex+pX),
				SMap.realSurfPtr(hutchIndex+pY),
				SMap.realSurfPtr(hutchIndex+pZ));
}

Geometry::Vec3D
ChipIRHutch::calcSideIntercept(const int sideIndex,const Geometry::Vec3D& Pt,
			       const Geometry::Vec3D& Norm) const
  /*!
    Given a side: Calculate the intercept point on that surface
    \param sideIndex :: Side index [0-6]
    \param Pt :: Start point of line
    \param Norm :: Normal of line
    \return The point that the vector impacts the outside
  */
{
  ELog::RegMethod RegA("ChipIRHutch","calcSideIntercept");
  
  const Geometry::Plane* PX=
    SMap.realPtr<Geometry::Plane>(hutchIndex+sideIndex);

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
ChipIRHutch::calcSurfNormal(const int surfIndex) const
  /*!
    Nasty function to get the outer surface normal
    \param surfIndex :: Side to use
    \return The Normal from the surface
   */
{
  ELog::RegMethod RegA("ChipIRHutch","getSurfNormal");

  const Geometry::Plane* PX=
    SMap.realPtr<Geometry::Plane>(hutchIndex+surfIndex);

  if (!PX)
    throw ColErr::InContainerError<int>(surfIndex,
					"No surface "+RegA.getFull());
  return PX->getNormal();
}

Geometry::Vec3D
ChipIRHutch::calcDoorPoint() const
  /*!
    Calculate the point at the back of the door
    \return Back Plane point
   */
{
  ELog::RegMethod RegA("ChipIRHutch","calcBackPlane");
  
  // Combination of surf: 22/44/25
  Geometry::Vec3D Offset=Origin+
    X*(hFullRWidth-hRWallThick)+
    Y*(hExtLen+2.5)+
    Z*(hFloorDepth+hVoidFloor-hFloor);   
  return Offset;
}

void
ChipIRHutch::createLinks()
  /*!
    Create all the link points
  */
{
  
  return;
}

void
ChipIRHutch::createAll(Simulation& System,
		       const shutterSystem::GeneralShutter& ShutterPort,
		       const attachSystem::TwinComp& Guide,
		       const attachSystem::ContainedComp& IC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param ShutterPort :: Shutter to build beam along
    \param Guide :: Linear Comp to beam from [chipGuide]
    \param IC :: Internal Component that contains is own external perimeter
  */
{
  ELog::RegMethod RegA("ChipIRHutch","createAll");

  populate(System);
  Trimmer->createSurf(System,Guide);
  createUnitVector(ShutterPort,Guide);
  createWallSurfaces(Guide);

  //  createBeamStopObjects(System);
  createWallObjects(System,IC);

  Trimmer->createObj(System);
  addOuterVoid();
  addCollimators(System,Guide);
  BStop->createAll(System,*this);

  FTable->setFloor(SMap.realSurf(hutchIndex+35));
  FTable->addInsertCell(tailVoid);
  FTable->createAll(System,Guide);
  BTable->setFloor(SMap.realSurf(hutchIndex+35));
  BTable->addInsertCell(tailVoid);
  BTable->createAll(System,Guide);

  std::vector<ChipSample>::iterator vc;
  for(vc=SampleItems.begin();vc!=SampleItems.end();vc++)
    {
      vc->addInsertCell(tailVoid);
      vc->createAll(System,*FTable,*BTable);
    }
  layerProcess(System);
  insertObjects(System);       
  
  writeMasterPoints();
  return;
}


  
}  // NAMESPACE shutterSystem

