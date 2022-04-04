/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/SoilRoof.cxx
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
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "SurfMap.h"
#include "ExternalCut.h"

#include "SoilRoof.h"

namespace tdcSystem
{

SoilRoof::SoilRoof(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

void
SoilRoof::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SoilRoof","populate");

  FixedRotate::populate(Control);

  topWidth=Control.EvalVar<double>(keyName+"TopWidth");
  baseWidth=Control.EvalVar<double>(keyName+"BaseWidth");
  fullHeight=Control.EvalVar<double>(keyName+"FullHeight");
  baseHeight=Control.EvalVar<double>(keyName+"BaseHeight");

  soilMat=ModelSupport::EvalMat<int>(Control,keyName+"SoilMat");

  return;
}

void
SoilRoof::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    This creates an shifted origin -- the normal origin is calculated
    BUT then the origin is shifted up so it impacts the roof surface.
    \param FC :: Fixed Component
    \param sideIndex :: link point 
   */
{
  ELog::RegMethod RegA("SoilRoof","createUnitVector");

  
  FixedRotate::createUnitVector(FC,sideIndex);

  Origin=ExternalCut::getRule("Roof").trackPoint(Origin,Z);
  ELog::EM<<"Orign = "<<Origin<<ELog::endDiag;
  return;
}
  
void
SoilRoof::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SoilRoof","createSurfaces");

  // we have Pre-calculated the origin at the roof so this works:  
  SurfMap::makePlane("Top",SMap,buildIndex+6,Origin+Z*fullHeight,Z);

  // note cross product already taken.
  const Geometry::Vec3D PAxis(Z*((baseWidth-topWidth)/2.0)+X*fullHeight);
  const Geometry::Vec3D MAxis(Z*((baseWidth-topWidth)/2.0)-X*fullHeight);
  // axis point outwared
  SurfMap::makePlane("Left",SMap,buildIndex+3,
		     Origin-X*(baseWidth/2.0),MAxis.unit());

  SurfMap::makePlane("Right",SMap,buildIndex+4,
		     Origin+X*(baseWidth/2.0),PAxis.unit());


  return;
}

void
SoilRoof::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SoilRoof","createObjects");

  HeadRule HR;
  // these must exist:
  const HeadRule roofHR=ExternalCut::getRule("Roof");
  const HeadRule frontHR=ExternalCut::getRule("Front");
  const HeadRule backHR=ExternalCut::getRule("Back");
  const HeadRule allHR(roofHR*frontHR*backHR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-3 -4 -6");
  makeCell("Berm",System,cellIndex++,soilMat,0.0,HR*allHR);

  addOuterSurf(HR*allHR);

  return;
}


void
SoilRoof::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("SoilRoof","createLinks");

  // ExternalCut::createLink("front",*this,0,Origin,Y);
  // ExternalCut::createLink("back",*this,1,Origin,Y);


  return;
}

void
SoilRoof::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("SoilRoof","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  
  return;
}

}  // tdcSystem
