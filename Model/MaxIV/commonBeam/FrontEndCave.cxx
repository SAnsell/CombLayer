/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/FrontEndCave.cxx
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

#include "FrontEndCave.h"

namespace xraySystem
{

FrontEndCave::FrontEndCave(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

FrontEndCave::FrontEndCave(const FrontEndCave& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  frontWallThick(A.frontWallThick),length(A.length),
  ringGap(A.ringGap),ringRadius(A.ringRadius),ringThick(A.ringThick),
  outerGap(A.outerGap),outerThick(A.outerThick),
  floorDepth(A.floorDepth),roofHeight(A.roofHeight),
  floorThick(A.floorThick),roofThick(A.roofThick),
  frontHoleRadius(A.frontHoleRadius),frontWallMat(A.frontWallMat),
  wallMat(A.wallMat),roofMat(A.roofMat),floorMat(A.floorMat)
  /*!
    Copy constructor
    \param A :: FrontEndCave to copy
  */
{}

FrontEndCave&
FrontEndCave::operator=(const FrontEndCave& A)
  /*!
    Assignment operator
    \param A :: FrontEndCave to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      frontWallThick=A.frontWallThick;
      length=A.length;
      ringGap=A.ringGap;
      ringRadius=A.ringRadius;
      ringThick=A.ringThick;
      outerGap=A.outerGap;
      outerThick=A.outerThick;
      floorDepth=A.floorDepth;
      roofHeight=A.roofHeight;
      floorThick=A.floorThick;
      roofThick=A.roofThick;
      frontHoleRadius=A.frontHoleRadius;
      frontWallMat=A.frontWallMat;
      wallMat=A.wallMat;
      roofMat=A.roofMat;
      floorMat=A.floorMat;
    }
  return *this;
}

FrontEndCave::~FrontEndCave() 
  /*!
    Destructor
  */
{}

void
FrontEndCave::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("FrontEndCave","populate");
  
  FixedOffset::populate(Control);

  frontWallThick=Control.EvalVar<double>(keyName+"FrontWallThick");
  
  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  ringGap=Control.EvalVar<double>(keyName+"RingGap");
  ringRadius=Control.EvalVar<double>(keyName+"RingRadius");
  ringThick=Control.EvalVar<double>(keyName+"RingThick");
  outerGap=Control.EvalVar<double>(keyName+"OuterGap");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");
  floorDepth=Control.EvalVar<double>(keyName+"FloorDepth");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  roofHeight=Control.EvalVar<double>(keyName+"RoofHeight");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  segmentXOffset=Control.EvalVar<double>(keyName+"SegmentXOffset");
  segmentAngle=Control.EvalVar<double>(keyName+"SegmentAngle");
  segmentLength=Control.EvalVar<double>(keyName+"SegmentLength");
  segmentThick=Control.EvalVar<double>(keyName+"SegmentThick");

  frontHoleRadius=Control.EvalVar<double>(keyName+"FrontHoleRadius");

  frontWallMat=ModelSupport::EvalMat<int>(Control,keyName+"FrontWallMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");

  return;
}

void
FrontEndCave::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("FrontEndCave","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
 
void
FrontEndCave::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("FrontEndCave","createSurfaces");

  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outerGap,X);
  // mid line divider
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(ringRadius+ringGap),X); 
  ModelSupport::buildCylinder(SMap,buildIndex+7,
			      Origin+X*(ringRadius+ringGap),Z,ringRadius); 
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*floorDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*roofHeight,Z);  

  // Walls
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(frontWallThick+length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(outerGap+outerThick),X);
  ModelSupport::buildCylinder(SMap,buildIndex+17,
		       Origin+X*(ringRadius+ringGap+ringThick),Z,ringRadius); 
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-
			   Z*(floorDepth+floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+
			   Z*(roofHeight+roofThick),Z);  

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,frontHoleRadius); 


  Geometry::Vec3D RPoint(Origin+X*segmentXOffset+
			 Y*(frontWallThick+length/2.0));
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+104,RPoint,X,-Z,segmentAngle);
  RPoint += X*segmentThick;
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+114,RPoint,X,-Z,segmentAngle);

  return;
}

void
FrontEndCave::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("FrontEndCave","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3  7 -4 5 -6 ");
  makeCell("Void",System,cellIndex++,0,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 3 -4 7  5 -6 107 ");
  makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 -107 ");
  makeCell("FrontWallHole",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -12 -7 -4 17 5 -6 ");
  makeCell("RingWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -12 -3 13 5 -6 ");
  makeCell("OutWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -12 13 -4 17 15 -5 ");
  makeCell("Floor",System,cellIndex++,floorMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -12 13 -4 17 6 -16 ");
  makeCell("Roof",System,cellIndex++,roofMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -12 13 -4 17  15 -16 ");
  addOuterSurf(Out);      

  return;
}

void
FrontEndCave::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("FrontEndCave","createLinks");
  
  setConnect(0,Origin-Y*(length/2.0),-Y);
  setConnect(1,Origin+Y*(frontWallThick+length/2.0),Y);
  
  setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  setLinkSurf(1,SMap.realSurf(buildIndex+12));

  return;
}

void
FrontEndCave::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("FrontEndCave","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE essSystem
