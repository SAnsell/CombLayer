/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essConstruct/TwinChopperFlat.cxx
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
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedRotate.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "boxPort.h"
#include "TwinBase.h"
#include "TwinChopperFlat.h"

namespace essConstruct
{

TwinChopperFlat::TwinChopperFlat(const std::string& Key) :
  TwinBase(Key),
  frontBoxPort(new essConstruct::boxPort(Key,"BoxPortFront")),
  backBoxPort(new essConstruct::boxPort(Key,"BoxPortBack"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(frontBoxPort);
  OR.addObject(backBoxPort);
}

TwinChopperFlat::~TwinChopperFlat() 
  /*!
    Destructor
  */
{}
  
void
TwinChopperFlat::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TwinChopperFlat","createObjects");
  
  const attachSystem::FixedComp& Main=getKey("Main");

  TwinBase::createObjects(System);

  frontBoxPort->setInnerExclude();
  frontBoxPort->addInsertCell(getCell("FrontCase"));
  frontBoxPort->setFront(SMap.realSurf(buildIndex+1));
  frontBoxPort->setBack(-SMap.realSurf(buildIndex+11));
  frontBoxPort->createAll(System,Main,0);

  // Back ring seal
  backBoxPort->setInnerExclude();
  backBoxPort->addInsertCell(getCell("BackCase"));
  backBoxPort->setFront(SMap.realSurf(buildIndex+12));
  backBoxPort->setBack(-SMap.realSurf(buildIndex+2));
  backBoxPort->createAll(System,Main,0);

  return;
}
  
void
TwinChopperFlat::createAll(Simulation& System,
                       const attachSystem::FixedComp& beamFC,
                       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param beamFC :: FixedComp at the beam centre
    \param FIndex :: side index
  */
{
  ELog::RegMethod RegA("TwinChopperFlat","createAll(FC)");

  populate(System.getDataBase());
  TwinBase::createUnitVector(beamFC,FIndex);
  TwinBase::createSurfaces();    
  createObjects(System);
  
  TwinBase::createLinks();

  TwinBase::processInsert(System);
  TwinBase::buildMotors(System);

  return;
}
  
}  // NAMESPACE constructSystem
