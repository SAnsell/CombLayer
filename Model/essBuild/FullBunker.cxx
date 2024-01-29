/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/FullBunker.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "surfDIter.h"
#include "SurInter.h"

#include "BunkerRoof.h"
#include "BunkerWall.h"
#include "FullBunker.h"


namespace essSystem
{

FullBunker::FullBunker(const std::string& Key)  :
  attachSystem::FixedComp(Key,12),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  roofObj(new BunkerRoof(Key)),
  wallObj(new BunkerWall(Key)),
  revFlag(false)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(roofObj);
  OR.addObject(wallObj);
}

FullBunker::~FullBunker() 
  /*!
    Destructor
  */
{}

void
FullBunker::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("FullBunker","populate");

  leftPhase=Control.EvalVar<double>(keyName+"LeftPhase");
  rightPhase=Control.EvalVar<double>(keyName+"RightPhase");
  leftAngle=Control.EvalVar<double>(keyName+"LeftAngle");
  rightAngle=Control.EvalVar<double>(keyName+"RightAngle");

  wallRadius=Control.EvalVar<double>(keyName+"WallRadius");
  floorDepth=Control.EvalVar<double>(keyName+"FloorDepth");
  roofHeight=Control.EvalVar<double>(keyName+"RoofHeight");
  wallHeight=Control.EvalDefVar<double>(keyName+"WallHeight",roofHeight);

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");
  
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  ModelSupport::populateAddRange(Control,nLayers,keyName+"WallLen",
  			      wallRadius,wallRadius+wallThick,wallFrac);
  for(size_t i=0;i<wallFrac.size();i++)
    wallFrac[i]=(wallFrac[i]-wallRadius)/wallThick;

  nSectors=Control.EvalVar<size_t>(keyName+"NSectors");
  nSectors=std::max<size_t>(nSectors,1);
  ModelSupport::populateRange(Control,nSectors,keyName+"SectAngle",
			      leftPhase,rightPhase,sectPhase);

  nSegment=Control.EvalDefVar<size_t>(keyName+"NSegment",0);  
  ModelSupport::populateRange(Control,nSegment,keyName+"SegDivide",
			      0,1.0,segDivide);

  // BOOLEAN NUMBER!!!!!!!
  nVert=Control.EvalVar<size_t>(keyName+"NVert");
  midZ=Control.EvalDefVar<double>(keyName+"MidZ",0.0);
  ModelSupport::populateQuadRange(Control,nVert,keyName+"VertLen",
				  -floorDepth,midZ,roofHeight,vertFrac);
  for(size_t i=0;i<vertFrac.size();i++)
    vertFrac[i]=(vertFrac[i]+floorDepth)/(floorDepth+roofHeight);
  
  // SIDE LAYERS:
  
  sideFlag=Control.EvalDefVar<int>(keyName+"SideFlag",0);
  nSide=Control.EvalVar<size_t>(keyName+"NSide");
  ModelSupport::populateAddRange(Control,nSide,keyName+"SideThick",
				 0.0,sideThick,sideFrac);
  nSideVert=Control.EvalVar<size_t>(keyName+"NSideVert");
  ModelSupport::populateQuadRange(Control,nSideVert,keyName+"SideVert",
				  -floorDepth,midZ,roofHeight,sideVertFrac);
  nSideThick=Control.EvalVar<size_t>(keyName+"NSideThick");
  ModelSupport::populateAddRange(Control,nSideVert,keyName+"SideThick",
				 0,wallThick,sideThickFrac);
  
  return;
}
  
void
FullBunker::createUnitVector(const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: Side for linkage centre
  */
{
  ELog::RegMethod RegA("FullBunker","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  if (revFlag) X*=-1;
  return;
}


void
FullBunker::calcSegPosition(const size_t segIndex,
                        Geometry::Vec3D& DPosition,
                        Geometry::Vec3D& DX,
                        Geometry::Vec3D& DY,
                        Geometry::Vec3D& DZ) const
  /*!
    Calculate the position of the mid segments
    \param segIndex :: segment index
    \param DPosition :: Position of centre of wall [inner]
    \param DX :: Direction of X 
    \param DY :: Direction of Y
    \param DZ :: Direction of Z
  */
{
  ELog::RegMethod RegA("FullBunker","calcSegPosition");

  const double phaseDiff(rightPhase-leftPhase);
  const double angleDiff(rightAngle-leftAngle);
  
  const double midPhaseStep((segIndex+1<nSectors) ?
                            (sectPhase[segIndex+1]-sectPhase[segIndex+1])/2.0 :
                            (sectPhase[segIndex]-sectPhase[segIndex-1])/2.0 );
                            
  const double F= (midPhaseStep-leftPhase)/phaseDiff;
  const double angle= leftAngle+F*angleDiff;
  
  DPosition=Origin-rotCentre;
  Geometry::Quaternion::calcQRotDeg(sectPhase[segIndex],-Z).rotate(DPosition);
  DPosition+=rotCentre;
      
  DX=X;
  DY=Y;
  DZ=Z;
  Geometry::Quaternion::calcQRotDeg(sectPhase[segIndex]+angle,-Z).rotate(DX);
  Geometry::Quaternion::calcQRotDeg(sectPhase[segIndex]+angle,-Z).rotate(DY);

  DPosition+=DY*(wallRadius-innerRadius);

  return;
}

  
void
FullBunker::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FullBunker","createSurface");

  const Geometry::Vec3D ZRotAxis((revFlag) ? -Z : Z);

  Geometry::Vec3D CentAxis(Y);
  Geometry::Vec3D AWallDir(X);
  Geometry::Vec3D BWallDir(X);
  // rotation of axis:
  Geometry::Quaternion::calcQRotDeg(leftAngle+leftPhase,-ZRotAxis).rotate(AWallDir);
  Geometry::Quaternion::calcQRotDeg(rightAngle+rightPhase,-ZRotAxis).rotate(BWallDir);
  Geometry::Quaternion::calcQRotDeg
    ((leftAngle+leftPhase+rightAngle+rightPhase)/2.0,-ZRotAxis).
    rotate(CentAxis);
  // rotation of phase points:

  // Points on wall
  Geometry::Vec3D AWall(Origin-rotCentre);
  Geometry::Vec3D BWall(Origin-rotCentre);
  Geometry::Quaternion::calcQRotDeg(-leftPhase,ZRotAxis).rotate(AWall);
  Geometry::Quaternion::calcQRotDeg(-rightPhase,ZRotAxis).rotate(BWall);
  AWall+=rotCentre;
  BWall+=rotCentre;
  
  
  // Divider
  ModelSupport::buildPlane(SMap,buildIndex+1,rotCentre,CentAxis);
  
  ModelSupport::buildCylinder(SMap,buildIndex+7,rotCentre,Z,wallRadius);

  ModelSupport::buildPlane(SMap,buildIndex+3,AWall,AWallDir);
  ModelSupport::buildPlane(SMap,buildIndex+4,BWall,BWallDir);
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*floorDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*roofHeight,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*wallHeight,Z);

  // Walls
  ModelSupport::buildCylinder(SMap,buildIndex+17,rotCentre,
			      Z,wallRadius+wallThick);
    
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   AWall-AWallDir*sideThick,AWallDir);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   BWall+BWallDir*sideThick,BWallDir);
  
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(floorDepth+floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(roofHeight+roofThick),Z);

  setSurf("leftWallInner",SMap.realSurf(buildIndex+3));
  setSurf("rightWallInner",SMap.realSurf(buildIndex+4));
  setSurf("leftWallOuter",SMap.realSurf(buildIndex+13));
  setSurf("rightWallOuter",SMap.realSurf(buildIndex+14));
  setSurf("floorInner",SMap.realSurf(buildIndex+5));
  setSurf("wallInner",SMap.realSurf(buildIndex+106));
  setSurf("roofInner",SMap.realSurf(buildIndex+6));
  setSurf("roofOuter",SMap.realSurf(buildIndex+16));
  // CREATE Sector boundary lines
  // Note negative subtraction as moving +ve to -ve


  //  double phaseAngle(leftPhase);
  //  const double phaseStep((leftPhase+rightPhase)/nSectors);

  // NEW:
  const double phaseDiff(rightPhase-leftPhase);
  const double angleDiff(rightAngle-leftAngle);

  int divIndex(buildIndex+1001);

  const double phaseStep((rightPhase-leftPhase)/static_cast<double>(nSectors));

  SMap.addMatch(divIndex,SMap.realSurf(buildIndex+3));
  for(size_t i=1;i<nSectors;i++)
    {
      divIndex++;
      const double F= (sectPhase[i]-leftPhase)/phaseDiff;
      const double angle= leftAngle+F*angleDiff;
            
      Geometry::Vec3D DPosition(Origin-rotCentre); // Y 
      Geometry::Quaternion::calcQRotDeg(sectPhase[i],-ZRotAxis).rotate(DPosition);
      DPosition+=rotCentre;
      
      Geometry::Vec3D DDir(X);      
      Geometry::Quaternion::calcQRotDeg(sectPhase[i]+angle,-ZRotAxis).rotate(DDir);
      ModelSupport::buildPlane(SMap,divIndex,DPosition,DDir);
    }
  divIndex++;
  SMap.addMatch(divIndex,SMap.realSurf(buildIndex+4));

  return;
}


  
void
FullBunker::createSideLinks(const Geometry::Vec3D& AWall,
			const Geometry::Vec3D& BWall,
			const Geometry::Vec3D& AWallDir,
			const Geometry::Vec3D& BWallDir)
  /*!
    Ugly function to create side wall linkes
    \param AWall :: Left wall point
    \param BWall :: Right wall point		
    \param AWallDir :: Left wall main direction
    \param BWallDir :: Right wall main direction    
   */
{
  ELog::RegMethod RegA("FullBunker","createSideLinks");
		      
  // Construct links on side walls:
  Geometry::Vec3D AWallY(AWallDir*Z);
  Geometry::Vec3D BWallY(BWallDir*Z);

  if (AWallY.dotProd(Y)<0.0)
    AWallY*=-1;
  if (BWallY.dotProd(Y)<0.0)
    BWallY*=-1;

  // Outer 
  FixedComp::setConnect(2,AWall+AWallY*wallRadius/2.0,AWallDir);
  FixedComp::setConnect(3,BWall+BWallY*wallRadius/2.0,BWallDir);

  return;
}
  
void
FullBunker::createObjects(Simulation& System,
		      const attachSystem::FixedComp& FC,  
		      const long int sideIndex)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC :: Side of bulk shield + divider(?)
    \param sideIndex :: side of link point
  */
{
  ELog::RegMethod RegA("FullBunker","createObjects");

  HeadRule HR;

  const HeadRule InnerHR=FC.getFullRule(sideIndex);
  const int InnerSurf=FC.getLinkSurf(sideIndex);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -7 3 -4 5 -6 ");
  makeCell("MainVoid",System,cellIndex++,voidMat,0.0,HR*InnerHR);

  // left:right:floor:roof:Outer
  int lwIndex(buildIndex);  // indexs for wall 
  int rwIndex(buildIndex);
  if (leftWallFlag)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -7 -3 13 5 -106");
      makeCell("leftWall",System,cellIndex++,wallMat,0.0,HR*InnerHR);
      lwIndex+=10;
    }
  if (rightWallFlag)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -7 4 -14 5 -106 ");
      makeCell("rightWall",System,cellIndex++,wallMat,0.0,HR*InnerHR);
      rwIndex+=10;
    }
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,lwIndex,rwIndex,
				 " 1 -17 3M -4N -5 15 ");
  makeCell("floor",System,cellIndex++,wallMat,0.0,HR*InnerHR);

  // Main wall not divided
  int divIndex(buildIndex+1000);

  for(size_t i=0;i<nSectors;i++)
    {
      // Divide the roof into sector as well
      HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -17 6 (106 : -7) -16 ");
      if (i)
	HR*=HeadRule(SMap,divIndex,1);
      else if (leftWallFlag)
	HR*=ModelSupport::getHeadRule(SMap,buildIndex,"13 (106 : 3)");
      else
	HR*=HeadRule(SMap,buildIndex,13);

      if (i+1!=nSectors)
	HR*=HeadRule(SMap,divIndex,-2);
      else if (rightWallFlag)
	HR*=ModelSupport::getHeadRule(SMap,buildIndex,"-14  (106 : -4)");
      else
	HR*=HeadRule(SMap,buildIndex,-14);
      
      makeCell("roof"+std::to_string(i),
	       System,cellIndex++,roofMat,0.0,HR*InnerHR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,divIndex,
				     "1 7 -17 1M -2M 5 -106");
      makeCell("frontWall",System,cellIndex++,wallMat,0.0,HR);
      addCell("frontWall"+std::to_string(i),cellIndex-1);
      divIndex++;
    }

  createMainWall(System);
  createMainRoof(System,InnerSurf);

  // External
  HR=ModelSupport::getHeadRule(SMap,buildIndex,lwIndex,rwIndex,
				 "1 -17 3M -4N 15 -16");
  addOuterSurf(HR*InnerHR);
        
  return;
}

void
FullBunker::createMainRoof(Simulation& System,const int innerSurf)
  /*!
    Create a tesselated main wall
    \param System :: Simulation system
    \param innerSurf :: inner roof cyclindrical surface
  */
{
  ELog::RegMethod RegA("FullBunker","createMainRoof");

  // Note that createSector has to convert these numbers
  // to realSurf so no need to pre-parse via SMap.
  const int outerSurf(buildIndex+17);

  const int lwIndex((leftWallFlag) ? buildIndex+10 : buildIndex);
  const int rwIndex((rightWallFlag) ? buildIndex+10 : buildIndex);
  int divIndex(buildIndex+1000);

  const HeadRule HR(SMap,buildIndex,1);
  roofObj->initialize(System.getDataBase(),*this,6);
  roofObj->setVertSurf(SMap.realSurf(buildIndex+6),
		       SMap.realSurf(buildIndex+16));
  
  roofObj->setRadialSurf(SMap.realSurf(innerSurf),SMap.realSurf(outerSurf));
  roofObj->setDivider(HR);

  for(size_t i=0;i<nSectors;i++)
    {
      const std::string SectNum(std::to_string(i));
      const int LW=(i) ? divIndex+1 : lwIndex+3;
      const int RW=(i+1!=nSectors) ? divIndex+2 : rwIndex+4;
      const int cellN=getCell("roof"+SectNum);

      roofObj->createSector(System,i,cellN,
			    SMap.realSurf(LW),SMap.realSurf(RW));
      
      removeCell("roof"+SectNum);

      //      ELog::EM<<"ROOF Cell = "<<cellN<<" "<<getCell("roof"+SectNum)<<ELog::endDiag;
	    
      const std::vector<int> newCells=roofObj->getCells("Sector"+SectNum);
      addCells("roof",newCells);

      addCell("roofBase",newCells.front());
      addCell("roofBase"+SectNum,newCells.front());
      addCell("roofSector"+SectNum,newCells.front());

      if (i+1==nSectors)
        addCells("roofFarEdge",roofObj->getCells("Sector"+SectNum));

      divIndex++;
    }
  return;
}
  
void
FullBunker::createMainWall(Simulation& System)
  /*!
    Create a tesselated main wall
    \param System :: Simulation system
  */
{
  ELog::RegMethod RegA("FullBunker","createMainWall");


  // Note that createSector has to convert these numbers
  // to realSurf so no need to pre-parse via SMap.
  const int innerSurf(buildIndex+7);
  const int outerSurf(buildIndex+17);

  const int lwIndex((leftWallFlag) ? buildIndex+10 : buildIndex);
  const int rwIndex((rightWallFlag) ? buildIndex+10 : buildIndex);
  int divIndex(buildIndex+1000);

  const HeadRule HR(SMap,buildIndex,1);

  wallObj->createAll(System,*this,0);
  wallObj->setVertSurf(SMap.realSurf(buildIndex+5),
		       SMap.realSurf(buildIndex+106));
  wallObj->setRadialSurf(SMap.realSurf(innerSurf),SMap.realSurf(outerSurf));
  wallObj->setDivider(HR);

  for(size_t i=0;i<nSectors;i++)
    {
      const std::string SectNum(std::to_string(i));
      const int LW=(i) ? divIndex+1 : lwIndex+3;
      const int RW=(i+1!=nSectors) ? divIndex+2 : rwIndex+4;
      const int cellN=getCell("frontWall"+SectNum);

      wallObj->createSector(System,i,cellN,
			    SMap.realSurf(LW),SMap.realSurf(RW));
      
      removeCell("frontWall"+SectNum);
      addCells("frontWall",wallObj->getCells("Sector"+SectNum));
      divIndex++;
    }
  return;  
}

  
void
FullBunker::createLinks(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Create all the links [OutGoing]
    \param FC :: FixedComp use for inner surf
    \param sideIndex :: link index for inner surf
  */
{
  ELog::RegMethod RegA("FullBunker","createLinks");

  FixedComp::setLinkCopy(2,FC,sideIndex);
  
  FixedComp::setConnect(0,rotCentre+Y*(wallRadius),Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+7));
  FixedComp::setBridgeSurf(0,SMap.realSurf(buildIndex+1));
  
  // Outer
  FixedComp::setConnect(1,rotCentre+Y*(wallRadius+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+17));
  FixedComp::setBridgeSurf(1,SMap.realSurf(buildIndex+1));
  
  FixedComp::setConnect(4,Origin-Z*(floorDepth+floorThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  FixedComp::setConnect(5,Origin+Z*(roofHeight+roofThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  // Rotation centre:
  FixedComp::setConnect(6,rotCentre,Y);
  FixedComp::setLinkSurf(6,0);

  // Inner wall
  FixedComp::setConnect(7,rotCentre+Y*wallRadius,-Y);
  FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+7));

  
  FixedComp::setConnect(10,Origin-Z*floorDepth,Z);
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+5));
  FixedComp::setConnect(11,Origin+Z*roofHeight,-Z);
  FixedComp::setLinkSurf(11,-SMap.realSurf(buildIndex+6));

  return;
}
  
std::string
FullBunker::calcSegment(const Simulation& System,
		    const Geometry::Vec3D& TPoint,
		    const Geometry::Vec3D& Axis) const
  /*!
    Determine the segment that a point falls in
    \param System :: Simulation System
    \param TPoint :: Point to check
    \param Axis :: Direction 
    \return Segment string
  */
{
  ELog::RegMethod RegA("FullBunker","calcSegment");
  for(size_t i=0;i<nSectors;i++)
    {
      const std::string SName="frontWall"+std::to_string(i);
      const int cN=getCell(SName);
      const MonteCarlo::Object* SUnit=System.findObject(cN);
      if (SUnit)
	{
	  HeadRule HR=SUnit->getHeadRule();
	  HR.populateSurf();
	  std::pair<Geometry::Vec3D,int> LCut=
	    SurInter::interceptRuleConst(HR,TPoint,Axis);
	  
	  if (LCut.second)
	    return SName;
	}
    }
  throw ColErr::InContainerError<Geometry::Vec3D>
    (TPoint,"Not in bunker wall sectors");
}

void
FullBunker::setCutWall(const bool lFlag,const bool rFlag)
  /*!
    Sets the cut rule base on an existing fixed component.
    \param lFlag :: Wall to use [0 none / 1 full ]
    \param rFlag :: Wall to use [0 none / 1 full ]
  */
{
  ELog::RegMethod RegA("FullBunker","setCutWall");

  leftWallFlag=lFlag;
  rightWallFlag=rFlag;
  return;
}

  

void
FullBunker::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int linkIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param linkIndex :: linkIndex number
  */
{
  ELog::RegMethod RegA("FullBunker","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,linkIndex);
  createSurfaces();
  createLinks(FC,linkIndex);
  createObjects(System,FC,linkIndex);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
