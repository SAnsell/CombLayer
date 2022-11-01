/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/HighBay.cxx
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
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "Bunker.h"
#include "HighBay.h"


namespace essSystem
{

HighBay::HighBay(const std::string& key) :
  attachSystem::FixedComp(key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param key :: Name of component
  */
{}

HighBay::HighBay(const HighBay& A) : 
  attachSystem::FixedComp(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  length(A.length),height(A.height),roofThick(A.roofThick),
  wallMat(A.wallMat),roofMat(A.roofMat)
  /*!
    Copy constructor
    \param A :: HighBay to copy
  */
{}

HighBay&
HighBay::operator=(const HighBay& A)
  /*!
    Assignment operator
    \param A :: HighBay to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      length=A.length;
      height=A.height;
      roofThick=A.roofThick;
      wallMat=A.wallMat;
      roofMat=A.roofMat;
    }
  return *this;
}

HighBay::~HighBay() 
  /*!
    Destructor
  */
{}
  
void
HighBay::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("HighBay","populate");
  
  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  roofThick=Control.EvalVar<double>(keyName+"Thick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");
  
  return;
}
  
  
void
HighBay::createSurfaces()

  /*!
    Create all the surfaces
   */
{
  ELog::RegMethod RegA("HighBay","createSurface");

  const HeadRule roofHR=getRule("roofOuter");
  
  const int leftSurfRoof=roofHR.getPrimarySurface();
  
  ModelSupport::buildShiftedPlane(SMap,buildIndex+6,
				  SMap.realPtr<Geometry::Plane>(leftSurfRoof),
				  height);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+16,
				  SMap.realPtr<Geometry::Plane>(leftSurfRoof),
				  height+roofThick);
  
  const int rightSurfRoof=leftSurfRoof;
  if (rightSurfRoof!=leftSurfRoof)
    {
      ModelSupport::buildShiftedPlane
	(SMap,buildIndex+106,SMap.realPtr<Geometry::Plane>(rightSurfRoof),
	 height);
      
      ModelSupport::buildShiftedPlane
	(SMap,buildIndex+116,SMap.realPtr<Geometry::Plane>(rightSurfRoof),
	 height+roofThick);
    }

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);  
  return;
}

void
HighBay::createObjects(Simulation& System)
  /*!
    Create all the objects
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("HighBay","createObjects");

  HeadRule HR;

  const HeadRule curtainHR=getComplementRule("curtainCut");
  const HeadRule frontCutHR=getRule("frontCut");
  const HeadRule leftIWallHR=getRule("leftWallInner");
  const HeadRule leftOWallHR=getRule("leftWallOuter");
  const HeadRule rightIWallHR=getRule("rightWallInner");
  const HeadRule rightOWallHR=getRule("rightWallOuter");
  const HeadRule bunkerTopHR=getRule("roofOuter");

  // void area
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -6");
  HR*=leftIWallHR*rightIWallHR*frontCutHR*bunkerTopHR;
  makeCell("Void",System,cellIndex++,0,0.0,HR*curtainHR);

  // Roof area
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 6 -16");
  HR*=leftIWallHR*rightIWallHR*frontCutHR;
  makeCell("Roof",System,cellIndex++,roofMat,0.0,HR);

  // Left wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -16");
  HR*=leftOWallHR*leftIWallHR.complement();
  HR*=bunkerTopHR*frontCutHR;
  makeCell("LeftWall",System,cellIndex++,wallMat,0.0,HR);
  // Right wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -16");
  HR*=rightOWallHR*rightIWallHR.complement();
  HR*=bunkerTopHR*frontCutHR;
  makeCell("RightWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -16");
  HR*=leftOWallHR*rightOWallHR*frontCutHR*bunkerTopHR;
  addOuterSurf(HR);

  return;
}
  
void
HighBay::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int sideIndex)
  
  /*!
    Generic function to initialize everything
    \param Control :: DataBase for variables
    \param leftBunker :: Left bunker unit
    \param rightBunker :: right bunker Unit
  */
{
  ELog::RegMethod RegA("HighBay","createAll");

  populate(System.getDataBase());  
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
