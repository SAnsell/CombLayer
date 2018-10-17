/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/R1Ring.cxx
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
#include "stringCombine.h"
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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
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
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "RingDoor.h"
#include "R1Ring.h"

namespace xraySystem
{

R1Ring::R1Ring(const std::string& Key) : 
  attachSystem::FixedOffset(Key,12),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  NPoints(0),concaveNPoints(0),
  doorActive(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
}

R1Ring::~R1Ring() 
  /*!
    Destructor
  */
{}

void
R1Ring::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("R1Ring","populate");
  
  FixedOffset::populate(Control);

  fullOuterRadius=Control.EvalVar<double>(keyName+"FullOuterRadius");
  
  hexRadius=Control.EvalVar<double>(keyName+"HexRadius");
  hexWallThick=Control.EvalVar<double>(keyName+"HexWallThick");


  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");

  NPoints=Control.EvalVar<size_t>(keyName+"NPoints");
  for(size_t i=0;i<NPoints;i++)
    {
      const std::string NStr=std::to_string(i);
      const Geometry::Vec3D PtA=
	Control.EvalVar<Geometry::Vec3D>(keyName+"VPoint"+NStr);
      const Geometry::Vec3D PtB=
	Control.EvalVar<Geometry::Vec3D>(keyName+"OPoint"+NStr);
      voidTrack.push_back(PtA);
      outerTrack.push_back(PtB);
    }
  for(size_t i=1;i<NPoints+1;i++)
    {
      const Geometry::Vec3D& C(voidTrack[i % NPoints]);
      const Geometry::Vec3D A((voidTrack[i-1]-C).unit());
      const Geometry::Vec3D B((voidTrack[(i+1) % NPoints]-C).unit());
      Geometry::Vec3D XA=A*B;
      if (XA.dotProd(Z)<0.0)
	concavePts.push_back(i);
    }
  concaveNPoints=concavePts.size();


  doorActive=Control.EvalDefVar<size_t>(keyName+"RingDoorWallID",0);
  return;
}

void
R1Ring::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("R1Ring","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
 
void
R1Ring::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("R1Ring","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+9007,Origin,Z,fullOuterRadius);
  
  // quick way to rotate outgoing vector to
  // dividing vector
  const Geometry::Quaternion Qz=Geometry::Quaternion::calcQRotDeg(-120.0,Z);
  double theta(0.0);
  int surfN(buildIndex);
  
  for(size_t i=0;i<6;i++)
    {
      Geometry::Vec3D Axis(sin(theta),cos(theta),0.0);
      ModelSupport::buildPlane(SMap,surfN+3,Origin+Axis*hexRadius,Axis);
      ModelSupport::buildPlane(SMap,surfN+103,
			       Origin+Axis*(hexRadius+hexWallThick),Axis);
      // divider vector
      ModelSupport::buildPlane(SMap,surfN+7,Origin,Qz.rotate(Axis));
      theta+=M_PI/3.0;
      surfN+=10;
    }
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+floorThick),Z);
  SurfMap::setSurf("Floor",SMap.realSurf(buildIndex+5));
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+roofThick),Z);

  surfN=buildIndex+1000;
  for(size_t i=0;i<NPoints;i++)
    {
      const Geometry::Vec3D AP(X*voidTrack[i].X()+Y*voidTrack[i].Y());
      const Geometry::Vec3D BP
	(X*voidTrack[(i+1)% NPoints].X()+Y*voidTrack[(i+1) % NPoints].Y());
      // note that voidTrack is about (0,0,0)
      const Geometry::Vec3D NDir=(-BP);  
      
      ModelSupport::buildPlane(SMap,surfN+3,
			       Origin+AP,
			       Origin+BP,
			       Origin+BP+Z,NDir);

      surfN+=10;
    }

  size_t cIndex(0);
  surfN=buildIndex+2000;
  for(size_t i=0;i<NPoints;i++)
    {
      const Geometry::Vec3D AP(X*outerTrack[i].X()+Y*outerTrack[i].Y());
      const Geometry::Vec3D BP
	(X*outerTrack[(i+1)% NPoints].X()+Y*outerTrack[(i+1) % NPoints].Y());
      const Geometry::Vec3D NDir=(Origin-BP);
      
      ModelSupport::buildPlane(SMap,surfN+3,
			       Origin+AP,Origin+BP,
			       Origin+BP+Z,NDir);
      // trick to get exit walls [inner /outer]
      if (concavePts[cIndex]==i+1)
	{
	  SurfMap::addSurf("BeamInner",SMap.realSurf(surfN-1000+3));
	  SurfMap::addSurf("BeamOuter",SMap.realSurf(surfN+3));

	  if (cIndex)
	    {
	      SurfMap::addSurf("SideInner",SMap.realSurf(surfN-1010+3));
	      SurfMap::addSurf("SideOuter",SMap.realSurf(surfN-10+3));
	    }
	  cIndex = (cIndex+1) % concaveNPoints;
	}
      surfN+=10;
    }
  // last wall
  SurfMap::addSurf("SideInner",SMap.realSurf(surfN-1010+3));
  SurfMap::addSurf("SideOuter",SMap.realSurf(surfN-10+3));
	  

  // Exit wall dividers
  surfN=buildIndex+3000;
  for(size_t i=0;i<concaveNPoints;i++)
    {
      const size_t aI=concavePts[i];
      const Geometry::Vec3D AP(X*voidTrack[aI].X()+Y*voidTrack[aI].Y());
      const Geometry::Vec3D BP(X*outerTrack[aI].X()+Y*outerTrack[aI].Y());
      // going round ring clockwize 
      const Geometry::Vec3D NDir=((BP-AP)*Z).unit();      
      ModelSupport::buildPlane(SMap,surfN+1,
			       Origin+AP,Origin+BP,
			       Origin+BP+Z,NDir);
      surfN+=10;
    } 

  // Inner cut points
  surfN=buildIndex+5000;
  for(size_t i=1;i<concaveNPoints+1;i++)
    {
      const size_t aI=concavePts[i-1];
      const size_t bI=concavePts[i % concaveNPoints];
      const Geometry::Vec3D AP(X*voidTrack[aI].X()+Y*voidTrack[aI].Y());
      const Geometry::Vec3D BP(X*voidTrack[bI].X()+Y*voidTrack[bI].Y());
      const Geometry::Vec3D NDir=(-BP);
      
      ModelSupport::buildPlane(SMap,surfN+9,
			       Origin+AP,Origin+BP,
			       Origin+BP+Z,NDir);
      surfN+=10;
    }

  
  return;
}

void
R1Ring::createFloor(Simulation& System)
  /*!
    Adds the main roof objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("R1Ring","createFloor");
  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-9007 -5 15 ");
  makeCell("Floor",System,cellIndex++,floorMat,0.0,Out);
  return;
}


void
R1Ring::createRoof(Simulation& System)
  /*!
    Adds the main roof objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("R1Ring","createRoof");

  std::string Out;
  
  int surfN(buildIndex);


  // Create inner roof
  std::string Unit;
  surfN=5000;
  for(size_t i=0;i<concaveNPoints;i++)
    {
      Unit+=std::to_string(surfN+9)+" ";
      surfN+=10;
    }
  Out=ModelSupport::getComposite(SMap,buildIndex,Unit+" 6 -16");
  Out+=ModelSupport::getComposite
    (SMap,buildIndex,"(103:113:123:133:143:153)");
  makeCell("InnerRoof",System,cellIndex++,roofMat,0.0,Out);  

     // loop to make individual units:
  const std::string TBase=
    ModelSupport::getComposite(SMap,buildIndex," 6 -16 ");
  size_t index=0;
  surfN=1000;
  int convexN=5000;
  Out="";
  std::string WOut;
  for(size_t i=1;i<NPoints+2;i++)
    {
      Out+=ModelSupport::getComposite(SMap,buildIndex+surfN," 3 ");
      if (index<concavePts.size() && i==concavePts[index])
	{
	  Out+=ModelSupport::getComposite(SMap,buildIndex+convexN," -9 ");
	  if (index)
	    {
	      makeCell("RoofTriangle",System,cellIndex++,roofMat,0.0,Out+TBase);
	      convexN+=10;
	    }

	  index++;
	  Out="";
	}
      surfN= (i==NPoints) ? 1000 : surfN+10;
    }
  Out+=ModelSupport::getComposite(SMap,buildIndex+convexN," -9 ");
  makeCell("RoofTriangle",System,cellIndex++,roofMat,0.0,Out+TBase);

  return;
}

void
R1Ring::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("R1Ring","createObjects");

  std::string Out;

  createRoof(System);
  createFloor(System);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -3 -13 -23 -33 -43 -53 5 -16" );
  makeCell("InnerVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -103 -113 -123 -133 -143 -153 5 -16 "
				 " (3 : 13 : 23 : 33 : 43 : 53) ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);
  

  // Create inner voids
  std::string Unit;
  int surfN=5000;
  for(size_t i=0;i<concaveNPoints;i++)
    {
      Unit+=std::to_string(surfN+9)+" ";
      surfN+=10;
    }
  Out=ModelSupport::getComposite(SMap,buildIndex,Unit+"5 -6");
  Out+=ModelSupport::getComposite
    (SMap,buildIndex,"(103:113:123:133:143:153)");
  makeCell("Void",System,cellIndex++,0,0.0,Out);  

   // loop to make individual units:
  const std::string TBase=
    ModelSupport::getComposite(SMap,buildIndex," 5 -6 ");
  size_t index=0;
  surfN=1000;
  int convexN=5000;
  Out="";
  std::string WOut;
  for(size_t i=1;i<NPoints+2;i++)
    {
      Out+=ModelSupport::getComposite(SMap,buildIndex+surfN," 3 ");
      if (index<concavePts.size() && i==concavePts[index])
	{
	  Out+=ModelSupport::getComposite(SMap,buildIndex+convexN," -9 ");
	  if (index)
	    {
	      makeCell("VoidTriangle",System,cellIndex++,0,0.0,Out+TBase);
	      convexN+=10;
	    }

	  index++;
	  Out="";
	}
      surfN= (i==NPoints) ? 1000 : surfN+10;
    }
  Out+=ModelSupport::getComposite(SMap,buildIndex+convexN," -9 ");
  makeCell("VoidTriangle",System,cellIndex++,0,0.0,Out+TBase);

  // WALLS:

  int divN=buildIndex+3000;
  surfN=buildIndex-10;
  for(size_t index=0;index<10;index++)
    {
      if (index==0 || index==3)
	{
	  const int prevN=(!index) ? buildIndex+210 : buildIndex+60;
	  const int fwdN=(!index) ? buildIndex : buildIndex+70;
	  // LONG SEGMENT 1:
	  Out=ModelSupport::getComposite    // s=1000 
	    (SMap,buildIndex,prevN,fwdN," 2003M -1003N -1013N 2003N  5 -16");
	  makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite
	    (SMap,buildIndex,fwdN," -2003M (-1013M:-1023M) 1033M 2013M 2023M 5 -16");
	  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);
	  surfN+=30;
	  divN+=10;
	}
      else
	{
	  Out=ModelSupport::getComposite  
	    (SMap,buildIndex,surfN,divN," -1013M 2003M 2013M -1N 5 -16");
	  makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out);
	  if (index!=9)
	    Out=ModelSupport::getComposite   
		(SMap,buildIndex,surfN,divN,"1N -1023M 1033M 2023M 5 -16");
	  else
	    Out=ModelSupport::getComposite   
	      (SMap,buildIndex,surfN,divN,"1N -1023M 1003 2023M 5 -16");
	  
	  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);
	  surfN+=20;
	  divN+=10;
	}
    }

  // NOW DO external void-triangles:
  int prevN=buildIndex+2200;
  surfN=buildIndex+2000;
  
  for(size_t i=0;i<12;i++)
    {      
      if (i==1 || i==5)
	{
	  // long segment :  
	  Out=ModelSupport::getComposite  
	    (SMap,buildIndex,surfN,prevN," -9007 -3M 13N 5 -16");
	  surfN+=10;
	}
      else
	{
	  // short segment :  
	  Out=ModelSupport::getComposite  
	    (SMap,buildIndex,surfN,prevN," -9007 -3M -13M 13N 5 -16");
	  surfN+=20;
	}
      prevN=surfN-20;
      makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);	  
    }
	  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-9007 15 -16");
  addOuterSurf(Out);    
  return;
}

void
R1Ring::createLinks()
  /*!
    Determines the link points for the beam direction first:
    This is special : each beamport has two coordinates (and axis)
    First is the mid triangle point [ start of straight section]
    The second is the mid point on the wall it points to
  */
{
  ELog::RegMethod RegA("R1Ring","createLinks");

  FixedComp::setNConnect(2*concaveNPoints+2);
  
  const double beamStepOut(1510.55);  // from origin
  // Main beam start points DONT have a surface [yet]
  
  for(size_t i=1;i<concaveNPoints+1;i++)
    {
      const size_t index(i-1);

      const Geometry::Plane* BInner=dynamic_cast<const Geometry::Plane*>
	(SurfMap::getSurfPtr("BeamInner",((i+1) % concaveNPoints)));
      if (!BInner)
	throw ColErr::InContainerError<std::string>
	  ("BeamInner"+std::to_string(index),"Surf map no found");

      const Geometry::Vec3D Beam= -BInner->getNormal();
      const Geometry::Vec3D Axis= -Beam*Z;
      const Geometry::Vec3D PtX=Origin+Axis*beamStepOut;
      
      FixedComp::nameSideIndex(index+2,"OpticCentre"+std::to_string(index));
      FixedComp::setLinkSurf(index+2,-BInner->getName());
      FixedComp::setConnect(index+2,PtX,Beam);
      
    }
  return;
}

void
R1Ring::createDoor(Simulation& System)
  /*!
    Build if a ring-door is required
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("R1Ring","createMaze");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();


  if (doorActive)
    {
      doorPtr=std::make_shared<xraySystem::RingDoor>(keyName+"RingDoor");
      OR.addObject(doorPtr);

      ELog::EM<<"Inner wall == "<<SurfMap::getSurf("SideInner",doorActive-1)
	      <<ELog::endDiag;
      ELog::EM<<"Outer wall == "<<SurfMap::getSurf("SideOuter",doorActive-1)
	      <<ELog::endDiag;
      doorPtr->setCutSurf
	("innerWall",SurfMap::getSurf("SideInner",doorActive-1));
      doorPtr->setCutSurf
	("outerWall",-SurfMap::getSurf("SideOuter",doorActive-1));

      ELog::EM<<"Cell == "<<getCell("Wall",(doorActive % 10))<<ELog::endDiag;
      doorPtr->addInsertCell(getCell("Wall",doorActive % 10));
      doorPtr->createAll(System,*this,3);
    }
  return;
}


void
R1Ring::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("R1Ring","createAll(FC)");

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
