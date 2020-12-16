/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/ChipIRGuide.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "shutterBlock.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h" 
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h" 
#include "ExternalCut.h"
#include "GeneralShutter.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "BulkShield.h"
#include "ScatterPlate.h"
#include "ChipIRFilter.h"
#include "WallCut.h"
#include "ChipIRGuide.h"

namespace hutchSystem
{

ChipIRGuide::ChipIRGuide(const std::string& Key) : 
  attachSystem::FixedGroup(Key,"Main",12,"Beam",2),
  attachSystem::ContainedGroup("inner","outer","leftwall","rightwall"),
  Filter("chipFilter"),nLayers(0),
  nConcLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ChipIRGuide::ChipIRGuide(const ChipIRGuide& A) : 
  attachSystem::FixedGroup(A),attachSystem::ContainedGroup(A),
  Filter(A.Filter),beamAngle(A.beamAngle),sideBeamAngle(A.sideBeamAngle),
  shutterAngle(A.shutterAngle),gLen(A.gLen),hYStart(A.hYStart),
  hFWallThick(A.hFWallThick),xShift(A.xShift),zShift(A.zShift),
  xBeamShift(A.xBeamShift),zBeamShift(A.zBeamShift),
  innerARoofZ(A.innerARoofZ),innerAFloorZ(A.innerAFloorZ),
  innerBRoofZ(A.innerBRoofZ),innerBFloorZ(A.innerBFloorZ),
  innerALWall(A.innerALWall),innerARWall(A.innerARWall),
  innerBLWall(A.innerBLWall),innerBRWall(A.innerBRWall),
  LThick(A.LThick),LMat(A.LMat),roofSteel(A.roofSteel),
  floorSteel(A.floorSteel),leftSteelInner(A.leftSteelInner),
  leftSteelFlat(A.leftSteelFlat),rightSteelInner(A.rightSteelInner),
  rightSteelFlat(A.rightSteelFlat),leftSteelAngle(A.leftSteelAngle),
  rightSteelAngle(A.rightSteelAngle),roofConc(A.roofConc),
  floorConc(A.floorConc),leftConcInner(A.leftConcInner),
  rightConcInner(A.rightConcInner),rightConcFlat(A.rightConcFlat),

  leftConcAngle(A.leftConcAngle),rightConcAngle(A.rightConcAngle),
  blockWallThick(A.blockWallThick),blockWallHeight(A.blockWallHeight),
  blockWallLen(A.blockWallLen),extraWallThick(A.extraWallThick),
  extraWallHeight(A.extraWallHeight),extraWallLen(A.extraWallLen),
  extraWallSideAngle(A.extraWallSideAngle),extraWallEndAngle(A.extraWallEndAngle),
  rightWallThick(A.rightWallThick),rightWallHeight(A.rightWallHeight),
  rightWallLen(A.rightWallLen),nLayers(A.nLayers),
  guideFrac(A.guideFrac),guideMat(A.guideMat),nConcLayers(A.nConcLayers),
  concFrac(A.concFrac),concLayMat(A.concLayMat),steelMat(A.steelMat),
  concMat(A.concMat),wallMat(A.wallMat),monoWallSurf(A.monoWallSurf),
  voidCells(A.voidCells),layerCells(A.layerCells)
  /*!
    Copy constructor
    \param A :: ChipIRGuide to copy
  */
{}

ChipIRGuide&
ChipIRGuide::operator=(const ChipIRGuide& A)
  /*!
    Assignment operator
    \param A :: ChipIRGuide to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedGroup::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      Filter=A.Filter;
      beamAngle=A.beamAngle;
      sideBeamAngle=A.sideBeamAngle;
      shutterAngle=A.shutterAngle;
      gLen=A.gLen;
      hYStart=A.hYStart;
      hFWallThick=A.hFWallThick;
      xShift=A.xShift;
      zShift=A.zShift;
      xBeamShift=A.xBeamShift;
      zBeamShift=A.zBeamShift;
      innerARoofZ=A.innerARoofZ;
      innerAFloorZ=A.innerAFloorZ;
      innerBRoofZ=A.innerBRoofZ;
      innerBFloorZ=A.innerBFloorZ;
      innerALWall=A.innerALWall;
      innerARWall=A.innerARWall;
      innerBLWall=A.innerBLWall;
      innerBRWall=A.innerBRWall;
      LThick=A.LThick;
      LMat=A.LMat;
      roofSteel=A.roofSteel;
      floorSteel=A.floorSteel;
      leftSteelInner=A.leftSteelInner;
      leftSteelFlat=A.leftSteelFlat;
      rightSteelInner=A.rightSteelInner;
      rightSteelFlat=A.rightSteelFlat;
      leftSteelAngle=A.leftSteelAngle;
      rightSteelAngle=A.rightSteelAngle;
      roofConc=A.roofConc;
      floorConc=A.floorConc;
      leftConcInner=A.leftConcInner;
      rightConcInner=A.rightConcInner;
      rightConcFlat=A.rightConcFlat;
      leftConcAngle=A.leftConcAngle;
      rightConcAngle=A.rightConcAngle;
      blockWallThick=A.blockWallThick;
      blockWallHeight=A.blockWallHeight;
      blockWallLen=A.blockWallLen;
      extraWallThick=A.extraWallThick;
      extraWallHeight=A.extraWallHeight;
      extraWallLen=A.extraWallLen;
      extraWallSideAngle=A.extraWallSideAngle;
      extraWallEndAngle=A.extraWallEndAngle;
      rightWallThick=A.rightWallThick;
      rightWallHeight=A.rightWallHeight;
      rightWallLen=A.rightWallLen;
      nLayers=A.nLayers;
      guideFrac=A.guideFrac;
      guideMat=A.guideMat;
      nConcLayers=A.nConcLayers;
      concFrac=A.concFrac;
      concLayMat=A.concLayMat;
      steelMat=A.steelMat;
      concMat=A.concMat;
      wallMat=A.wallMat;
      monoWallSurf=A.monoWallSurf;
      voidCells=A.voidCells;
      layerCells=A.layerCells;
    }
  return *this;
}


ChipIRGuide::~ChipIRGuide() 
  /*!
    Destructor
  */
{}


void
ChipIRGuide::populateWallItems(const FuncDataBase& Control)
  /*!
    Populate the wall cut item variables
    \param Control :: Variable database 
  */
{
  ELog::RegMethod RegA("ChipIRGuide","popualateWallItems");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const int nActive=Control.EvalDefVar<int>(keyName+"CutsActive",1);
  if (nActive)
    {
      nCuts=Control.EvalVar<size_t>(keyName+"NCuts");
      WCObj.clear();
      for(size_t i=0;i<nCuts;i++)
	{
	  const std::string KN="CutKey"+std::to_string(i);
	  WCObj.push_back(std::shared_ptr<constructSystem::WallCut>
			  (new constructSystem::WallCut("CGCut",i)));
	  OR.addObject(WCObj.back());
	}
      // top to base slice
      WCObj.push_back(std::shared_ptr<constructSystem::WallCut>
		      (new constructSystem::WallCut("CGSlice",0)));
      OR.addObject(WCObj.back());
    }
  return;
}
  
void
ChipIRGuide::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("ChipIRGuide","populate");

  beamAngle=Control.EvalVar<double>(keyName+"Angle");
  sideBeamAngle=Control.EvalVar<double>(keyName+"SideAngle");
  hYStart=Control.EvalVar<double>("chipHutYStart");
  hFWallThick=Control.EvalVar<double>("chipHutFWallThick");

  xShift=Control.EvalVar<double>(keyName+"XShift");
  zShift=Control.EvalVar<double>(keyName+"ZShift");

  xBeamShift=Control.EvalVar<double>(keyName+"XBeamShift");
  zBeamShift=Control.EvalVar<double>(keyName+"ZBeamShift");

  innerARoofZ=Control.EvalVar<double>(keyName+"InnerARoofZ");
  innerAFloorZ=Control.EvalVar<double>(keyName+"InnerAFloorZ");
  innerBRoofZ=Control.EvalVar<double>(keyName+"InnerBRoofZ");
  innerBFloorZ=Control.EvalVar<double>(keyName+"InnerBFloorZ");

  innerALWall=Control.EvalVar<double>(keyName+"InnerALWall");
  innerARWall=Control.EvalVar<double>(keyName+"InnerARWall");
  innerBLWall=Control.EvalVar<double>(keyName+"InnerBLWall");
  innerBRWall=Control.EvalVar<double>(keyName+"InnerBRWall");

  // Add beamshift [+ve in Z and +ve (right) in X]:

  // Liner
  const int NL=Control.EvalVar<int>(keyName+"NLiner");
  for(int i=0;i<NL;i++)
    {
      const std::string keyIndex=keyName+"Liner"+std::to_string(i+1);
      LThick.push_back
	(Control.EvalPair<double>(keyIndex+"Thick",keyName+"LinerThick"));
      LMat.push_back
	(ModelSupport::EvalMat<int>(Control,keyIndex+"Mat",keyName+"LinerMat"));

    }

  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  // Steel lines:
  roofSteel=Control.EvalVar<double>(keyName+"RoofSteel");
  floorSteel=Control.EvalVar<double>(keyName+"FloorSteel");
  leftSteelFlat=Control.EvalVar<double>(keyName+"LeftSteel");
  rightSteelFlat=Control.EvalVar<double>(keyName+"RightSteel");
  leftSteelInner=Control.EvalVar<double>(keyName+"LeftSteelInner");
  rightSteelInner=Control.EvalVar<double>(keyName+"RightSteelInner");
  leftSteelAngle=Control.EvalVar<double>(keyName+"LeftSteelAngle");
  rightSteelAngle=Control.EvalVar<double>(keyName+"RightSteelAngle");

  // Conc lines:
  roofConc=Control.EvalVar<double>(keyName+"RoofConc");
  floorConc=Control.EvalVar<double>(keyName+"FloorConc");
  rightConcFlat=Control.EvalVar<double>(keyName+"RightConc");
  leftConcInner=Control.EvalVar<double>(keyName+"LeftConcInner");
  rightConcInner=Control.EvalVar<double>(keyName+"RightConcInner");
  leftConcAngle=Control.EvalVar<double>(keyName+"LeftConcAngle");
  rightConcAngle=Control.EvalVar<double>(keyName+"RightConcAngle");

  // Divide Wall 
  blockWallThick=Control.EvalVar<double>(keyName+"BlockWallThick");
  blockWallHeight=Control.EvalVar<double>(keyName+"BlockWallHeight");
  blockWallLen=Control.EvalVar<double>(keyName+"BlockWallLength");
  
  // Extra shielding block on TSA
  extraWallThick=Control.EvalVar<double>(keyName+"ExtraWallThick");
  extraWallHeight=Control.EvalVar<double>(keyName+"ExtraWallHeight");
  extraWallLen=Control.EvalVar<double>(keyName+"ExtraWallLength");
  extraWallSideAngle=Control.EvalVar<double>(keyName+"ExtraWallSideAngle");
  extraWallEndAngle=Control.EvalVar<double>(keyName+"ExtraWallEndAngle");
  // Extra shielding block on W2 side
  rightWallThick=Control.EvalVar<double>(keyName+"RightWallThick");
  rightWallHeight=Control.EvalVar<double>(keyName+"RightWallHeight");
  rightWallLen=Control.EvalVar<double>(keyName+"RightWallLength");


  remedialVoid=Control.EvalVar<int>(keyName+"RemedialVoid");
  remedialWestWallThick=Control.EvalVar<double>(keyName+"RemedialWallThick");
  remedialWallHeight=Control.EvalVar<double>(keyName+"RemedialWallHeight");


  populateWallItems(Control);

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  nConcLayers=Control.EvalDefVar<size_t>(keyName+"NConcLayers",0);

  // SteelLayers IS BASIC:
  ModelSupport::populateDivide(Control,nLayers,keyName+"SteelMat_",
			       steelMat,guideMat);
  ModelSupport::populateDivide(Control,nLayers,
   			       keyName+"SteelFrac_",guideFrac);

  // Concreate :
  ModelSupport::populateDivide(Control,nConcLayers,keyName+"ConcMat_",
			       concMat,concLayMat);
  ModelSupport::populateDivide(Control,nLayers,
   			       keyName+"ConcFrac_",concFrac);

  // Remedial layers
  nRemedialWestLayers=Control.EvalVar<size_t>(keyName+"RemedialWestNLayers");
  // Concreate :
  ModelSupport::populateDivide(Control,nRemedialWestLayers,
			       keyName+"WRemedMat_",
			       steelMat,remedialWestMat);
  ModelSupport::populateDivideLen(Control,nRemedialWestLayers,
				  keyName+"WRemedLen_",
				  remedialWestWallThick,
				  remedialWestFrac);


  return;
}

void
ChipIRGuide::createUnitVector(const shutterSystem::BulkShield& BS,
			      const shutterSystem::GeneralShutter& GS)
  /*!
    Create the unit vectors
    \param BS :: Bulk Shield [ for outside distance]
    \param GS :: shutter direction
  */
{
  ELog::RegMethod RegA("ChipIRGuide","createUnitVector");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  // Gravity axis [up]
  Geometry::Vec3D tmpbZ= Geometry::Vec3D(-1,0,0); 
  Geometry::Vec3D tmpbY= GS.getXYAxis().unit();
  Geometry::Vec3D tmpbX= tmpbZ * tmpbY;
  // Change so that not dependent on the angle of the shutter:

  mainFC.createUnitVector(GS.getKey("Main"));
  setDefault("Main");
  mainFC.createUnitVector(Origin,-X,Y,Z);
  setDefault("Main");
  const Geometry::Vec3D OXX=GS.getOrigin()+Y*BS.getORadius()
    +X*xBeamShift+Z*(zBeamShift+zShift);
  
  mainFC.applyShift(xShift,BS.getORadius(),zShift);
  beamFC.createUnitVector(OXX,tmpbX,tmpbY,tmpbZ);  
  
  //  mainFC.setCentre(Origin);
  //  bEnter+=Z*zBeamShift+X*xBeamShift;

  // Rotate beamAxis to the final angle

  //  beamFC.applyShift(-xBeamShift,0.0,-zBeamShift);


  Geometry::Quaternion::calcQRotDeg(beamAngle,-X).rotate(tmpbX);
  Geometry::Quaternion::calcQRotDeg(beamAngle,-X).rotate(tmpbZ);
  Geometry::Quaternion::calcQRotDeg(beamAngle,-X).rotate(tmpbZ);
  Geometry::Quaternion::calcQRotDeg(beamAngle,-X).rotate(tmpbY);
  Geometry::Quaternion::calcQRotDeg(sideBeamAngle,Z).rotate(tmpbX);
  Geometry::Quaternion::calcQRotDeg(sideBeamAngle,Z).rotate(tmpbY);

  
  beamFC.createUnitVector(OXX,tmpbX,tmpbY,tmpbZ);
  ELog::EM<<"BXYZ == "<<beamFC.getX()<<":"<<bY<<":"<<bZ<<ELog::endDiag;
  
  // Now calculate Cent
  setDefault("Main","Beam");
  gLen=hYStart-BS.getORadius();
  beamFC.setExit(bOrigin+bY*(gLen/std::abs(bY.dotProd(Y))),bY);

  setDefault("Main","Beam");
  return;
}

void
ChipIRGuide::createUnitVector(const attachSystem::FixedComp& WO,
			      const double ORadius)
  /*!
    Create the unit vectors
    \param WO :: Fixed Unit (Base origin)
    \param ORadius :: Outer Radius since hlen is fixed to centre coordinates
  */
{
  ELog::RegMethod RegA("ChipIRGuide","createUnitVector(Fixed)");


  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.createUnitVector(WO);
  beamFC.createUnitVector(WO);

  beamFC.applyAngleRotate(sideBeamAngle,beamAngle);

  setDefault("Main","Beam");
  
  // Now calculate Cent
  gLen=hYStart-ORadius; 
  // Output Datum [beam centre]
  // Distance to Y Plane [ gLen / (beamAxis . Y )


  beamFC.setExit(bOrigin+bY*(hYStart/std::abs(bY.dotProd(Y))),
		 bY);
  //  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();
  //  CS.setDNum(chipIRDatum::guideExit,MR.calcRotate(getExit()));
  //  CS.setDNum(chipIRDatum::floodC,MR.calcRotate(getExit()-bY*210.0));
  
  return;
}

void
ChipIRGuide::createLiner(const int index,const double offset)
  /*!
    Create an inner section with an offset
    \param index :: Index number
    \param offset :: offset distance
  */
{
  ELog::RegMethod RegA("ChipIRGuide","createLiner");

  const int GI(buildIndex+index);
  // INNER VOID CORE [+ve X : +ve z]
  ModelSupport::buildPlane(SMap,GI+3,
			   bOrigin-bX*(innerALWall+offset),
			   bOrigin-bX*(innerALWall+offset)+bY*gLen,
			   bOrigin-bX*(innerALWall+offset)+bZ,X);
  ModelSupport::buildPlane(SMap,GI+4,
			   bOrigin+bX*(innerARWall+offset),
			   bOrigin+bX*(innerBRWall+offset)+bY*gLen,
			   bOrigin+bX*(innerARWall+offset)+bZ,X);
  ModelSupport::buildPlane(SMap,GI+5,
			   bOrigin-bZ*(innerAFloorZ+offset),
			   bOrigin-bZ*(innerBFloorZ+offset)+bY*gLen,
			   bOrigin-bZ*(innerAFloorZ+offset)+bX,Z);
  ModelSupport::buildPlane(SMap,GI+6,
			   bOrigin+bZ*(innerARoofZ+offset),
			   bOrigin+bZ*(innerBRoofZ+offset)+bY*gLen,
			   bOrigin+bZ*(innerARoofZ+offset)+bX,Z);

  return;
}


void
ChipIRGuide::createSurfaces(const shutterSystem::GeneralShutter& GS)
  /*!
    Create All the surfaces :
    Link surfaces : 1 : 2 are  2 and 1002
    \param GS :: GeneralShutter [for divide]
  */
{
  ELog::RegMethod RegA("ChipIRGuide","createSurface(GeneralShutter)");
 
  SMap.addMatch(buildIndex+100,GS.getDivideSurf());
  SMap.addMatch(buildIndex+1,monoWallSurf);
  createSurfacesCommon();

  return;
}

void
ChipIRGuide::createSurfaces()
  /*!
    Create All the surfaces :
    \param FC :: FixedComp unit
  */
{
  ELog::RegMethod RegA("ChipIRGuide","createSurface(void)");
  const double surfRadius(200.0);
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin-Y*surfRadius,-Y);
  ModelSupport::buildCylinder(SMap,buildIndex+1,
			      Origin-Y*surfRadius,Z,surfRadius);
  createSurfacesCommon();

  return;
}

void
ChipIRGuide::createSurfacesCommon()
  /*!
    Create All the surfaces :
  */
{
  ELog::RegMethod RegA("ChipIRGuide","createSurfaceCommon");
 
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*gLen,Y);

  ModelSupport::buildPlane(SMap,buildIndex+1002,
			   Origin+Y*(gLen-hFWallThick),Y);

  createLiner(0,0.0); // Inner vacuum
  for(size_t i=0;i<LThick.size();i++)
    createLiner(static_cast<int>(i+1)*20,LThick[i]);

  // Steel Work:
  //  [sides]
  Geometry::Vec3D rX(X);
  Geometry::Quaternion::calcQRotDeg(-leftSteelAngle,Z).rotate(rX);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*leftSteelInner,rX);
  rX=X;
  Geometry::Quaternion::calcQRotDeg(rightSteelAngle,Z).rotate(rX);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*rightSteelInner,rX);
  ModelSupport::buildPlane(SMap,buildIndex+113,
			   Origin-X*leftSteelFlat,X);
  ModelSupport::buildPlane(SMap,buildIndex+114,
			   Origin+X*rightSteelFlat,X);
  // Steel follows guide vertically:
  ModelSupport::buildPlane(SMap,buildIndex+115,
			   Origin-Z*floorSteel,bZ);
  ModelSupport::buildPlane(SMap,buildIndex+116,
			   Origin+Z*roofSteel,bZ);

  // Concrete:
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*rightConcFlat,X);
  ModelSupport::buildPlane(SMap,buildIndex+205,Origin-Z*floorConc,Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*roofConc,Z);

  // Left Wall
  rX=X;
  Geometry::Quaternion::calcQRotDeg(-leftConcAngle,Z).rotate(rX);  
  ModelSupport::buildPlane(SMap,buildIndex+213,
			   Origin-X*leftConcInner,rX);
  // Block wall [Left only]:
  ModelSupport::buildPlane(SMap,buildIndex+303,
			   Origin-X*(leftConcInner+blockWallThick),rX);
  ModelSupport::buildPlane(SMap,buildIndex+306,
			   Origin+Z*(blockWallHeight-floorConc),Z);
  ModelSupport::buildPlane(SMap,buildIndex+302,Origin+
			   Y*(gLen+blockWallLen),Y);

  // Extra wall on left:
  rX=X;
  Geometry::Quaternion::calcQRotDeg(-extraWallSideAngle,Z).rotate(rX);  
  ModelSupport::buildPlane(SMap,buildIndex+403,Origin-X*
			   (leftConcInner+blockWallThick+extraWallThick),rX);
  ModelSupport::buildPlane(SMap,buildIndex+406,
			   Origin+Z*(extraWallHeight-floorConc),Z);
  // Dont add gLen to extra wall - can be shorter than guide
  rX=X;
  Geometry::Quaternion::calcQRotDeg(extraWallEndAngle,Z).rotate(rX);  
  ModelSupport::buildPlane(SMap,buildIndex+402,Origin+Y*extraWallLen,rX);
  

  // Right Wall [W2 side]
  rX=X;
  Geometry::Quaternion::calcQRotDeg(rightConcAngle,Z).rotate(rX);  
  ModelSupport::buildPlane(SMap,buildIndex+214,
			   Origin+X*rightConcInner,rX);


  // Extra wall on right [W2 side]:
  ModelSupport::buildPlane(SMap,buildIndex+503,
			   Origin+X*(rightConcInner+rightWallThick),rX);
  ModelSupport::buildPlane(SMap,buildIndex+506,
			   Origin+Z*(rightWallHeight-floorConc),Z);
  ModelSupport::buildPlane(SMap,buildIndex+502,Origin+
			   Y*rightWallLen,Y);

  // RemedialWall wall on right [W2 side]:
  // extended out from 214/204
  ModelSupport::buildPlane(SMap,buildIndex+604,
			   Origin+X*(rightConcFlat+remedialWestWallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+614,
			   Origin+X*(rightConcInner+remedialWestWallThick),rX);
  ModelSupport::buildPlane(SMap,buildIndex+606,
			   Origin+Z*(remedialWallHeight-floorConc),Z);


  // Wedge shielding piece on TSA side
  //  rX=X;
  //  Geometry::Quaternion::calcQRotDeg(leftWedgeAngle,Z).rotate(rX);  
  //  ModelSupport::buildPlane(SMap,buildIndex+603,
  //			   Origin-X*(leftConcInner+blockWallThick+
  //				     extraWallThick+leftWedgeThick),rX);
  //ModelSupport::buildPlane(SMap,buildIndex+606,
  //                          Origin+Z*(leftWedgeHeight-floorConc),Z);

  return;
}

int
ChipIRGuide::calcTallyPlanes(const int frontFlag,
			     std::vector<int>& Planes) const
  /*!
    Calculate the front/back plane 
    \param frontFlag :: Front(1) / Back(-1) 
    \param Planes :: Vector of edge surface
    \return front / back plane
  */
{
  Planes.clear();
  for(int i=3;i<=6;i++)
    Planes.push_back(SMap.realSurf(buildIndex+i));
  Planes.push_back(-SMap.realSurf(buildIndex+100));
  return (frontFlag==1) ? 
    SMap.realSurf(buildIndex+1) : SMap.realSurf(buildIndex+2);
}

void
ChipIRGuide::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ChipIRGuide","createObjects");
  const int rConcMat=(remedialVoid) ? 0 : concMat;
      
  std::string Out;
  // Inner void:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-100 1 -2 3 -4 5 -6");
  voidCells.push_back(cellIndex);
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  // Cell liner:
  const std::string BasicOut=
    ModelSupport::getComposite(SMap,buildIndex,"-100 1 -2 ");
  int GI(buildIndex);
  for(size_t i=0;i<LMat.size();i++)
    {
      Out=BasicOut+
	ModelSupport::getComposite(SMap,GI," (-3:4:-5:6) ");
      GI+=20;
      Out+=ModelSupport::getComposite(SMap,GI," 3 -4 5 -6 ");
      voidCells.push_back(cellIndex);
      System.addCell(MonteCarlo::Object(cellIndex++,LMat[i],0.0,Out));
    }
  // Outer layer
  Out=ModelSupport::getComposite(SMap,GI," (-3:4:-5:6) ");
    
  // First Layer
  Out+=ModelSupport::getComposite(SMap,buildIndex,
       "-100 1 -2 13 113 -14 -114 115 -116 ");
  System.addCell(MonteCarlo::Object(cellIndex++,steelMat,0.0,Out));
  voidCells.push_back(cellIndex-1);
  layerCells.insert(LCTYPE::value_type("SteelInner",cellIndex-1));

  // Concrete:
  // Roof:
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -1002 116 -206 213 -204 -214");
  System.addCell(MonteCarlo::Object(cellIndex++,concMat,0.0,Out));
  voidCells.push_back(cellIndex-1);

  // Floor
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -1002 -115 205 213 -204 -214 ");
  System.addCell(MonteCarlo::Object(cellIndex++,concMat,0.0,Out));
  voidCells.push_back(cellIndex-1);

  // Right Wall
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100  1 -1002 115 -116 (14:114) -204 -214 ");
  System.addCell(MonteCarlo::Object(cellIndex++,concMat,0.0,Out));
  //  voidCells.push_back(cellIndex-1);
  layerCells.insert(LCTYPE::value_type("ConcRight",cellIndex-1));

  // Left Wall
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -1002 115 -116 (-13:-113) 213 ");
  System.addCell(MonteCarlo::Object(cellIndex++,concMat,0.0,Out));
  voidCells.push_back(cellIndex-1);
  layerCells.insert(LCTYPE::value_type("ConcLeft",cellIndex-1));

  // BlockWall
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -302 205 -306 -213 303 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -302 306 -206 -213 303 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  // Extra left Wall
  // everything below wall height to limit of wall length
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 402 205 -406 -303 403 ");
  System.addCell(MonteCarlo::Object(cellIndex++,concMat,0.0,Out));
  //above wall height, below roof
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -302 406 -206 -303 403 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  // below wall height beyond end of wall length

  // Extra right Wall [W2 side]
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -502 205 -506 214 -503 ");
  System.addCell(MonteCarlo::Object(cellIndex++,concMat,0.0,Out));

  // REMEDIAL WALL West [W2 side]
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -1002 (214:204) (502:503:506) "
				 "205 -606 -604 -614 ");
  System.addCell(MonteCarlo::Object(cellIndex++,rConcMat,0.0,Out));
  layerCells.insert(LCTYPE::value_type("RWConcW2",cellIndex-1));

    // REMEDIAL ROOF to the wall
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -1002 -214 -204 213 206 -606");
  System.addCell(MonteCarlo::Object(cellIndex++,rConcMat,0.0,Out));
  layerCells.insert(LCTYPE::value_type("RWConcRoof",cellIndex-1));



 //above wall height, below roof
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -502 506 -206 214 -503 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  // Add full outer system
  // Inner 
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -2 113 -114 115 -116 ");
  addOuterSurf("inner",Out);  

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-100 1 -1002 213 -204 -214 205 -606 ");
  addOuterSurf("outer",Out);  
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
                                 "-100 1 205 -206 -213 -302 403 (402:303)");
  addOuterSurf("leftwall",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
                                 "-100 1 -1002 -614 -604 (214:204) 205 -606 ");
  addOuterSurf("rightwall",Out);

  return;
}

void
ChipIRGuide::addInsertPlate(Simulation& System)
  /*!
    Adds an insert plate into the system.
    \param System :: Simulation to add to     
  */
{
  ELog::RegMethod RegA("ChipIRGuide","addInsertPlate");

  ScatterPlate SP("chipGPlate");
  SP.addBoundarySurf(ModelSupport::getComposite(SMap,buildIndex,"3 -4 5 -6"));
  if (!voidCells.empty())
    SP.addInsertCell(voidCells.front());

  SP.createAll(System,*this,0);
  return;
}


void 
ChipIRGuide::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("ChipIRGuide","layerProcess");
  // Steel layers
  //  layerSpecial(System);

  if (nLayers>1)
    {
      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nLayers;i++)
	{
	  DA.addFrac(guideFrac[i-1]);
	  DA.addMaterial(guideMat[i-1]);
	}
      DA.addMaterial(guideMat.back());
      
      // Cell Specific:
      DA.setCellN(layerCells["SteelInner"]);
      DA.setOutNum(cellIndex,buildIndex+801);
      const int linerOffset(buildIndex
			    +20*static_cast<int>(LThick.size()));

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;

      surroundRule.setSurfPair(SMap.realSurf(linerOffset+3),
			       SMap.realSurf(buildIndex+13));
      surroundRule.setSurfPair(SMap.realSurf(linerOffset+3),
			       SMap.realSurf(buildIndex+113));
      surroundRule.setSurfPair(SMap.realSurf(linerOffset+4),
			       SMap.realSurf(buildIndex+14));
      surroundRule.setSurfPair(SMap.realSurf(linerOffset+4),
			       SMap.realSurf(buildIndex+114));
      surroundRule.setSurfPair(SMap.realSurf(linerOffset+5),
			       SMap.realSurf(buildIndex+115));
      surroundRule.setSurfPair(SMap.realSurf(linerOffset+6),
			       SMap.realSurf(buildIndex+116));
      OutA=ModelSupport::getComposite(SMap,linerOffset," (-3:4:-5:6) ");
      OutB=ModelSupport::getComposite(SMap,buildIndex,
				      " 13 113 -14 -114 115 -116 ");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);

      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);

      cellIndex=DA.getCellNum();
    }
  // ---------------
  // Concrete layers
  // ---------------
  if (nConcLayers>1)
    {
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nConcLayers;i++)
	{
	  DA.addFrac(concFrac[i-1]);
	  DA.addMaterial(concLayMat[i-1]);
	}
      DA.addMaterial(concLayMat.back());

      const std::vector<std::pair<int,int> > 
	AX={{14,214},{114,204}};
      const std::vector<std::pair<int,int> > 
	BX={{213,13},{213,113}};
	
      cellIndex=DA.procSurfDivide(System,SMap,layerCells["ConcRight"],
			buildIndex,1600,cellIndex,AX,"(14:114)","-214 -204");
      cellIndex=DA.procSurfDivide(System,SMap,layerCells["ConcLeft"],
			buildIndex,2400,cellIndex,BX,"213","(-13:-113)");
    }

  // ----------------------
  // REMEDIAL WALL Divider:
  // ----------------------
  if (nRemedialWestLayers>1 && !remedialVoid)
    {
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nRemedialWestLayers;i++)
	{
	  DA.addFrac(remedialWestFrac[i-1]);
	  DA.addMaterial(remedialWestMat[i-1]);
	}
      DA.addMaterial(remedialWestMat.back());

      const std::vector<std::pair<int,int> > 
	AX={{214,614},{204,604}};
      cellIndex=DA.procSurfDivide(System,SMap,layerCells["RWConcW2"],
			buildIndex,3000,cellIndex,AX,
			"(204:214)","-614 -604");
    }
  
  return;
}
      
void
ChipIRGuide::writeMasterPoints()
  /*!
    Write out the master points
  */
{
  ELog::RegMethod RegA("ChipIRGuide","writeMasterPoints");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  
  const masterRotate& MR=masterRotate::Instance();  
  // Output Datum
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();
  
  // Get surface from around the guide
  std::vector<Geometry::Plane*> boxSurf;
  // not realPtr throws
  for(int i=0;i<4;i++)
    boxSurf.push_back(SMap.realPtr<Geometry::Plane>(buildIndex+3+i));

  // Points are a-b, c-d hence swap b/c
  std::swap(boxSurf[1],boxSurf[2]);

  // get front and back
  const Geometry::Surface* FSurf=SMap.realSurfPtr(buildIndex+1);
  const Geometry::Surface* BSurf=SMap.realSurfPtr(buildIndex+2);
  const Geometry::Surface* testSurf=SMap.realSurfPtr(buildIndex+100);

  std::vector<Geometry::Vec3D> Out;
  // Calculate the points at the guides  [FrontSurf]
  for(size_t i=0;i<4;i++)
    {
      Out.push_back
	(SurInter::getPoint(boxSurf[i],boxSurf[(i+1) % 4],
			    FSurf,-1,testSurf));
    }  

  // Calculate the points at the guides [BackSurf]
  for(size_t i=0;i<4;i++)
    {
      Out.push_back
	(SurInter::getPoint(boxSurf[i],boxSurf[(i+1) % 4],
			    BSurf,-1,testSurf));
    }  
  // set points on the datum
  chipIRDatum::CPENUM A(chipIRDatum::guidePt1);
  for(size_t i=0;i<8;i++)
    CS.setCNum(A+i,MR.calcRotate(Out[i]));   
  
  // Now make connection points:

  // Rotates round base/right/top/left:
  int gIndex[]={5,4,6,3};
  size_t cNum[]={4,3,5,2};
  for(size_t i=0;i<4;i++)
    {
      const double signV((i>2) ? -1.0 : 1.0);
      Geometry::Vec3D Cp=
	Out[i]+Out[(i+1) % 4]+Out[i+4]+Out[4+((i+1) % 4)];
      Cp/=4.0;
      const Geometry::Plane* PX=
	SMap.realPtr<Geometry::Plane>(gIndex[i]+buildIndex);
      mainFC.setConnect(cNum[i],Cp,PX->getNormal()*signV);      
    }
  
  return;
}

void
ChipIRGuide::addFilter(Simulation& System)
  /*!
    Add the filter unit to the guide
    \param System :: Simulation to add
  */
{
  ELog::RegMethod RegA("ChipIRGuide","addFilter");

  for(const int& VC : voidCells)
    Filter.addInsertCell(VC);

  Filter.createAll(System,*this,0);
  return;
}
 
void
ChipIRGuide::createLinks()
  /*!
    Create the links for ChipIR
   */
{
  ELog::RegMethod RegA("ChipIRGuide","createLinks");
  // Set directional exit:
  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.setConnect(0,Origin,-Y);
  mainFC.setConnect(1,Origin+Y*gLen,Y);
  mainFC.setConnect(6,Origin+Y*(gLen-hFWallThick),Y);

  mainFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));  
  mainFC.setLinkSurf(2,-SMap.realSurf(buildIndex+3));  
  mainFC.setLinkSurf(3,SMap.realSurf(buildIndex+4));  
  mainFC.setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  mainFC.setLinkSurf(5,SMap.realSurf(buildIndex+6));  

  mainFC.setLinkSurf(6,SMap.realSurf(buildIndex+1002));

  // OutersideWalls
  mainFC.setLinkSurf(7,-SMap.realSurf(buildIndex+213));
  mainFC.setLinkSurf(8,SMap.realSurf(buildIndex+604));
      



  return;
}

int
ChipIRGuide::exitWindow(const double Dist,
			std::vector<int>& window,
			Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Distance from window
    \param window :: window vector of paired planes
    \param Pt :: Point for 
    \return Point at exit + Dist
  */
{
  window.clear();
  // Not valid numbers:
  window.push_back(SMap.realSurf(buildIndex+23));
  window.push_back(SMap.realSurf(buildIndex+24));
  window.push_back(SMap.realSurf(buildIndex+25));
  window.push_back(SMap.realSurf(buildIndex+26));
  // Note cant rely on exit point because that is the 
  // virtual 46 degree exit point.
  //  Pt=getExit()+bY*Dist; 
  return SMap.realSurf(buildIndex+2);
}



void
ChipIRGuide::addWallCuts(Simulation& System)
  /*!
    Add wallcuts if necessary
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("ChipIRGuide","makeWallCuts");

  const int GI(buildIndex+static_cast<int>(LMat.size())*20);
  std::string Out;

  for(std::shared_ptr<constructSystem::WallCut> WC : WCObj)
    {
      WC->populateKey(System.getDataBase());
      const std::string kN=WC->getInsertKey();
      if (kN=="SteelInnerRight")
	{
	  WC->addInsertCell(layerCells["SteelInner"]);
	  Out=ModelSupport::getComposite(SMap,buildIndex,GI,"4M -14");
	}
      else if (kN=="SteelInnerLeft")
	{
	  WC->addInsertCell(layerCells["SteelInner"]);
	  Out=ModelSupport::getComposite(SMap,buildIndex,GI,"-3M 13");
	}
      else if (kN=="SteelOuterLeft")
	{
	  WC->addInsertCell(layerCells["SteelInner"]);
	  Out=ModelSupport::getComposite(SMap,buildIndex,GI,"-3M 113");
	}
      else if (kN=="SteelInner")
	{
	  WC->addInsertCell(layerCells["SteelInner"]);
	  Out="";
	}
      else if (kN=="SteelRightVertical")
	{
	  WC->addInsertCell(layerCells["SteelInner"]);
	  Out=ModelSupport::getComposite(SMap,buildIndex," -14 -114 115 -116");
	}

      else
	{
	  throw ColErr::InContainerError<std::string>(WC->getInsertKey(),
						    "InsertKey");
	}

      HeadRule Boundary(Out);
      Boundary.populateSurf();
      WC->setCutSurf("WallBoundary",Boundary);
      WC->createAll(System,*this,0);
    }
  
  return;
}

void
ChipIRGuide::createCommon(Simulation& System)
  /*!
    Generic function to create common stuff
    \param System :: Simulation item
  */
{
  ELog::RegMethod RegA("ChipIRGuide","createCommon");

  createObjects(System);  
  createLinks();
  
  addInsertPlate(System);
  addFilter(System);
  writeMasterPoints();

  addWallCuts(System);
  layerProcess(System);
  insertObjects(System);   

  return;
}
  
void
ChipIRGuide::createAll(Simulation& System,
		       const shutterSystem::BulkShield& BS,
		       const size_t GIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param BS :: Bulk shield
    \param GIndex :: Guide index
  */
{
  ELog::RegMethod RegA("ChipIRGuide","createAll");
  
  const shutterSystem::GeneralShutter* GPtr=BS.getShutter(GIndex);
  populate(System.getDataBase());
  createUnitVector(BS,*GPtr);
  createSurfaces(*GPtr);

  createCommon(System);
  return;
}

void
ChipIRGuide::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp for origin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("ChipIRGuide","createAll[FC]");

  const FuncDataBase& Control=System.getDataBase();
  populate(Control);
  
  const double ORadius=
    Control.EvalDefVar<double>("bulkOuterRadius",600.1);
  createUnitVector(FC,ORadius);
  createSurfaces();

  createCommon(System);
  return;
}
  
}  // NAMESPACE hutchSystem

