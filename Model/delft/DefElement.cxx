/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/DefElement.cxx
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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "FuelLoad.h"
#include "RElement.h"
#include "DefElement.h"

namespace delftSystem
{


DefElement::DefElement(const size_t XI,const size_t YI,
		       const std::string& Key) :
  RElement(XI,YI,Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: Grid position
    \param YI :: Grid position 
    \param Key :: Keyname for basic cell
  */
{}


void
DefElement::populate(const FuncDataBase&)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param  :: FuncDatabase
  */
{
  ELog::RegMethod RegA("DefElement","populate");

  return;
}

void
DefElement::createSurfaces()
  /*!
    Creates/duplicates the surfaces for this block
  */
{  
  ELog::RegMethod RegA("DefElement","createSurface");


  return;
}

void
DefElement::createObjects(Simulation&)
  /*!
    Create the objects
    \param :: Simulation placeholder
  */
{
  ELog::RegMethod RegA("DefElement","createObjects");

  
  return;
}


void
DefElement::createLinks()
  /*!
    Creates a full attachment set
    0 - 1 standard points
    2 - 3 beamaxis points
  */
{

  return;
}

void
DefElement::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Unit
    \param OG :: Origin				
  */
{
  ELog::RegMethod RegA("DefElement","createAll(DefElement)");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}


} // NAMESPACE delftSystem
