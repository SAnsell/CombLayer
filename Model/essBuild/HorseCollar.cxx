/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/HorseCollar.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
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
#include "stringCombine.h"
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
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "HorseCollar.h"

namespace essSystem
{

HorseCollar::HorseCollar(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

HorseCollar::HorseCollar(const HorseCollar& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  length(A.length),internalRadius(A.internalRadius),mainRadius(A.mainRadius),
  mainMat(A.mainMat)
  /*!
    Copy constructor
    \param A :: HorseCollar to copy
  */
{}

HorseCollar&
HorseCollar::operator=(const HorseCollar& A)
  /*!
    Assignment operator
    \param A :: HorseCollar to copy
    \return *this
  */
{
  if (this!=&A)
  {
    attachSystem::ContainedComp::operator=(A);
    attachSystem::FixedOffset::operator=(A);
    attachSystem::CellMap::operator=(A);
    length=A.length;
    internalRadius=A.internalRadius;
    mainRadius=A.mainRadius;
    mainMat=A.mainMat;
  }
  return *this;
}

HorseCollar::~HorseCollar() 
  /*!
    Destructor
  */
{}

void
HorseCollar::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("HorseCollar","populate");
  
  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  internalRadius=Control.EvalVar<double>(keyName+"internalRadius");
  mainRadius=Control.EvalVar<double>(keyName+"mainRadius");
  
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  
  return;
}

void
HorseCollar::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin/internalRadius
    \param sideIndex :: Side for monolith
  */
{
  ELog::RegMethod RegA("HorseCollar","createUnitVector");
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
  
void
HorseCollar::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("HorseCollar","createSurface");
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,internalRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,mainRadius);
  
  return;
}

void
HorseCollar::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("HorseCollar","createObjects");
  
  std::string Out;
  // Void
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -7 -2");
  System.addCell(MonteCarlo::Object(cellIndex++,0.0,0.0,Out));
  addCell("Hole",cellIndex-1);
  
  // Steel wrapper
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 7 -17 -2 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out));
  addCell("Wall",cellIndex-1);
  
  // Outer boundary
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -17 -2 ");
  addOuterSurf(Out);
  
  return;
}

void
HorseCollar::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("HorseCollar","createLinks");
  
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+Y*length,Y);
  
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  
  return;
}

void
HorseCollar::createAll(Simulation& System,
                       const attachSystem::FixedComp& FC,
                       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: Side of monolith
  */
{
  ELog::RegMethod RegA("HorseCollar","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);
  
  return;
}

}  // NAMESPACE essSystem
