/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: danmax/danmaxFrontEnd.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <iterator>
#include <memory>
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "BlockZone.h"

#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "Wiggler.h"
#include "R3FrontEnd.h"

#include "danmaxFrontEnd.h"

namespace xraySystem
{

// Note currently uncopied:
  
danmaxFrontEnd::danmaxFrontEnd(const std::string& Key) :
  R3FrontEnd(Key),
  undulatorTube(new constructSystem::PortTube(newName+"UndulatorTube")),
  undulator(new Wiggler(newName+"Undulator"))
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(undulatorTube);
  OR.addObject(undulator);
}
  
danmaxFrontEnd::~danmaxFrontEnd()
  /*!
    Destructor
   */
{}

void
danmaxFrontEnd::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("danmaxFrontEnd","createLinks");
  
  setLinkCopy(0,*undulatorTube,1);
  setLinkCopy(1,*lastComp,2);
  return;
}
  

const attachSystem::FixedComp&
danmaxFrontEnd::buildUndulator(Simulation& System,
			       const attachSystem::FixedComp& preFC,
			       const long int preSideIndex)
/*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
    \param preFC :: Initial cell
    \param preSideIndex :: Initial side index
    \return link object 
  */
{
  ELog::RegMethod RegA("danmaxFrontEnd","buildUndulator");

  int outerCell;

  undulatorTube->createAll(System,preFC,preSideIndex);
  outerCell=buildZone.createUnit(System,*undulatorTube,2);
  
  undulator->addInsertCell(undulatorTube->getCell("Void"));
  undulator->createAll(System,*undulatorTube,0);

  CellMap::addCell("UndulatorOuter",outerCell);
  undulatorTube->insertAllInCell(System,outerCell);
  return *undulatorTube;

}

  
}   // NAMESPACE xraySystem

