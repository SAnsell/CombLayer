/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/ConcreteWall.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <array>

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
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "VacVessel.h"
#include "FlightLine.h"
#include "World.h"

#include "ConcreteWall.h"

namespace bibSystem
{

ConcreteWall::ConcreteWall(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  wallIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(wallIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ConcreteWall::ConcreteWall(const ConcreteWall& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  wallIndex(A.wallIndex),cellIndex(A.cellIndex),
  innerRadius(A.innerRadius),
  thickness(A.thickness),height(A.height),base(A.base),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: ConcreteWall to copy
  */
{}

ConcreteWall&
ConcreteWall::operator=(const ConcreteWall& A)
  /*!
    Assignment operator
    \param A :: ConcreteWall to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      innerRadius=A.innerRadius;
      thickness=A.thickness;
      height=A.height;
      base=A.base;
      mat=A.mat;
    }
  return *this;
}


ConcreteWall::~ConcreteWall()
  /*!
    Destructor
  */
{}

void
ConcreteWall::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to use
 */
{
  ELog::RegMethod RegA("ConcreteWall","populate");
  
  FixedOffset::populate(Control);

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  thickness=Control.EvalVar<double>(keyName+"WallThick");
  height=Control.EvalVar<double>(keyName+"WallHeight");
  base=Control.EvalVar<double>(keyName+"Base");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}
  
void
ConcreteWall::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("ConcreteWall","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}
  
void
ConcreteWall::createSurfaces(const attachSystem::FixedComp& ReflFC,
			     const long int sideIndex)
  /*!
    Create All the surfaces
    \param ReflFC :: Inner wall of reflector
    \param sideIndex :: Side of inner wall of reflector
  */
{
  ELog::RegMethod RegA("ConcreteWall","createSurface");
  // rotation of axis:

  const long int SI(std::abs(sideIndex));
  SMap.addMatch(wallIndex+7,ReflFC.getLinkSurf(SI));
  SMap.addMatch(wallIndex+1,ReflFC.getLinkSurf(1+((SI+1) % 3)));
  SMap.addMatch(wallIndex+2,ReflFC.getLinkSurf(1+((SI+2) % 3)));

  ModelSupport::buildCylinder(SMap,wallIndex+17,Origin,Z,innerRadius);
  ModelSupport::buildCylinder(SMap,wallIndex+27,Origin,Z,innerRadius+thickness);

  ModelSupport::buildPlane(SMap,wallIndex+5,Origin-Z*base,Z);
  ModelSupport::buildPlane(SMap,wallIndex+6,Origin+Z*height,Z);



  return;
}

void
ConcreteWall::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ConcreteWall","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,wallIndex,"7 -17 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,wallIndex,"17 -27 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

  // Inner core separte to avoid need to track by guides
  Out=ModelSupport::getComposite(SMap,wallIndex,"-7 5 -6 (-1:-2)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,wallIndex,"-27 5 -6"); 
  addOuterSurf(Out);
  addBoundarySurf(Out);
  
  return;
}

void
ConcreteWall::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("ConcreteWall","createLinks");
  // set Links :: Inner links:

  FixedComp::setConnect(0,Origin+Y*innerRadius,Z);
  FixedComp::setLinkSurf(0,-SMap.realSurf(wallIndex+17));

  FixedComp::setConnect(1,Origin+Y*(innerRadius+thickness),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(wallIndex+27));

  FixedComp::setConnect(2,Origin-Z*base,-Z);
  FixedComp::setLinkSurf(2,-SMap.realSurf(wallIndex+5));

  FixedComp::setConnect(3,Origin+Z*height,Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(wallIndex+6));

  return;
}

void
ConcreteWall::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const size_t originIndex,
		       const attachSystem::FixedComp& RefFC, 
		       const size_t sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param originIndex
    \param RefFC :: Reflector 
    \param sideIndex :: inner surface 
   */
{
  ELog::RegMethod RegA("ConcreteWall","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,originIndex+1);
  createSurfaces(RefFC,sideIndex+1);
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE bibSystem
