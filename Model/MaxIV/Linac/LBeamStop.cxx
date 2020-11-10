/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/LBeamStop.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#include "Exception.h"

#include "LBeamStop.h"

namespace tdcSystem
{

LBeamStop::LBeamStop(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

LBeamStop::~LBeamStop()
  /*!
    Destructor
  */
{}

void
LBeamStop::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("LBeamStop","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");

  innerVoidLen=Control.EvalVar<double>(keyName+"InnerVoidLen");
  innerLength=Control.EvalVar<double>(keyName+"InnerLength");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");

  midVoidLen=Control.EvalVar<double>(keyName+"MidVoidLen");
  midLength=Control.EvalVar<double>(keyName+"MidLength");
  midRadius=Control.EvalVar<double>(keyName+"MidRadius");

  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  midMat=ModelSupport::EvalMat<int>(Control,keyName+"MidMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");

  return;
}

void
LBeamStop::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("LBeamStop","createSurfaces");

  // pipe and flanges
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,midRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,outerRadius);

  // full outer
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*length,Y);

  // mid
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*midVoidLen,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+12,Origin+Y*(midVoidLen+midLength),Y);

  // inner
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*innerVoidLen,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+2,Origin+Y*(innerVoidLen+innerLength),Y);
  
  return;
}

void
LBeamStop::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("LBeamStop","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -1 -7 ");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -7 ");
  makeCell("CarbonInner",System,cellIndex++,innerMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -11 -17 7");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 -17 (7:2)");
  makeCell("MidLayer",System,cellIndex++,midMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -22 -27 (17:12)");
  makeCell("Outer",System,cellIndex++,outerMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -22 -27 ");
  addOuterSurf(Out);

  return;
}


void
LBeamStop::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("LBeamStop","createLinks");

  FixedComp::setConnect(0,Origin,Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}

void
LBeamStop::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("LBeamStop","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
