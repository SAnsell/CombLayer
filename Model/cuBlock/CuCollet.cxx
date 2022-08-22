/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   cuBlock/CuCollet.cxx
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
#include <iterator>
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
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "CuCollet.h"

namespace cuSystem
{

CuCollet::CuCollet(const std::string& Key)  : 
  attachSystem::FixedRotate(Key,3),attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

CuCollet::CuCollet(const CuCollet& A) : 
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  radius(A.radius),cuRadius(A.cuRadius),holeRadius(A.holeRadius),
  cuGap(A.cuGap),cerThick(A.cerThick),steelThick(A.steelThick),
  cuThick(A.cuThick),cerMat(A.cerMat),steelMat(A.steelMat),
  cuMat(A.cuMat)
  /*!
    Copy constructor
    \param A :: CuCollet to copy
  */
{}

CuCollet&
CuCollet::operator=(const CuCollet& A)
  /*!
    Assignment operator
    \param A :: CuCollet to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      radius=A.radius;
      cuRadius=A.cuRadius;
      holeRadius=A.holeRadius;
      cuGap=A.cuGap;
      cerThick=A.cerThick;
      steelThick=A.steelThick;
      cuThick=A.cuThick;
      cerMat=A.cerMat;
      steelMat=A.steelMat;
      cuMat=A.cuMat;
    }
  return *this;
}


CuCollet::~CuCollet() 
  /*!
    Destructor
  */
{}

void
CuCollet::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("CuCollet","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");   
  holeRadius=Control.EvalVar<double>(keyName+"HoleRadius");   
  cuRadius=Control.EvalVar<double>(keyName+"CuRadius");   
  cuGap=Control.EvalVar<double>(keyName+"CuGap");   

  cerThick=Control.EvalVar<double>(keyName+"CeramicThick");   
  steelThick=Control.EvalVar<double>(keyName+"SteelThick");   
  cuThick=Control.EvalVar<double>(keyName+"CuThick");   

  cerMat=ModelSupport::EvalMat<int>(Control,keyName+"CeramicMat");
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");
  cuMat=ModelSupport::EvalMat<int>(Control,keyName+"CuMat");
      
  return;
}

void
CuCollet::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("CuCollet","createSurface");

  // ceramic:
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*cerThick,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);  

  // Steel:
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(steelThick+cerThick),Y);

  // Cu:
  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin+Y*(steelThick+cerThick+cuGap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(steelThick+cerThick+cuGap+cuThick),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,holeRadius);  
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,cuRadius);  

  return;
}

void
CuCollet::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("CuCollet","createObjects");
  
  HeadRule HR;
  // Ceramic
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7");
  System.addCell(cellIndex++,cerMat,0.0,HR);
  // Steel
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 -7");
  System.addCell(cellIndex++,steelMat,0.0,HR);
  if (cuGap>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -21 -27");
      System.addCell(cellIndex++,0,0.0,HR);
    }

  // Cu : Checked for existance of inner
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"21 -22 -27 17");
  System.addCell(cellIndex++,cuMat,0.0,HR);
  if (cuGap>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"21 -22 -17");
      System.addCell(cellIndex++,0.0,0.0,HR);
    }
  // Outer Boundary : 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -22 -27 (-7:12)");
  addOuterSurf(HR);
  
  return;
}


void
CuCollet::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("CuCollet","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+
			Y*(cuGap+cuThick+cerThick+steelThick),Y);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));  

  return;
}

void
CuCollet::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param FC :: FixedComp for orgin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("CuCollet","createAll");
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
