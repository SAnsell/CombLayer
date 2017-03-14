/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/FaradayCup.cxx
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
#include "BaseMap.h"
#include "FixedOffset.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "FaradayCup.h"

namespace essSystem
{

FaradayCup::FaradayCup(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FaradayCup::FaradayCup(const FaradayCup& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  length(A.length),outerRadius(A.outerRadius),innerRadius(A.innerRadius),
  colLength(A.colLength),
  mainMat(A.mainMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: FaradayCup to copy
  */
{}

FaradayCup&
FaradayCup::operator=(const FaradayCup& A)
  /*!
    Assignment operator
    \param A :: FaradayCup to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      length=A.length;
      outerRadius=A.outerRadius;
      innerRadius=A.innerRadius;
      colLength=A.colLength;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
    }
  return *this;
}

FaradayCup*
FaradayCup::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new FaradayCup(*this);
}
  
FaradayCup::~FaradayCup() 
  /*!
    Destructor
  */
{}

void
FaradayCup::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("FaradayCup","populate");

  FixedOffset::populate(Control);
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  length=Control.EvalVar<double>(keyName+"Length");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  colLength=Control.EvalVar<double>(keyName+"CollimatorLength");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}
  
void
FaradayCup::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FaradayCup","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}
  
void
FaradayCup::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FaradayCup","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length/2.0),Y);

  ModelSupport::buildCylinder(SMap,surfIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+17,Origin,Y,outerRadius);

  return;
}
  
void
FaradayCup::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FaradayCup","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 7 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 -17 ");
  addOuterSurf(Out);

  return;
}

  
void
FaradayCup::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("FaradayCup","createLinks");

  //  FixedComp::setConnect(0,Origin,-Y);
  //  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));
  
  return;
}
  
  

  
void
FaradayCup::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FaradayCup","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);              

  return;
}

}  // essSystem
