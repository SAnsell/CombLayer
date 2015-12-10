/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/odin/PinHole.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
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
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "CellMap.h"
#include "HoleShape.h"
#include "RotaryCollimator.h"
#include "Jaws.h"

#include "PinHole.h"

namespace essSystem
{

PinHole::PinHole(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,2),
  attachSystem::CellMap(),
  pinIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pinIndex+1),
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
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  pinIndex(A.pinIndex),cellIndex(A.cellIndex),CollA(A.CollA),
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
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
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
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");

  return;
}

void
PinHole::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("PinHole","createUnitVector");
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

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
  ModelSupport::buildPlane(SMap,pinIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,pinIndex+2,Origin+Y*(length),Y);
  ModelSupport::buildCylinder(SMap,pinIndex+7,Origin,Y,radius);

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

  Out=ModelSupport::getComposite(SMap,pinIndex,"1 -2 -7");
  addOuterSurf(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  setCell("Void",cellIndex-1);

  return;
}

void
PinHole::createLinks()
  /*!
    Simple beam transfer link points
  */
{
  
  ELog::RegMethod RegA("PinHole","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(pinIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(pinIndex+2));
  
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
  CollA->createAll(System,FC,FIndex);
  CollB->createAll(System,FC,FIndex);
  JawX->setInsertCell(getCell("Void"));
  JawX->createAll(System,FC,FIndex);

  JawXZ->setInsertCell(getCell("Void"));
  JawXZ->createAll(System,FC,FIndex);
  
  return;
}
  
}  // NAMESPACE essSystem
