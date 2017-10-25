/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/Bucket.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "Bucket.h"

namespace moderatorSystem
{

Bucket::Bucket(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,1),
  cdIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(cdIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Bucket::Bucket(const Bucket& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  cdIndex(A.cdIndex),cellIndex(A.cellIndex),
  radius(A.radius),thickness(A.thickness),
  openZ(A.openZ),topZ(A.topZ),matN(A.matN)
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
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
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
Bucket::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Bucket","populate");
  
  FixedOffset::populate(Control);
  
  radius=Control.EvalVar<double>(keyName+"Radius"); 
  thickness=Control.EvalVar<double>(keyName+"Thick"); 
  openZ=Control.EvalVar<double>(keyName+"OpenZ"); 
  topZ=Control.EvalVar<double>(keyName+"TopZ"); 

  matN=ModelSupport::EvalMat<int>(Control,keyName+"Mat"); 
  
  return;
}
  

void
Bucket::createUnitVector(const attachSystem::FixedComp& CUnit,
			 const long int sideIndex)
  /*!
    Create the unit vectors
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex ::: link point
  */
{
  ELog::RegMethod RegA("Bucket","createUnitVector");

  FixedComp::createUnitVector(CUnit,sideIndex);
  applyOffset();

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
		  const attachSystem::FixedComp& FUnit,
		  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Bucket","createAll");
  populate(System.getDataBase());

  createUnitVector(FUnit,sideIndex);
  createSurfaces();
  createObjects(System);
  insertObjects(System);       

  return;
}
  
}  // NAMESPACE moderatorSystem
