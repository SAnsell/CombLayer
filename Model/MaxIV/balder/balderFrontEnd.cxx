/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: balder/balderFrontEnd.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "BlockZone.h"

#include "VacuumBox.h"
#include "Wiggler.h"
#include "R3FrontEnd.h"
#include "R3FrontEndFMBB.h"

#include "balderFrontEnd.h"

namespace xraySystem
{

// Note currently uncopied:

balderFrontEnd::balderFrontEnd(const std::string& Key) :
  R3FrontEndFMBB(Key),
  wigglerBox(new constructSystem::VacuumBox(newName+"WigglerBox",1)),
  wiggler(new Wiggler(newName+"Wiggler"))
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(wigglerBox);
  OR.addObject(wiggler);
}

balderFrontEnd::~balderFrontEnd()
  /*!
    Destructor
   */
{}

void
balderFrontEnd::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("balderFrontEnd","createLinks");

  setLinkCopy(0,*wigglerBox,1);
  setLinkCopy(1,*lastComp,2);
  return;
}


const attachSystem::FixedComp&
balderFrontEnd::buildUndulator(Simulation& System,
			       const attachSystem::FixedComp& preFC,
			       const long int preSideIndex)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
    \param masterCell :: Main cell with all components in
    \param preFC :: Initial cell
    \param preSideIndex :: Initial side index
    \return link object
  */
{
  ELog::RegMethod RegA("balderFrontEnd","buildUndulator");

  int outerCell;

  wigglerBox->createAll(System,preFC,preSideIndex);
  outerCell=buildZone.createUnit(System,*wigglerBox,2);

  wiggler->addInsertCell(wigglerBox->getCell("Void"));
  wiggler->createAll(System,*wigglerBox,0);


  CellMap::addCell("WiggerOuter",outerCell);
  wigglerBox->insertInCell(System,outerCell);
  return *wigglerBox;

}


}   // NAMESPACE xraySystem
