/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderator/Bucket.cxx
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "Bucket.h"

namespace moderatorSystem
{

Bucket::Bucket(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,1),
  cdIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(cdIndex+1),populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Bucket::Bucket(const Bucket& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  cdIndex(A.cdIndex),cellIndex(A.cellIndex),
  populated(A.populated),xStep(A.xStep),yStep(A.yStep),radius(A.radius),
  thickness(A.thickness),openZ(A.openZ),topZ(A.topZ),matN(A.matN)
  /*!
    Copy constructor
    \param A :: Bucket to copy
  */
{}

Bucket&
Bucket::operator=(const Bucket& A)
  /*!
    Assignment operator
    \param A :: Bucket to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      xStep=A.xStep;
      yStep=A.yStep;
      radius=A.radius;
      thickness=A.thickness;
      openZ=A.openZ;
      topZ=A.topZ;
      matN=A.matN;
    }
  return *this;
}

Bucket::~Bucket() 
  /*!
    Destructor
  */
{}

void
Bucket::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Bucket","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep"); 
  yStep=Control.EvalVar<double>(keyName+"YStep"); 
  radius=Control.EvalVar<double>(keyName+"Radius"); 
  thickness=Control.EvalVar<double>(keyName+"Thick"); 
  openZ=Control.EvalVar<double>(keyName+"OpenZ"); 
  topZ=Control.EvalVar<double>(keyName+"TopZ"); 

  matN=ModelSupport::EvalMat<int>(Control,keyName+"Mat"); 
  
  populated |= 1;
  return;
}
  

void
Bucket::createUnitVector(const attachSystem::FixedComp& CUnit)
  /*!
    Create the unit vectors
    - Y Points down Target
    - X Across the target
    - Z up 
    \param CUnit :: Fixed unit that it is connected to 
  */
{
  ELog::RegMethod RegA("Bucket","createUnitVector");

  FixedComp::createUnitVector(CUnit);

  Origin+=X*xStep+Y*yStep;


  return;
}

void
Bucket::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Bucket","createSurface");

  ModelSupport::buildPlane(SMap,cdIndex+1,Origin+Z*openZ,Z);
  ModelSupport::buildPlane(SMap,cdIndex+2,Origin+Z*topZ,Z);
  ModelSupport::buildPlane(SMap,cdIndex+12,Origin+Z*(topZ-thickness),Z);

  ModelSupport::buildCylinder(SMap,cdIndex+7,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,cdIndex+17,Origin,Z,radius-thickness);

  return;
}

void
Bucket::createObjects(Simulation& System)
  /*!
    Adds the Cylinder
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Bucket","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,cdIndex,"1 -2 -7 (17 : 12)");
  addOuterSurf(Out);

  // Inner Void
  Out+=" "+ContainedComp::getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,matN,0.0,Out));

  return;
}

  
void
Bucket::createAll(Simulation& System,
		     const attachSystem::FixedComp& FUnit)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
  */
{
  ELog::RegMethod RegA("Bucket","createAll");
  populate(System);

  createUnitVector(FUnit);
  createSurfaces();
  createObjects(System);
  insertObjects(System);       

  return;
}
  
}  // NAMESPACE moderatorSystem
