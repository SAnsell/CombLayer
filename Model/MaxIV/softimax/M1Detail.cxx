/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimax/M1Detail.cxx
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
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "M1Mirror.h"
#include "M1BackPlate.h"
#include "M1FrontShield.h"
#include "M1Detail.h"

namespace xraySystem
{

M1Detail::M1Detail(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  mirror(new M1Mirror(keyName+"Mirror")),
  cClamp(new M1BackPlate(keyName+"CClamp")),
  frontShield(new M1FrontShield(keyName+"FShield"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(mirror);
  OR.addObject(cClamp);
}

M1Detail::~M1Detail()
  /*!
    Destructor
   */
{}

void
M1Detail::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1Detail","populate");

  FixedRotate::populate(Control);

  return;
}


void
M1Detail::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1Detail","createSurfaces");

  
  return; 
}

void
M1Detail::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1Detail","createObjects");

  mirror->addInsertCell(getInsertCells());
  mirror->createAll(System,*this,0);
  ELog::EM<<"mirror -> "<<mirror->getLinkPt(0)<<ELog::endDiag;

  cClamp->addInsertCell(getInsertCells());
  cClamp->setCutSurf("FarEnd",*mirror,"back");
  cClamp->setCutSurf("NearEnd",*mirror,"front");
  cClamp->setCutSurf("Back",*mirror,"outSide");
  cClamp->setCutSurf("Mirror",*mirror,"mirrorSide");
  cClamp->setCutSurf("Top",*mirror,"top");
  cClamp->setCutSurf("Base",*mirror,"base");
  cClamp->createAll(System,*mirror,"backPlateOrg");
  
  return; 
}

void
M1Detail::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1Detail","createLinks");

  
  return;
}

void
M1Detail::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1Detail","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();

  return;
}

}  // NAMESPACE xraySystem
