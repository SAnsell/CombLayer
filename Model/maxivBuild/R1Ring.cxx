/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/R1Cave.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "R1Ring.h"

namespace xraySystem
{

R1Ring::R1Ring(const std::string& Key) : 
  attachSystem::FixedOffset(Key,12),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(2,"outerWall");
  nameSideIndex(3,"ringWall");
  nameSideIndex(4,"floor");
  nameSideIndex(5,"roof");
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

  hexRadius=Control.EvalVar<double>(keyName+"HexRadius");
  hexWallThick=Control.EvalVar<double>(keyName+"HexWallThick");


  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");

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

  // quick way to rotate outgoing vector to
  // dividing vector
  const Geometry::Quaternion Qz=Geometry::Quaternion::calcQRotDeg(120.0,Z);
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
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+roofThick),Z);
  
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

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -3 -13 -23 -33 -43 -53 15 -16" );
  makeCell("InnerVoid",System,cellIndex++,0,0.0,Out);

  int prevN(buildIndex+50);
  int surfN(buildIndex);
  for(size_t i=0;i<6;i++)
    {
      Out=ModelSupport::getComposite(SMap,surfN,prevN,buildIndex,
				     " 7M -7 3 -103 15N -16N" );
      makeCell("Wall",System,cellIndex++,0,0.0,Out);
    }

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -103 -113 -123 -133 -143 -153 15 -16" );
  addOuterSurf(Out);      

  return;
}

void
R1Ring::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("R1Ring","createLinks");
  

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
  return;
}
  
}  // NAMESPACE xraySystem
