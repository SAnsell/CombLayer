/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/PressureVessel.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "PressureVessel.h"

namespace delftSystem
{

PressureVessel::PressureVessel(const std::string& Key)  :
  attachSystem::FixedRotate(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PressureVessel::PressureVessel(const PressureVessel& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  frontLength(A.frontLength),frontRadius(A.frontRadius),
  frontWall(A.frontWall),backLength(A.backLength),
  backRadius(A.backRadius),backWall(A.backWall),
  sideRadius(A.sideRadius),sideWall(A.sideWall),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: PressureVessel to copy
  */
{}

PressureVessel&
PressureVessel::operator=(const PressureVessel& A)
  /*!
    Assignment operator
    \param A :: PressureVessel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      frontLength=A.frontLength;
      frontRadius=A.frontRadius;
      frontWall=A.frontWall;
      backLength=A.backLength;
      backRadius=A.backRadius;
      backWall=A.backWall;
      sideRadius=A.sideRadius;
      sideWall=A.sideWall;
      wallMat=A.wallMat;
    }
  return *this;
}


PressureVessel::~PressureVessel() 
 /*!
   Destructor
 */
{}

void
PressureVessel::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to used
 */
{
  ELog::RegMethod RegA("PressureVessel","populate");

  attachSystem::FixedRotate::populate(Control);

  reshiftOrigin=Control.EvalDefVar<int>(keyName+"ReshiftOrigin",0);
  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  frontRadius=Control.EvalVar<double>(keyName+"FrontRadius");
  frontWall=Control.EvalVar<double>(keyName+"FrontWall");

  backLength=Control.EvalVar<double>(keyName+"BackLength");
  backRadius=Control.EvalVar<double>(keyName+"BackRadius");
  backWall=Control.EvalVar<double>(keyName+"BackWall");
  
  sideRadius=Control.EvalVar<double>(keyName+"SideRadius");
  sideWall=Control.EvalVar<double>(keyName+"SideWall");
  

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
PressureVessel::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PressureVessel","createSurfaces");

  //  front:
  //[arbitary divider plane]
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*frontLength,Y);  
  ModelSupport::buildSphere(SMap,buildIndex+8,Origin-Y*frontLength,frontRadius);
  ModelSupport::buildSphere(SMap,buildIndex+18,Origin-Y*frontLength,frontRadius+frontWall);
  
  // back:
  // [arbitary divider plane]
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*backLength,Y);
  ModelSupport::buildSphere(SMap,buildIndex+108,Origin+Y*backLength,backRadius);
  ModelSupport::buildSphere(SMap,buildIndex+118,Origin+Y*backLength,backRadius+backWall);
  
  // Main
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,sideRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,sideRadius+sideWall);
  return;
}


void
PressureVessel::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("PressureVessel","createObjects");
  
  HeadRule HR;

  // Main void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(-8:1) (-108:-2) -7");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  
  // Main Wall
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(-18:1) (-118:-2) -17 (7 : (-1 8) : (2 108))");
  System.addCell(cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(-18:1) (-118:-2) -17");
  addOuterSurf(HR);
  
  return;
}

void
PressureVessel::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("PressureVessel","createLinks");

  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+18));
  FixedComp::setBridgeSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+118));
  FixedComp::setBridgeSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));

  FixedComp::setLineConnect(0,Origin,-Y);
  FixedComp::setLineConnect(1,Origin,Y);
  FixedComp::setConnect(2,Origin+Z*(sideRadius+sideWall),Z);

  return;
}

void
PressureVessel::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("PressureVessel","createAll");

  
  populate(System.getDataBase());
  const double yUnit=(reshiftOrigin>0) ? 
    -backLength+backRadius+backWall : 
    frontLength+frontRadius+frontWall;
  
  createCentredUnitVector(FC,sideIndex,yUnit);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE delftSystem
