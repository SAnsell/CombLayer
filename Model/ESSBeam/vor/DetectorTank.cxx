/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/vor/DetectorTank.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "DetectorTank.h"

namespace essSystem
{

DetectorTank::DetectorTank(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

DetectorTank::DetectorTank(const DetectorTank& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  midAngle(A.midAngle),height(A.height),innerThick(A.innerThick),
  frontThick(A.frontThick),backThick(A.backThick),
  roofThick(A.roofThick),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: DetectorTank to copy
  */
{}

DetectorTank&
DetectorTank::operator=(const DetectorTank& A)
  /*!
    Assignment operator
    \param A :: DetectorTank to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      midAngle=A.midAngle;
      height=A.height;
      innerThick=A.innerThick;
      frontThick=A.frontThick;
      backThick=A.backThick;
      roofThick=A.roofThick;
      wallMat=A.wallMat;
    }
  return *this;
}


DetectorTank::~DetectorTank() 
 /*!
   Destructor
 */
{}

void
DetectorTank::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database of variable
 */
{
  ELog::RegMethod RegA("DetectorTank","populate");
  
  FixedRotate::populate(Control);

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  height=Control.EvalVar<double>(keyName+"Height");
  midAngle=Control.EvalVar<double>(keyName+"MidAngle");

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  // Material
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}
  
void
DetectorTank::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DetectorTank","createSurface");

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+1,
				  Origin,Y,Z,midAngle);
  const Geometry::Plane* PX=SMap.realPtr<Geometry::Plane>(buildIndex+1);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+11,PX,-frontThick);
  

  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(height/2.0+roofThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(height/2.0+roofThick),Z);


  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin,Z,outerRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,
			      Z,innerRadius+innerThick);
  ModelSupport::buildCylinder(SMap,buildIndex+18,Origin,
			      Z,outerRadius+backThick);

  
  return;
}

void
DetectorTank::createObjects(Simulation& System)
  /*!
    Adds the vacuum object
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DetectorTank","createObjects");

  HeadRule HR;

  // Main voids
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 5 -6");
  makeCell("SampleVoid",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 5 -6 7 -8");
  makeCell("DetVoid",System,cellIndex++,0,0.0,HR);

  // WALLS:
  // sample
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 7 5 -6 -1");
  makeCell("Steel",System,cellIndex++,wallMat,0.0,HR);

  // front wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-8 17 5 -6 -1 11");
  makeCell("Steel",System,cellIndex++,wallMat,0.0,HR);
  // Detector wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6 11 8 -18");
  makeCell("Steel",System,cellIndex++,wallMat,0.0,HR);

  // Roof wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"6 -16 11 -18 17");
  makeCell("RoofSteel",System,cellIndex++,wallMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"6 -16 -17");
  //  System.addCell(cellIndex++,wallMat,0.0,HR);
  //  addCell("RoofSteel",cellIndex-1);

    // Detector wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6 11 8 -18");
  //  System.addCell(cellIndex++,wallMat,0.0,HR);
  //  addCell("Steel",cellIndex-1);
  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -16 ((11 -18) : -17)");
  addOuterSurf(HR);
  
  return;
}

void
DetectorTank::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("DetectorTank","createLinks");
  
  return;
}

void
DetectorTank::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("DetectorTank","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE essSystem
