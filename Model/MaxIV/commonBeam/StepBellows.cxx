/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/StepBellows.cxx
 *
 * Copyright (c) 2026 by U. Friman-Gayer
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

#include <cmath>
#include <fstream>
#include <iomanip>
#include <list>
#include <vector>
#include <set>
#include <map>
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
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "MaterialSupport.h"
#include "Exception.h"

#include "SmallAngleBellows.h"
#include "StepBellows.h"

namespace xraySystem
{

StepBellows::StepBellows(const std::string& Key):
  /*!
    Constructor
    \param Key :: Key name
  */
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedGroup("FrontBellows","BackBellows"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  frontBellows(new xraySystem::SmallAngleBellows(keyName+"FrontBellows")),
  backBellows(new xraySystem::SmallAngleBellows(keyName+"BackBellows"))
{}

StepBellows::StepBellows(const StepBellows& A) :
  /*!
    Copy constructor
    \param A :: Bellows to copy
  */
  attachSystem::FixedRotate(A),
  attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),step(A.step)
{}

StepBellows&
StepBellows::operator=(const StepBellows& A)
  /*!
    Assignment operator
    \param A :: Bellows to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      step=A.step;
    }
  return *this;
}

StepBellows::~StepBellows()
  /*!
    Destructor
  */
{}

void
StepBellows::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables and check input
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("StepBellows","populate");

  FixedRotate::populate(Control);

  length=Control.EvalDefVar<double>(keyName+"Length",0.0);
  step=Control.EvalDefVar<double>(keyName+"Step",0.0);

  return;
}

void
StepBellows::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("StepBellows","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
}

void
StepBellows::buildComponents(Simulation& System)
  /*!
    Construct the object to be built
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("StepBellows","buildComponents");
 
  frontBellows->addInsertCell(getCC("FrontBellows").getInsertCells());

  if (isActive("front"))
    frontBellows->setFront(this->getRuleStr("front"));
  frontBellows->createAll(System,*this,0);

  backBellows->addInsertCell(getCC("BackBellows").getInsertCells());

  if (isActive("back"))
    backBellows->setBack(this->getRuleStr("back"));  
  backBellows->createAll(System,*frontBellows,"back");


  ContainedGroup::addOuterSurf("FrontBellows",frontBellows->getOuterSurf());
  ContainedGroup::addOuterSurf("BackBellows",backBellows->getOuterSurf());
  
  return;
}

void
StepBellows::createLinks()
/*!
  Create links
 */
{
  ELog::RegMethod RegA("StepBellows","createLinks");

  setLinkCopy(0,*frontBellows,1);
  setLinkCopy(1,*backBellows,2);

  return;
}

void
StepBellows::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int FIndex)
/*!
  Create bellows
 */
{
  ELog::RegMethod RegA("StepBellows","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  buildComponents(System);

  createSurfaces();
  createLinks();

  return;
}

}  // NAMESPACE constructSystem
