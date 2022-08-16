/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/RefCutOut.cxx
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
#include "RefCutOut.h"

namespace moderatorSystem
{

RefCutOut::RefCutOut(const std::string& Key)  :
  attachSystem::FixedRotate(Key,1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

RefCutOut::RefCutOut(const RefCutOut& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  tarOutStep(A.tarOutStep),
  radius(A.radius),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: RefCutOut to copy
  */
{}

RefCutOut&
RefCutOut::operator=(const RefCutOut& A)
  /*!
    Assignment operator
    \param A :: RefCutOut to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      tarOutStep=A.tarOutStep;
      radius=A.radius;
      mat=A.mat;
    }
  return *this;
}

RefCutOut::~RefCutOut() 
  /*!
    Destructor
  */
{}

void
RefCutOut::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("RefCutOut","populate");

  FixedRotate::populate(Control);
  
  tarOutStep=Control.EvalVar<double>(keyName+"TarOutStep");
  radius=Control.EvalVar<double>(keyName+"Radius"); 
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat"); 

  return;
}
  
void
RefCutOut::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RefCutOut","createSurface");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  return;
}

void
RefCutOut::createObjects(Simulation& System)
  /*!
    Adds the Cylinder
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RefCutOut","createObjects");
  
  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -7");

  // Inner Void
  ELog::EM<<"Center == "<<Origin<<ELog::endDiag;
  makeCell("Inner",System,cellIndex++,0,0.0,HR*getRule("RefEdge"));
  addOuterSurf(HR);
  return;
}

  
void
RefCutOut::createAll(Simulation& System,
		     const attachSystem::FixedComp& FUnit,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("RefCutOut","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FUnit,sideIndex,tarOutStep);
  createSurfaces();
  createObjects(System);
  insertObjects(System);       

  return;
}
  
}  // NAMESPACE moderatorSystem
