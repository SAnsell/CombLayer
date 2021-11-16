/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/CollUnit.cxx
 *
 * Copyright (c) 2004-2021 Stuart Ansell
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
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Line.h"
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
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Quaternion.h"

#include "CollUnit.h"

namespace xraySystem
{

CollUnit::CollUnit(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


CollUnit::~CollUnit()
  /*!
    Destructor
  */
{}

void
CollUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CollUnit","populate");

  FixedRotate::populate(Control);

  tubeRadius=Control.EvalVar<double>(keyName+"TubeRadius");
  tubeWidth=Control.EvalVar<double>(keyName+"TubeWidth");
  tubeWall=Control.EvalVar<double>(keyName+"TubeWall");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");
  plateLength=Control.EvalVar<double>(keyName+"PlateLength");
  plateWidth=Control.EvalVar<double>(keyName+"PlateWidth");


  nHoles=Control.EvalVar<size_t>(keyName+"NHoles");
  holeGap=Control.EvalVar<double>(keyName+"HoleGap");
  holeRadius=Control.EvalVar<double>(keyName+"HoleRadius");
  
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  screenMat=ModelSupport::EvalMat<int>(Control,keyName+"ScreenMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  return;
}

void
CollUnit::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CollUnit","createSurfaces");

  if (!isActive("Flange"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1001,Origin+Z*(3*plateLength),Y);
      ExternalCut::setCutSurf("Flange",-SMap.realSurf(buildIndex+1001));
    }
  // main plate
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(plateThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(plateThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(plateWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(plateWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+5,Origin-Z*(plateLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+6,Origin+Z*(plateLength/2.0),Y);

  // Holes
  int BI(buildIndex+100);
  Geometry::Vec3D HCent(Origin-Z*static_cast<double>((nHoles-1)/2));
  for(size_t i=0;i<nHoles;i++)
    {
      ModelSupport::buildCylinder(SMap,BI+7,HCent,Y,holeRadius);
      HCent+=Z*holeGap;
      BI+=10;
    }
  
  return;
}

void
CollUnit::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CollUnit","createObjects");

  HeadRule HR;

  // linear pneumatics feedthrough
  int BI(buildIndex+100);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  for(size_t i=0;i<nHoles;i++)
    {
      std::string hName("Hole"+std::to_string(i));
      const HeadRule HRHole=ModelSupport::getHeadRule(SMap,BI,"-7");
      makeCell(hName,System,cellIndex++,voidMat,0.0,HRHole);
      HR*=HRHole.complement();
    }
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");  
  addOuterSurf(HR);      

  return;
}


void
CollUnit::createLinks()
  /*!
    Create all the linkes [need front/back to join/use InnerZone]
  */
{
  ELog::RegMethod RegA("CollUnit","createLinks");

  return;
}


void
CollUnit::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("CollUnit","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
