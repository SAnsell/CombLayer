/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DTL.cxx
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

#include "DTL.h"

namespace essSystem
{

DTL::DTL(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,3),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

DTL::DTL(const DTL& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  length(A.length),radius(A.radius),height(A.height),
  wallThick(A.wallThick),
  mainMat(A.mainMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: DTL to copy
  */
{}

DTL&
DTL::operator=(const DTL& A)
  /*!
    Assignment operator
    \param A :: DTL to copy
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
      radius=A.radius;
      height=A.height;
      wallThick=A.wallThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
    }
  return *this;
}

DTL*
DTL::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new DTL(*this);
}
  
DTL::~DTL() 
  /*!
    Destructor
  */
{}

void
DTL::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("DTL","populate");

  FixedOffset::populate(Control);
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  // height=Control.EvalVar<double>(keyName+"Height");
  // wallThick=Control.EvalVar<double>(keyName+"WallThick");

  // mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  // wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}
  
void
DTL::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("DTL","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}
  
void
DTL::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DTL","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length),Y);
  ModelSupport::buildCylinder(SMap,surfIndex+7,Origin,Y,radius);

  return;
}
  
void
DTL::createObjects(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int sideIndex)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC :: Object to connect to
    \param sideIndex :: side link point
  */
{
  ELog::RegMethod RegA("DTL","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," -7 -2 ") +
    FC.getSignedLinkString(sideIndex);
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  addOuterSurf(Out);

  return;
}

  
void
DTL::createLinks(const attachSystem::FixedComp& FC,
		 const long int sideIndex)

  /*!
    Create all the linkes
    \param FC :: Object to connect to
    \param sideIndex :: side link point
  */
{
  ELog::RegMethod RegA("DTL","createLinks");

  FixedComp::setLinkCopy(0,FC,sideIndex);
  
  FixedComp::setConnect(1,Origin+Y*length,-Y);
  FixedComp::setLinkSurf(1,-SMap.realSurf(surfIndex+2));

  FixedComp::setConnect(2,Origin+Y*(length/2.0)+Z*radius,Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(surfIndex+7));

  //  for (int i=0; i<3; i++)
  //    ELog::EM << keyName << " " << getLinkPt(i) << ELog::endDiag;

  return;
}
  
  

  
void
DTL::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("DTL","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System,FC,sideIndex);
  createLinks(FC,sideIndex);
  insertObjects(System);              

  return;
}

}  // essSystem
