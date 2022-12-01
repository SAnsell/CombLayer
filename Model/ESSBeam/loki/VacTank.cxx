/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/loki/VacTank.cxx
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
#include "VacTank.h"

namespace essSystem
{

VacTank::VacTank(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

VacTank::VacTank(const VacTank& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  radius(A.radius),length(A.length),frontThick(A.frontThick),
  sideThick(A.sideThick),backThick(A.backThick),
  noseLen(A.noseLen),windowThick(A.windowThick),
  windowRadius(A.windowRadius),
  windowInsetLen(A.windowInsetLen),wallMat(A.wallMat),
  windowMat(A.windowMat)
  /*!
    Copy constructor
    \param A :: VacTank to copy
  */
{}

VacTank&
VacTank::operator=(const VacTank& A)
  /*!
    Assignment operator
    \param A :: VacTank to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      radius=A.radius;
      length=A.length;
      frontThick=A.frontThick;
      sideThick=A.sideThick;
      backThick=A.backThick;
      noseLen=A.noseLen;
      windowThick=A.windowThick;
      windowRadius=A.windowRadius;
      windowInsetLen=A.windowInsetLen;
      wallMat=A.wallMat;
      windowMat=A.windowMat;
    }
  return *this;
}

VacTank::~VacTank() 
 /*!
   Destructor
 */
{}

void
VacTank::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database of variable
 */
{
  ELog::RegMethod RegA("VacTank","populate");
  
  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");


  windowThick=Control.EvalVar<double>(keyName+"WindowThick");
  windowRadius=Control.EvalVar<double>(keyName+"WindowRadius");
  windowInsetLen=Control.EvalVar<double>(keyName+"WindowInsetLen");
  
  // Material
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  windowMat=ModelSupport::EvalMat<int>(Control,keyName+"WindowMat");

  return;
}
  
void
VacTank::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("VacTank","createSurface");

  // Main cylinder tank
  ModelSupport::buildPlane(SMap,buildIndex+1,
			   Origin+Y*windowInsetLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,
			   Origin+Y*(windowInsetLen+length),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(windowInsetLen+length+backThick),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+17,
			   Origin,Y,radius+sideThick);

  //
  // Nose cone:
  //
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin-Y*windowThick,Y);

  const double coneAngle=180.0*atan((radius-windowRadius)/windowInsetLen)/M_PI;
  const Geometry::Vec3D coneOrigin=Origin-Y*
    (windowRadius*windowInsetLen)/(radius-windowRadius);

  ModelSupport::buildCone(SMap,buildIndex+108,
			  coneOrigin,Y,coneAngle);
  ModelSupport::buildCone(SMap,buildIndex+118,
			  coneOrigin-Y*(frontThick*sin(coneAngle*M_PI/180.0)),
			  Y,coneAngle);
  
  return;
}

void
VacTank::createObjects(Simulation& System)
  /*!
    Adds the vacuum object
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("VacTank","createObjects");
  

  HeadRule HR;

  // Main voids
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  // Steel layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -12 -17 -118 (2:7)");
  makeCell("Steel",System,cellIndex++,wallMat,0.0,HR);

  // Nose cone
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -1 -108");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  // Nose Steel
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -1 -118 108");
  makeCell("Steel",System,cellIndex++,wallMat,0.0,HR);

  // window
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -101 -118");
  makeCell("Window",System,cellIndex++,windowMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -118 -12 -17");
  addOuterSurf(HR);
  
  return;
}

void
VacTank::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("VacTank","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*windowThick,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+101));
  FixedComp::setConnect(1,Origin+Y*(length+windowInsetLen),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}

void
VacTank::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("VacTank","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE zoomSystem
