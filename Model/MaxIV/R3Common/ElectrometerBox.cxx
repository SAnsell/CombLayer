/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/ElectrometerBox.cxx
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "ElectrometerBox.h"

namespace xraySystem
{

ElectrometerBox::ElectrometerBox(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


ElectrometerBox::~ElectrometerBox()
  /*!
    Destructor
  */
{}

void
ElectrometerBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("ElectrometerBox","populate");

  FixedRotate::populate(Control);

  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");

  frontThick=Control.EvalVar<double>(keyName+"FrontThick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  skinThick=Control.EvalVar<double>(keyName+"SkinThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
ElectrometerBox::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  
  ELog::RegMethod RegA("ElectrometerBox","createSurfaces");

  if (!isActive("Floor"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(voidHeight/2.0),Z);
      ExternalCut::setCutSurf("Floor",SMap.realSurf(buildIndex+5));
    }
  ExternalCut::makeShiftedSurf(SMap,"Floor",buildIndex+6,Z,voidHeight);
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(voidWidth/2.0),X);

  ModelSupport::buildPlane
    (SMap,buildIndex+11,Origin-Y*(skinThick+voidLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+12,Origin+Y*(skinThick+voidLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin-X*(skinThick+voidWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+14,Origin+X*(skinThick+voidWidth/2.0),X);
  
  ModelSupport::buildPlane
    (SMap,buildIndex+21,Origin-Y*(frontThick+skinThick+voidLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+22,Origin+Y*(backThick+skinThick+voidLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+23,Origin-X*(sideThick+skinThick+voidWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+24,Origin+X*(sideThick+skinThick+voidWidth/2.0),X);

  ModelSupport::buildPlane
    (SMap,buildIndex+31,Origin-Y*(frontThick+2*skinThick+voidLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+32,Origin+Y*(backThick+2*skinThick+voidLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+33,Origin-X*(sideThick+2*skinThick+voidWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+34,Origin+X*(sideThick+2*skinThick+voidWidth/2.0),X);

  return;
}

void
ElectrometerBox::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ElectrometerBox","createObjects");

  const HeadRule floorHR=getRule("Floor");

  HeadRule HR;
  ELog::EM<<"Box == "<<keyName<<ELog::endDiag;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 -6");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 13 -14 (-1:2:-3:4) -6");
  makeCell("InnerWall",System,cellIndex++,skinMat,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"21 -22 23 -24 (-11:12:-13:14) -6");
  makeCell("Shield",System,cellIndex++,wallMat,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"31 -32 33 -34 (-21:22:-23:24) -6");
  makeCell("Shield",System,cellIndex++,skinMat,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 33 -34 -6");
  addOuterSurf(HR*floorHR);
  return;
}


void
ElectrometerBox::createLinks()
  /*!
    Create all the linkes [need front/back to join/use InnerZone]
  */
{
  ELog::RegMethod RegA("ElectrometerBox","createLinks");

  return;
}

void
ElectrometerBox::createAll(Simulation& System,
			   const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("ElectrometerBox","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
