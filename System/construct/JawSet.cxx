/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/JawSet.cxx
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Jaws.h"

#include "JawSet.h"

namespace constructSystem
{

JawSet::JawSet(const std::string& Key) :
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  JawX(new constructSystem::Jaws(Key+"Vert")),
  JawXZ(new constructSystem::Jaws(Key+"Diag"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(JawX);
  OR.addObject(JawXZ);	  
 }

JawSet::JawSet(const JawSet& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  JawX(A.JawX),JawXZ(A.JawXZ),radius(A.radius),
  length(A.length)
  /*!
    Copy constructor
    \param A :: JawSet to copy
  */
{}

JawSet&
JawSet::operator=(const JawSet& A)
  /*!
    Assignment operator
    \param A :: JawSet to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      JawX=A.JawX;
      JawXZ=A.JawXZ;
      radius=A.radius;
      length=A.length;
    }
  return *this;
}

JawSet::~JawSet() 
  /*!
    Destructor
  */
{}

void
JawSet::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("JawSet","populate");
  
  FixedRotate::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");

  return;
}

void
JawSet::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("JawSet","createSurfaces");

  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  return;
}

void
JawSet::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("JawSet","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  addOuterSurf(HR);

  return;
}

void
JawSet::createLinks()
  /*!
    Simple beam transfer link points
  */
{
  
  ELog::RegMethod RegA("JawSet","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  
  return;
}

  
void
JawSet::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("JawSet","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  JawX->setInsertCell(getCell("Void"));
  JawX->createAll(System,FC,FIndex);
  
  JawXZ->setInsertCell(getCell("Void"));
  JawXZ->createAll(System,FC,FIndex);
  
  return;
}
  
}  // NAMESPACE constructSystem
