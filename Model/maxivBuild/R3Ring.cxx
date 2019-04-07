/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/R1Ring.cxx
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
#include "SpaceCut.h"
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
  NInnerSurf(20),
  NPoints(0),concaveNPoints(0),
  doorActive(0)
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

  outerRadius=Control.EvalVar<double>(keyName+"IcosagonWallThick");
  offsetCornerX=Control.EvalVar<double>(keyName+"OffsetCornerX");
  offsetCornerY=Control.EvalVar<double>(keyName+"OffsetCornerY");
  
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  roofExtra=Control.EvalVar<double>(keyName+"RoofExtraVoid");

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
      ModelSupport::buildPlane
	(SMap,surfN+1003,APt+Axis*icosagonWallThick,Axis);
      // mid point
      innerPts.push_back(APt+Axis*icosagonWallThick);
      theta+=2.0*M_PI/static_cast<double>(NInnerSurf);
      surfN+=10;
    }
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+floorThick),Z);
  SurfMap::setSurf("Floor",SMap.realSurf(buildIndex+5));
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+roofThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,
			   Origin+Z*(height+roofThick+roofExtra),Z);

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
      const Geometry::Vec3D XX=Z*YY;
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
      //      const size_t li((!i) ? NInnerSurf-1 : i-1);
      const size_t li(i);
      const Geometry::Vec3D& APt(outerPts[li]);
      const Geometry::Vec3D& XX(outerX[li]);
      const Geometry::Vec3D& YY(outerY[li]);

      ModelSupport::buildPlane(SMap,surfN+1,APt,XX);
      ModelSupport::buildPlane(SMap,surfN+3,APt,YY);
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
R3Ring::createRoof(Simulation& System)
  /*!
    Adds the main roof objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("R3Ring","createRoof");

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

  const std::string TBase=
    ModelSupport::getComposite(SMap,buildIndex," 6 -16 ");
  const std::string EBase=
    ModelSupport::getComposite(SMap,buildIndex," 16 -26 ");

  Out=ModelSupport::getComposite
    (SMap,buildIndex,Unit+"(103:113:123:133:143:153)");
  makeCell("InnerRoof",System,cellIndex++,roofMat,0.0,Out+TBase);
  makeCell("InnerExtra",System,cellIndex++,0,0.0,Out+EBase);  

     // loop to make individual units:
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
	      makeCell("RoofExtra",System,cellIndex++,0,0.0,Out+EBase);
	      convexN+=10;
	    }

	  index++;
	  Out="";
	}
      surfN= (i==NPoints) ? 1000 : surfN+10;
    }
  Out+=ModelSupport::getComposite(SMap,buildIndex+convexN," -9 ");
  makeCell("RoofTriangle",System,cellIndex++,roofMat,0.0,Out+TBase);
  makeCell("RoofExtra",System,cellIndex++,0,0.0,Out+EBase);
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

  const std::string wallBase=
    ModelSupport::getComposite(SMap,buildIndex," 5 -16 ");
  const std::string extraBase=
    ModelSupport::getComposite(SMap,buildIndex," 16 -26 ");
  const std::string fullBase=
    ModelSupport::getComposite(SMap,buildIndex," 5 -26 ");
  const std::string innerBase=
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
  makeCell("InnerVoid",System,cellIndex++,0,0.0,Out+fullBase);

  IV.makeComplement();
  Out=ModelSupport::getComposite(SMap,buildIndex+1000,lineBuild);
  HeadRule XV(Out);
  XV.makeComplement();
  makeCell("InnerWall",System,cellIndex++,0,0.0,Out+fullBase+IV.display());

  // Now build each individial segment

  int BPrev(buildIndex+2190);
  int BNext(buildIndex+2000);
  int IPrev(buildIndex+1190);
  int INext(buildIndex+1000);
  for(size_t i=0;i<10 && i<NInnerSurf;i++)
    {

      // outer
      Out=ModelSupport::getComposite
	(SMap,BNext,BPrev," -1M -3M  1 ");
      // inner
      //      Out+=ModelSupport::getComposite(SMap,IPrev,INext," 3 3M ");
      Out+=XV.display();
      makeCell("InnerVoid",System,cellIndex++,0,0.0,Out+fullBase);      
      IPrev=INext;
      BPrev=BNext;
      INext+=10;
      BNext+=10;
    }

  Out=ModelSupport::getComposite(SMap,buildIndex+2000,lineBuild);
  addOuterSurf(Out);    
  return;




  


  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -103 -113 -123 -133 -143 -153 "
				 " (3 : 13 : 23 : 33 : 43 : 53) ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out+wallBase);
  makeCell("WallExtra",System,cellIndex++,wallMat,0.0,Out+extraBase);



  // Create inner voids
  std::string Unit;
  int surfN=5000;
  for(size_t i=0;i<concaveNPoints;i++)
    {
      Unit+=std::to_string(surfN+9)+" ";
      surfN+=10;
    }
  Out=ModelSupport::getComposite(SMap,buildIndex,Unit);
  Out+=ModelSupport::getComposite
    (SMap,buildIndex,"(103:113:123:133:143:153)");
  makeCell("Void",System,cellIndex++,0,0.0,Out+innerBase);  

   // loop to make individual units:
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
	      makeCell("VoidTriangle",System,cellIndex++,0,0.0,Out+innerBase);
	      convexN+=10;
	    }

	  index++;
	  Out="";
	}
      surfN= (i==NPoints) ? 1000 : surfN+10;
    }
  Out+=ModelSupport::getComposite(SMap,buildIndex+convexN," -9 ");
  makeCell("VoidTriangle",System,cellIndex++,0,0.0,Out+innerBase);

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
	    (SMap,buildIndex,prevN,fwdN," 2003M -1003N -1013N 2003N ");
	  makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out+wallBase);
	  makeCell("FrontExtra",System,cellIndex++,0,0.0,Out+extraBase);

	  Out=ModelSupport::getComposite
	    (SMap,buildIndex,fwdN," -2003M (-1013M:-1023M) 1033M 2013M 2023M ");

	  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out+wallBase);
	  makeCell("Extra",System,cellIndex++,0,0.0,Out+extraBase);
	  surfN+=30;
	  divN+=10;
	}
      else
	{
	  Out=ModelSupport::getComposite  
	    (SMap,buildIndex,surfN,divN," -1013M 2003M 2013M -1N ");

	  makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out+wallBase);
	  makeCell("FrontExtra",System,cellIndex++,0,0.0,Out+extraBase);
	  if (index!=9)
	    Out=ModelSupport::getComposite   
	      (SMap,buildIndex,surfN,divN,"(1N:-2013M) -1023M 1033M 2023M ");
	  else
	    Out=ModelSupport::getComposite   
	      (SMap,buildIndex,surfN,divN," 1N -1023M 1003 2023M  ");

	  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out+wallBase);
	  makeCell("Extra",System,cellIndex++,0,0.0,Out+extraBase);
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
	    (SMap,buildIndex,surfN,prevN," -9007 -3M 13N ");
	  surfN+=10;
	}
      else
	{
	  // short segment :  
	  Out=ModelSupport::getComposite  
	    (SMap,buildIndex,surfN,prevN," -9007 -3M -13M 13N ");
	  surfN+=20;
	}
      prevN=surfN-20;
      makeCell("OuterSegment",System,cellIndex++,0,0.0,Out+fullBase);
    }
	  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-9007 15 -26");
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

  FixedComp::setNConnect(2*concaveNPoints+2);
  return;
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
R3Ring::createDoor(Simulation& System)
  /*!
    Build if a ring-door is required
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("R3Ring","createMaze");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();


  if (doorActive)
    {
      doorPtr=std::make_shared<xraySystem::RingDoor>(keyName+"RingDoor");
      OR.addObject(doorPtr);

      doorPtr->setCutSurf
	("innerWall",SurfMap::getSurf("SideInner",doorActive-1));
      doorPtr->setCutSurf
	("outerWall",-SurfMap::getSurf("SideOuter",doorActive-1));

      doorPtr->addInsertCell(getCell("Wall",doorActive % 10));
      doorPtr->createAll(System,*this,doorActive+2);
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
  ELog::EM<<"ASDFAF"<<ELog::endDiag;
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);

  createDoor(System);
  return;
}
  
}  // NAMESPACE xraySystem
