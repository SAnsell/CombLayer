/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MonoShutter.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "ShutterUnit.h"
#include "MonoShutter.h"

namespace xraySystem
{

MonoShutter::MonoShutter(const std::string& Key) :
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB"),
  attachSystem::FixedOffset(Key,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  
  shutterPipe(new constructSystem::PortTube(keyName+"Pipe")),
  monoShutterA(new xraySystem::ShutterUnit(keyName+"UnitA")),
  monoShutterB(new xraySystem::ShutterUnit(keyName+"UnitB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}


MonoShutter::~MonoShutter()
  /*!
    Destructor
   */
{}

void
MonoShutter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MonoShutter","populate");

  FixedOffset::populate(Control);
    
  return;
}

void
MonoShutter::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)

  /*!
    Create the unit vectors.
    The first beamFC is to set the X,Y,Z relative to the beam
    and the origin at the beam centre position.
    The origin is corrected so that the flange centre + alpha*Y'
    where Y' is its primary direction is closeest to 
    beamCenter + Y'*beta. The flange centre point is taken.

    \param FC :: FixedComp for origin at beam height
    \param sideIndex :: link point of centre [and axis]
  */
{
  ELog::RegMethod RegA("MonoShutter","createUnitVector");


  createUnitVector(FC,sideIndex);

  applyOffset();
  return;
}

void
MonoShutter::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("MonoShutter","createSurfaces");
			   
  return; 
}

void
MonoShutter::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("MonoShutter","createObjects");


  return; 
}

void
MonoShutter::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MonoShutter","createLinks");
  
  return;
}


void
MonoShutter::buildComponents(Simulation& System)
  /*!
    Construct the object to be built
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("MonoShutter","buildComponents");

  
  shutterPipe->addInsertCell("Main",getCC("Main").getInsertCells());

  if (isActive("front"))
    shutterPipe->setFront(this->getRuleStr("front"));
  
  shutterPipe->createAll(System,*this,0);

  const constructSystem::portItem& PIA=shutterPipe->getPort(0);
  monoShutterA->addInsertCell("Inner",shutterPipe->getCell("Void"));
  monoShutterA->addInsertCell("Inner",PIA.getCell("Void"));
  monoShutterA->addInsertCell("Outer",getCC("Main").getInsertCells());
  monoShutterA->createAll(System,*shutterPipe,0,PIA,2);

  const constructSystem::portItem& PIB=shutterPipe->getPort(1);
  monoShutterB->addInsertCell("Inner",shutterPipe->getCell("Void"));
  monoShutterB->addInsertCell("Inner",PIB.getCell("Void"));
  monoShutterB->addInsertCell("Outer",getCC("Main").getInsertCells());
  monoShutterB->createAll(System,*shutterPipe,1,PIB,2);

  return;
}

  
void
MonoShutter::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp for beam origin
    \param sideIndex :: link point of centre [and axis]
   */
{
  ELog::RegMethod RegA("MonoShutter","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  buildComponents(System);

  return;
}

}  // NAMESPACE xraySystem
