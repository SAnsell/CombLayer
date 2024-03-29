/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/Bucket.cxx
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"


#include "Bucket.h"

namespace moderatorSystem
{

Bucket::Bucket(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Bucket::Bucket(const Bucket& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  radius(A.radius),thickness(A.thickness),
  openZ(A.openZ),topZ(A.topZ),mat(A.mat)
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
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      radius=A.radius;
      thickness=A.thickness;
      openZ=A.openZ;
      topZ=A.topZ;
      mat=A.mat;
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
  
  FixedRotate::populate(Control);
  
  radius=Control.EvalVar<double>(keyName+"Radius"); 
  thickness=Control.EvalVar<double>(keyName+"Thick"); 
  openZ=Control.EvalVar<double>(keyName+"OpenZ"); 
  topZ=Control.EvalVar<double>(keyName+"TopZ"); 

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat"); 
  
  return;
}
  
void
Bucket::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Bucket","createSurface");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Z*openZ,Z);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Z*topZ,Z);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Z*(topZ-thickness),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius-thickness);

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

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7 (17 : 12)");
  addOuterSurf(HR);

  HR*=ExternalCut::getComplementRule("FLwish");
  HR*=ExternalCut::getComplementRule("FLnarrow");
  makeCell("Bucket",System,cellIndex++,mat,0.0,HR);

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
