/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/ConnectorTube.cxx
 *
 * Copyright (c) 2019-2021 by Stuart Ansell
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
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
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

#include "ConnectorTube.h"

namespace xraySystem
{

ConnectorTube::ConnectorTube(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ConnectorTube::~ConnectorTube()
  /*!
    Destructor
  */
{}

void
ConnectorTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("ConnectorTube","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerLength=Control.EvalVar<double>(keyName+"InnerLength");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerLength=Control.EvalVar<double>(keyName+"OuterLength");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}

void
ConnectorTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ConnectorTube","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // main cell
  ModelSupport::buildPlane
    (SMap,buildIndex+11,Origin-Y*(length/2.0-flangeLength),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+12,Origin+Y*(length/2.0-flangeLength),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  // flange
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeRadius);

  // inner surface
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,innerRadius);
  // shield / outer surface
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,outerRadius);

  // inner cell
  ModelSupport::buildPlane
    (SMap,buildIndex+201,Origin-Y*(innerLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+202,Origin+Y*(innerLength/2.0),Y);

  // outer cell
  ModelSupport::buildPlane
    (SMap,buildIndex+301,Origin-Y*(outerLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+302,Origin+Y*(outerLength/2.0),Y);

  return;
}

void
ConnectorTube::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ConnectorTube","createObjects");

  HeadRule HR;
  const HeadRule frontHR(getRule("front"));
  const HeadRule backHR(getRule("back"));

  // direct inner
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);
  // inner spacer:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"207 -7 201 -202");
  makeCell("Inner",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"207 -7 -201");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"207 -7 202");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR*backHR);

  // main wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 7");
  makeCell("Inner",System,cellIndex++,wallMat,0.0,HR*frontHR*backHR);

  // flanges
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 -107 17 ");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -107 17 ");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,HR*backHR);

  // Outer

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"17 -307 301 -302");
  makeCell("Outer",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"11 -12 -107 17 (307:-301:302)");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-107");
  addOuterSurf(HR*frontHR*backHR);

  return;
}


void
ConnectorTube::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("ConnectorTube","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);
  
  return;
}

void
ConnectorTube::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Si\mulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("ConnectorTube","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // constructSystem
