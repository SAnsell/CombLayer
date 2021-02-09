/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/RefCutOut.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
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
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "RefCutOut.h"

namespace moderatorSystem
{

RefCutOut::RefCutOut(const std::string& Key)  :
  attachSystem::FixedOffset(Key,1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

RefCutOut::RefCutOut(const RefCutOut& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  tarLen(A.tarLen),tarOut(A.tarOut),radius(A.radius),matN(A.matN)
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
      attachSystem::FixedOffset::operator=(A);
      tarLen=A.tarLen;
      tarOut=A.tarOut;
      radius=A.radius;
      matN=A.matN;
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
  

  active=Control.EvalDefVar<int>(keyName+"Active",1);
  if (active)
    {
      xyAngle=Control.EvalVar<double>(keyName+"XYAngle"); 
      zAngle=Control.EvalVar<double>(keyName+"ZAngle"); 
      tarLen=Control.EvalVar<double>(keyName+"TargetDepth");
      tarOut=Control.EvalVar<double>(keyName+"TargetOut");
      radius=Control.EvalVar<double>(keyName+"Radius"); 
      
      matN=ModelSupport::EvalMat<int>(Control,keyName+"Mat"); 
    }
  
  return;
}
  

void
RefCutOut::createUnitVector(const attachSystem::FixedComp& CUnit,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param CUnit :: Fixed unit that it is connected to 
  */
{
  ELog::RegMethod RegA("RefCutOut","createUnitVector");
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  yStep+=tarLen;
  FixedOffset::createUnitVector(CUnit,sideIndex);

  return;
}

void
RefCutOut::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RefCutOut","createSurface");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*tarOut,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin+Y*tarOut,Y,radius);
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
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -7 ");
  addOuterSurf(Out);

  // Inner Void
  Out+=" "+ContainedComp::getContainer();
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

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
  */
{
  ELog::RegMethod RegA("RefCutOut","createAll");
  populate(System.getDataBase());

  if (active)
    {
      createUnitVector(FUnit,sideIndex);
      createSurfaces();
      createObjects(System);
      insertObjects(System);       
    }
  return;
}
  
}  // NAMESPACE moderatorSystem
