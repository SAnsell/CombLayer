
/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/R3Ring.cxx
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
#include <array>

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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "RingDoor.h"
#include "R3Ring.h"

namespace xraySystem
{

R3Ring::R3Ring(const std::string& Key) : 
  attachSystem::FixedOffset(Key,12),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  NInnerSurf(20)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
}

R3Ring::~R3Ring() 
  /*!
    Destructor
  */
{}

void
R3Ring::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("R3Ring","populate");
  
  FixedOffset::populate(Control);

  fullOuterRadius=Control.EvalVar<double>(keyName+"FullOuterRadius");
  
  icosagonRadius=Control.EvalVar<double>(keyName+"IcosagonRadius");
  icosagonWallThick=Control.EvalVar<double>(keyName+"IcosagonWallThick");

  beamRadius=Control.EvalVar<double>(keyName+"BeamRadius");
  offsetCornerX=Control.EvalVar<double>(keyName+"OffsetCornerX");
  offsetCornerY=Control.EvalVar<double>(keyName+"OffsetCornerY");

  outerWall=Control.EvalVar<double>(keyName+"OuterWall");
  ratchetWall=Control.EvalVar<double>(keyName+"RatchetWall");
  
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");

  doorActive=Control.EvalDefVar<size_t>(keyName+"RingDoorWallID",0);
  return;
}

void
R3Ring::createUnitVector(const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("R3Ring","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
 
void
R3Ring::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("R3Ring","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+9007,Origin,Z,fullOuterRadius);
  
  // quick way to rotate outgoing vector to
  // dividing vector
  double theta(0.0);
  int surfN(buildIndex);

  std::vector<Geometry::Vec3D> innerPts;
  for(size_t i=0;i<NInnerSurf;i++)
    {
      Geometry::Vec3D Axis(sin(theta),cos(theta),0.0);
      const Geometry::Vec3D APt(Origin+Axis*icosagonRadius);
      ModelSupport::buildPlane(SMap,surfN+3,APt,Axis);
      SurfMap::addSurf("CentInner",-SMap.realSurf(surfN+3));
      ModelSupport::buildPlane
	(SMap,surfN+1003,APt+Axis*icosagonWallThick,Axis);
      SurfMap::addSurf("RingInner",SMap.realSurf(surfN+1003));
      // mid point
      innerPts.push_back(APt+Axis*icosagonWallThick);
      theta+=2.0*M_PI/static_cast<double>(NInnerSurf);
      surfN+=10;
    }
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);
  SurfMap::setSurf("Floor",SMap.realSurf(buildIndex+5));

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+roofThick),Z);

  // Inner coordinate points are all offset from the inner points

  // length of flat extent either side of mid point
  double LLength=(icosagonRadius+icosagonWallThick)*
    sin(M_PI/static_cast<double>(NInnerSurf));
  
  std::vector<Geometry::Vec3D> outerPts;
  std::vector<Geometry::Vec3D> outerX;
  std::vector<Geometry::Vec3D> outerY;
  for(size_t i=0;i<NInnerSurf;i++)
    {
      Geometry::Vec3D APt=innerPts[i];
      const Geometry::Vec3D YY=(APt-Origin).unit();
      const Geometry::Vec3D XX=YY*Z;
      APt-= XX*LLength;  // start point of flat
      outerPts.push_back(APt+XX*offsetCornerX+YY*offsetCornerY);
      outerX.push_back(XX);
      outerY.push_back(YY);
      LLength+=0.1;
    }
  // outer walls [inner] == 2000
  surfN=buildIndex+2000;
  for(size_t i=0;i<NInnerSurf;i++)
    {
      const size_t li((!i) ? NInnerSurf-1 : i-1);
      const Geometry::Vec3D& APt(outerPts[i]);
      const Geometry::Vec3D& XX(outerX[li]);
      const Geometry::Vec3D& YY(outerY[i]);

      ModelSupport::buildPlane(SMap,surfN+1,APt,XX);
      SurfMap::addSurf("BeamInner",SMap.realSurf(surfN+1));
      ModelSupport::buildPlane(SMap,surfN+3,APt,YY);
      SurfMap::addSurf("FlatInner",-SMap.realSurf(surfN+3));
      
      // outer wall
      ModelSupport::buildPlane(SMap,surfN+1001,APt+XX*ratchetWall,XX);
      SurfMap::addSurf("BeamOuter",SMap.realSurf(surfN+1001));
      ModelSupport::buildPlane(SMap,surfN+1003,APt+YY*outerWall,YY);
      SurfMap::addSurf("FlatOuter",SMap.realSurf(surfN+1003));

      surfN+=10;
    }
  
  return;
}

void
R3Ring::createFloor(Simulation& System)
  /*!
    Adds the main roof objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("R3Ring","createFloor");
  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-9007 -5 15 ");
  makeCell("Floor",System,cellIndex++,floorMat,0.0,Out);
  return;
}

void
R3Ring::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("R3Ring","createObjects");

  const std::string fullLayer=
    ModelSupport::getComposite(SMap,buildIndex," 5 -16 ");
  const std::string roofLayer=
    ModelSupport::getComposite(SMap,buildIndex," 6 -16 ");
  const std::string innerLayer=
    ModelSupport::getComposite(SMap,buildIndex," 5 -6 ");

  // horrible code to build a list of 20 values
  std::ostringstream cx;
  cx<<" ";
  for(long int i=0;i<static_cast<long int>(NInnerSurf);i++)
    cx<<-(10*i+3)<<" ";
  const std::string lineBuild(cx.str());

  
  std::string Out;

  //  createRoof(System);
  createFloor(System);

  // INNER VOID:

  Out=ModelSupport::getComposite(SMap,buildIndex,lineBuild);
  
  HeadRule IV(Out);
  makeCell("InnerVoid",System,cellIndex++,0,0.0,Out+fullLayer);

  IV.makeComplement();
  Out=ModelSupport::getComposite(SMap,buildIndex+1000,lineBuild);
  HeadRule XV(Out);
  XV.makeComplement();
  makeCell("InnerWall",System,cellIndex++,wallMat,0.0,
	   Out+fullLayer+IV.display());

  // Now build each individial segment

  int BPrev(buildIndex+2190);
  int BNext(buildIndex+2000);
  int IPrev(buildIndex+1190);
  int INext(buildIndex+1000);
  for(size_t i=0;i<NInnerSurf;i++)
    {

      // outer
      Out=ModelSupport::getComposite
	(SMap,BNext,BPrev," 1M -3M  -1 ");
      // inner
      Out+=ModelSupport::getComposite(SMap,IPrev,INext," (3:3M) ");
      //      Out+=XV.display();
      makeCell("InnerVoid",System,cellIndex++,0,0.0,Out+innerLayer);
      makeCell("Roof",System,cellIndex++,roofMat,0.0,Out+roofLayer);

      Out=ModelSupport::getComposite
	(SMap,BNext,BPrev," 1001M -1003M  -1 3M");
      makeCell("OuterFlat",System,cellIndex++,wallMat,0.0,Out+fullLayer);

      Out=ModelSupport::getComposite
	(SMap,BNext,BPrev," 1 -1001 -1003M  3 ");
      makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out+fullLayer);

      Out=ModelSupport::getComposite
	(SMap,BNext,BPrev,buildIndex," 1001 -1003M  1003 -9007N ");
      makeCell("OuterSegment",System,cellIndex++,0,0.0,Out+fullLayer);
      
      IPrev=INext;
      BPrev=BNext;
      INext+=10;
      BNext+=10;
    }
  

  Out=ModelSupport::getComposite(SMap,buildIndex,"-9007 15 -16");  
  addOuterSurf(Out);    

  return;
}

void
R3Ring::createLinks()
  /*!
    Determines the link points for the beam direction first:
    This is special : each beamport has two coordinates (and axis)
    First is the mid triangle point [ start of straight section]
    The second is the mid point on the wall it points to
  */
{
  ELog::RegMethod RegA("R3Ring","createLinks");

  FixedComp::setNConnect(2*(NInnerSurf+1));

  double theta(-2.0*M_PI/static_cast<double>(NInnerSurf));
  for(size_t i=0;i<NInnerSurf;i++)
    {
      const Geometry::Vec3D Axis(sin(theta),cos(theta),0.0);
      const Geometry::Vec3D PtX(Origin+Axis*beamRadius);
      const Geometry::Plane* BInner=dynamic_cast<const Geometry::Plane*>
	(SurfMap::getSurfPtr("BeamInner",(i+NInnerSurf-1) % NInnerSurf));
      const Geometry::Plane* BWall=dynamic_cast<const Geometry::Plane*>
	(SurfMap::getSurfPtr("BeamInner",i));
      const Geometry::Plane* BExit=dynamic_cast<const Geometry::Plane*>
	(SurfMap::getSurfPtr("BeamOuter",i));
      
      const Geometry::Vec3D Beam= BWall->getNormal();

      const Geometry::Vec3D beamOrigin=
	SurInter::getLinePoint(PtX,Beam,BInner);
      const Geometry::Vec3D exitCentre=
	SurInter::getLinePoint(PtX,Beam,BExit);

      FixedComp::nameSideIndex(i,"OpticCentre"+std::to_string(i));
      FixedComp::setLinkSurf(i,-BInner->getName());
      FixedComp::setConnect(i,beamOrigin,Beam);

      FixedComp::nameSideIndex(i+NInnerSurf,"ExitCentre"+std::to_string(i));
      FixedComp::setLinkSurf(NInnerSurf+i,-BExit->getName());
      FixedComp::setConnect(NInnerSurf+i,exitCentre,Beam);
					
      theta+=2.0*M_PI/static_cast<double>(NInnerSurf);
    }
  return;
}

void
R3Ring::createDoor(Simulation& System)
  /*!
    Build if a ring-door is required [ access to area outside of ring]
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("R3Ring","createDoor");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
    

  if (doorActive)
    {
      doorPtr=std::make_shared<xraySystem::RingDoor>(keyName+"RingDoor");
      OR.addObject(doorPtr);

      doorPtr->setCutSurf
	("innerWall",-SurfMap::getSurf("FlatInner",doorActive-1));
      doorPtr->setCutSurf
	("outerWall",-SurfMap::getSurf("FlatOuter",doorActive-1));
      doorPtr->setCutSurf("floor",SurfMap::getSurf("Floor"));

      doorPtr->addAllInsertCell(getCell("OuterFlat",doorActive % NInnerSurf));
      doorPtr->createAll(System,*this,doorActive+1);
    }
  return;
}


void
R3Ring::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("R3Ring","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);

  createDoor(System);
  return;
}
  
}  // NAMESPACE xraySystem
