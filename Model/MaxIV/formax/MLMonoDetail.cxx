/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MLMdetail.cxx
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

#include "MLMCrystal.h"
#include "MLMSupportWheel.h"
#include "MLMRadialSupport.h"
#include "MLMonoDetail.h"


namespace xraySystem
{

MLMonoDetail::MLMonoDetail(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  xstalA(new MLMCrystal(keyName+"XstalA")),
  xstalB(new MLMCrystal(keyName+"XstalB")),
  aRadial(new MLMRadialSupport(keyName+"Radial")),
  bWheel(new MLMSupportWheel(keyName+"BWheel"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(xstalA);
  OR.addObject(xstalB);
  OR.addObject(bWheel);
}

MLMonoDetail::~MLMonoDetail()
  /*!
    Destructor
   */
{}

void
MLMonoDetail::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MLMonoDetail","populate");

  FixedRotate::populate(Control);

  return;
}


void
MLMonoDetail::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("MLMonoDetail","createSurfaces");

  
  return; 
}

void
MLMonoDetail::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("MLMonoDetail","createObjects");

  xstalA->addInsertCell(getInsertCells());
  xstalB->addInsertCell(getInsertCells());

  xstalA->createAll(System,*this,0);
  xstalB->createAll(System,*this,0);

  aRadial->addInsertCell(getInsertCells());
  aRadial->createAll(System,*xstalA,"BasePt");

  bWheel->addInsertCell(getInsertCells());
  bWheel->createAll(System,*xstalB,"BaseRotPt");

  return; 
}

void
MLMonoDetail::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MLMonoDetail","createLinks");

  return;
}

void
MLMonoDetail::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("MLMonoDetail","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();

  return;
}

}  // NAMESPACE xraySystem
