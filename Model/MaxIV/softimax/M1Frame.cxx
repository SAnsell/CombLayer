/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1Frame.cxx
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PointMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "M1Frame.h"

namespace xraySystem
{

M1Frame::M1Frame(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

M1Frame::~M1Frame()
  /*!
    Destructor
   */
{}

void
M1Frame::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1Frame","populate");

  FixedRotate::populate(Control);

  bladeOutRad=Control.EvalVar<double>(keyName+"BladeOutRad");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeTopAngle=Control.EvalVar<double>(keyName+"BladeTopAngle");
  bladeBaseAngle=Control.EvalVar<double>(keyName+"BladeBaseAngle");
  bladeBaseWidth=Control.EvalVar<double>(keyName+"BladeBaseWidth");
  bladeBaseHeight=Control.EvalVar<double>(keyName+"BladeBaseHeight");

  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");  

  return;
}

void
M1Frame::createSurfaces()
  /*!
    Create surfaces for the frame
  */
{
  ELog::RegMethod RegA("M1Frame","createSurfaces");

  SurfMap::makeExpandedCylinder("InnerRadius",SMap,buildIndex+7,bladeOutRad);
  SurfMap::makeShiftedPlane("FSurf",SMap,buildIndex+101,Y,bladeThick);
  
  return;
}

void
M1Frame::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1Frame","createObjects");

  const HeadRule flatHR=getSurfRule("FSurf");
  const HeadRule innerHR=getSurfRule("InnerRadius");
  HeadRule HR;
  

  ELog::EM<<"Innert == "<<innerHR<<ELog::endDiag;
  ELog::EM<<"Flat == "<<flatHR<<ELog::endDiag;

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -7");
  makeCell("FRingsupport",System,cellIndex++,supportMat,0.0,HR*flatHR*innerHR);  

  
  return;
}

void
M1Frame::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1Frame","createLinks");

  // link points are defined in the end of createSurfaces

  return;
}

void
M1Frame::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1Frame","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  ELog::EM<<"Origin == "<<Origin <<" :: "<<Y<<ELog::endDiag;
  return;
}

}  // NAMESPACE xraySystem
