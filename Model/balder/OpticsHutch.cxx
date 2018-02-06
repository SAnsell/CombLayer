/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/OpticsHutch.cxx
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
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "OpticsHutch.h"

namespace xraySystem
{

OpticsHutch::OpticsHutch(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  hutIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hutIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

OpticsHutch::OpticsHutch(const OpticsHutch& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  hutIndex(A.hutIndex),cellIndex(A.cellIndex),depth(A.depth),
  height(A.height),length(A.length),ringWidth(A.ringWidth),
  ringWallLen(A.ringWallLen),ringWallAngle(A.ringWallAngle),
  outWidth(A.outWidth),innerThick(A.innerThick),
  pbWallThick(A.pbWallThick),pbBackThick(A.pbBackThick),
  pbRoofThick(A.pbRoofThick),outerThick(A.outerThick),
  floorThick(A.floorThick),holeXStep(A.holeXStep),
  holeZStep(A.holeZStep),holeRadius(A.holeRadius),
  skinMat(A.skinMat),pbMat(A.pbMat),floorMat(A.floorMat)
  /*!
    Copy constructor
    \param A :: OpticsHutch to copy
  */
{}

OpticsHutch&
OpticsHutch::operator=(const OpticsHutch& A)
  /*!
    Assignment operator
    \param A :: OpticsHutch to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      depth=A.depth;
      height=A.height;
      length=A.length;
      ringWidth=A.ringWidth;
      ringWallLen=A.ringWallLen;
      ringWallAngle=A.ringWallAngle;
      outWidth=A.outWidth;
      innerThick=A.innerThick;
      pbWallThick=A.pbWallThick;
      pbBackThick=A.pbBackThick;
      pbRoofThick=A.pbRoofThick;
      outerThick=A.outerThick;
      floorThick=A.floorThick;
      holeXStep=A.holeXStep;
      holeZStep=A.holeZStep;
      holeRadius=A.holeRadius;
      skinMat=A.skinMat;
      pbMat=A.pbMat;
      floorMat=A.floorMat;
    }
  return *this;
}

OpticsHutch::~OpticsHutch() 
  /*!
    Destructor
  */
{}

void
OpticsHutch::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("OpticsHutch","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  outWidth=Control.EvalVar<double>(keyName+"OutWidth");
  ringWidth=Control.EvalVar<double>(keyName+"RingWidth");
  ringWallLen=Control.EvalVar<double>(keyName+"RingWallLen");
  ringWallAngle=Control.EvalVar<double>(keyName+"RingWallAngle");

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  pbWallThick=Control.EvalVar<double>(keyName+"PbWallThick");
  pbBackThick=Control.EvalVar<double>(keyName+"PbBackThick");
  pbRoofThick=Control.EvalVar<double>(keyName+"PbRoofThick");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");

  floorThick=Control.EvalVar<double>(keyName+"FloorThick");

  holeXStep=Control.EvalDefVar<double>(keyName+"HoleXStep",0.0);
  holeZStep=Control.EvalDefVar<double>(keyName+"HoleZStep",0.0);
  holeRadius=Control.EvalDefVar<double>(keyName+"HoleRadius",0.0);

  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");
  pbMat=ModelSupport::EvalMat<int>(Control,keyName+"PbMat");
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");

  return;
}

void
OpticsHutch::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("OpticsHutch","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
 
void
OpticsHutch::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("OpticsHutch","createSurfaces");

  // Inner void
  ModelSupport::buildPlane(SMap,hutIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,hutIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,hutIndex+3,Origin-X*outWidth,X);
  ModelSupport::buildPlane(SMap,hutIndex+4,Origin+X*ringWidth,X);
  ModelSupport::buildPlane(SMap,hutIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,hutIndex+6,Origin+Z*height,Z);  


  ModelSupport::buildPlane(SMap,hutIndex+15,Origin-Z*(depth+floorThick),Z);

  // Steel inner layer
  ModelSupport::buildPlane(SMap,hutIndex+12,
			   Origin+Y*(length+innerThick),Y);
  ModelSupport::buildPlane(SMap,hutIndex+13,
			   Origin-X*(outWidth+innerThick),X);
  ModelSupport::buildPlane(SMap,hutIndex+14,
			   Origin+X*(ringWidth+innerThick),X);
  ModelSupport::buildPlane(SMap,hutIndex+16,
			       Origin+Z*(height+innerThick),Z);  

  // Lead
  ModelSupport::buildPlane(SMap,hutIndex+22,
			   Origin+Y*(length+innerThick+pbBackThick),Y);
  ModelSupport::buildPlane(SMap,hutIndex+23,
			   Origin-X*(outWidth+innerThick+pbWallThick),X);
  ModelSupport::buildPlane(SMap,hutIndex+24,
			   Origin+X*(ringWidth+innerThick+pbWallThick),X);
  ModelSupport::buildPlane(SMap,hutIndex+26,
			       Origin+Z*(height+innerThick+pbRoofThick),Z);

  const double steelThick(innerThick+outerThick);
  // OuterWall
  ModelSupport::buildPlane(SMap,hutIndex+32,
			   Origin+Y*(length+steelThick+pbBackThick),Y);
  ModelSupport::buildPlane(SMap,hutIndex+33,
			   Origin-X*(outWidth+steelThick+pbWallThick),X);
  ModelSupport::buildPlane(SMap,hutIndex+34,
			   Origin+X*(ringWidth+steelThick+pbWallThick),X);
  ModelSupport::buildPlane(SMap,hutIndex+36,
			       Origin+Z*(height+steelThick+pbRoofThick),Z);  

  
  if (std::abs(ringWallAngle)>Geometry::zeroTol)
    {
      Geometry::Vec3D RPoint(Origin+X*ringWidth+Y*ringWallLen);
      ModelSupport::buildPlaneRotAxis
	(SMap,hutIndex+104,RPoint,X,-Z,ringWallAngle);
      RPoint += X*innerThick;
      ModelSupport::buildPlaneRotAxis
	(SMap,hutIndex+114,RPoint,X,-Z,ringWallAngle);
      RPoint += X*pbWallThick;
      ModelSupport::buildPlaneRotAxis
	(SMap,hutIndex+124,RPoint,X,-Z,ringWallAngle);
      RPoint += X*outerThick;
      ModelSupport::buildPlaneRotAxis
	(SMap,hutIndex+134,RPoint,X,-Z,ringWallAngle);
    }

  if (holeRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder
      (SMap,hutIndex+107,Origin+X*holeXStep+Z*holeZStep,Y,holeRadius);
  
  return;
}

void
OpticsHutch::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("OpticsHutch","createObjects");

  std::string Out;

  Out=ModelSupport::getSetComposite(SMap,hutIndex,"1 -2 3 (-4:-104) 5 -6 ");
  makeCell("Void",System,cellIndex++,0,0.0,Out);

  // walls:
  int HI(hutIndex);

  std::list<int> matList({skinMat,pbMat,skinMat});
  for(const std::string& layer : {"Inner","Lead","Outer"})
    {
      const int mat=matList.front();
      matList.pop_front();
      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,"1 -2 -3M 13M 5 -6 ");
      makeCell(layer+"Wall",System,cellIndex++,mat,0.0,Out);

      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,
					"1 -2  4M  104M  (-14M:-114M) 5 -6 ");
      makeCell(layer+"Wall",System,cellIndex++,mat,0.0,Out);
      
      //back wall
      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,"2M -12M 33 (-34:-134) 5 -6 107 ");
      makeCell(layer+"BackWall",System,cellIndex++,mat,0.0,Out);
      
      // roof
      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,"1 -32 33 (-34:-134) 6M -16M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
      setCell(layer+"Roof",cellIndex-1);
      HI+=10;
    }

  if (holeRadius>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI," 2 -2M -107 ");
      makeCell("ExitHole",System,cellIndex++,0,0.0,Out);
    }
  
  // floor
  Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,
				    "1 -2M 3M (-4M:-104M) 15 -5 ");
  makeCell("Floor",System,cellIndex++,floorMat,0.0,Out);
  
  // Exclude:
  Out=ModelSupport::getComposite
    (SMap,hutIndex,HI," 1 -2M 3M (-4M:-104M) 15 -6M ");
  addOuterSurf(Out);      

  return;
}

void
OpticsHutch::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("OpticsHutch","createLinks");

  const double extraT(innerThick+outerThick+pbBackThick);
  setConnect(0,Origin-Y,-Y);
  setConnect(1,Origin+Y*(length+extraT),Y);
  
  setLinkSurf(0,-SMap.realSurf(hutIndex+1));
  setLinkSurf(1,SMap.realSurf(hutIndex+32));

  
  return;
}

void
OpticsHutch::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("OpticsHutch","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
