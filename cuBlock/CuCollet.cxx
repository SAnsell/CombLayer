/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   cuBlock/CuCollet.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "PointOperation.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SurInter.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "World.h"
#include "CuCollet.h"

namespace cuSystem
{

CuCollet::CuCollet(const std::string& Key)  : 
  attachSystem::FixedComp(Key,3),attachSystem::ContainedComp(),
  cuIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(cuIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}


CuCollet::~CuCollet() 
  /*!
    Destructor
  */
{}

void
CuCollet::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("CuCollet","populate");

  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");   

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
CuCollet::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("CuCollet","createUnitVector");

  attachSystem::FixedComp::createUnitVector(World::masterOrigin());
  applyShift(xStep,yStep,zStep);
  
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
  ModelSupport::buildPlane(SMap,cuIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,cuIndex+2,Origin+Y*cerThick,Y);
  ModelSupport::buildCylinder(SMap,cuIndex+7,Origin,Y,radius);  

  // Steel:
  ModelSupport::buildPlane(SMap,cuIndex+12,
			   Origin+Y*(steelThick+cerThick),Y);

  // Cu:
  ModelSupport::buildPlane(SMap,cuIndex+21,
			   Origin+Y*(steelThick+cerThick+cuGap),Y);
  ModelSupport::buildPlane(SMap,cuIndex+22,
			   Origin+Y*(steelThick+cerThick+cuGap+cuThick),Y);
  ModelSupport::buildCylinder(SMap,cuIndex+17,Origin,Y,holeRadius);  
  ModelSupport::buildCylinder(SMap,cuIndex+27,Origin,Y,cuRadius);  

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
  
  std::string Out;

  // Ceramic
  Out=ModelSupport::getComposite(SMap,cuIndex,"1 -2 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cerMat,0.0,Out));
  // Steel
  Out=ModelSupport::getComposite(SMap,cuIndex,"2 -12 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
  if (cuGap>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,cuIndex,"12 -21 -27");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
    }

  // Cu : Checked for existance of inner
  Out=ModelSupport::getSetComposite(SMap,cuIndex,"21 -22 -27 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cuMat,0.0,Out));
  if (cuGap>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite(SMap,cuIndex,"21 -22 -17 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0.0,0.0,Out));
    }
  // Outer Boundary : 
  Out=ModelSupport::getComposite(SMap,cuIndex,"1 -22 -27 (-7:12)");
  addOuterSurf(Out);
  
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

  FixedComp::setLinkSurf(0,-SMap.realSurf(cuIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(cuIndex+2));  

  return;
}

void
CuCollet::createAll(Simulation& System)
  /*!
    Create the shutter
    \param System :: Simulation to process
  */
{
  ELog::RegMethod RegA("CuCollet","createAll");
  populate(System);
  createUnitVector();
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
