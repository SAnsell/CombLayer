/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/PinHole.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "RotaryCollimator.h"
#include "Jaws.h"

#include "PinHole.h"

namespace constructSystem
{

PinHole::PinHole(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  CollA(new constructSystem::RotaryCollimator(Key+"CollA")),
  CollB(new constructSystem::RotaryCollimator(Key+"CollB")),
  JawX(new constructSystem::Jaws(Key+"JawVert")),
  JawXZ(new constructSystem::Jaws(Key+"JawDiag"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(CollA);
  OR.addObject(CollB);

  OR.addObject(JawX);
  OR.addObject(JawXZ);	  
 }

PinHole::PinHole(const PinHole& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  CollA(A.CollA),
  CollB(A.CollB),JawX(A.JawX),JawXZ(A.JawXZ),radius(A.radius),
  length(A.length)
  /*!
    Copy constructor
    \param A :: PinHole to copy
  */
{}

PinHole&
PinHole::operator=(const PinHole& A)
  /*!
    Assignment operator
    \param A :: PinHole to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      CollA=A.CollA;
      CollB=A.CollB;
      JawX=A.JawX;
      JawXZ=A.JawXZ;
      radius=A.radius;
      length=A.length;
    }
  return *this;
}

PinHole::~PinHole() 
  /*!
    Destructor
  */
{}

void
PinHole::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("PinHole","populate");
  
  FixedRotate::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");

  return;
}

void
PinHole::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("PinHole","createSurfaces");

  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  return;
}

void
PinHole::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("PinHole","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -7");
  makeCell("Void",System,cellIndex++,0,0.0,Out);

  addOuterSurf(Out);

  return;
}

void
PinHole::createLinks()
  /*!
    Simple beam transfer link points
  */
{
  
  ELog::RegMethod RegA("PinHole","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));
  
  return;
}

  
void
PinHole::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("PinHole","createAll(FC)");

  populate(System.getDataBase());

  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  CollA->setInsertCell(getCell("Void"));
  CollB->setInsertCell(getCell("Void"));
  CollA->createAll(System,*this,0);
  CollB->createAll(System,*this,0);

  JawX->setInsertCell(getCell("Void"));
  JawX->createAll(System,*this,0);

  JawXZ->setInsertCell(getCell("Void"));
  JawXZ->createAll(System,*this,0);
  return;
}
  
}  // NAMESPACE constructSystem
