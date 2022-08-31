/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xrayTarget/xrayTarget.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "xrayTarget.h"

namespace xrayHutSystem
{

xrayTarget::xrayTarget(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

xrayTarget::~xrayTarget()
  /*!
    Destructor
  */
{}
  

void
xrayTarget::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("xrayTarget","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  sliceAngle=Control.EvalDefVar<double>(keyName+"SliceAngle",45.0);
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  return;
}


void
xrayTarget::createSurfaces()
  /*!
    Create planes for system
  */
{
  ELog::RegMethod RegA("xrayTarget","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);  

  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+11,Origin-Y*(length/2.0),Y,Z,sliceAngle);
  return; 
}

void
xrayTarget::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("xrayTarget","createObjects");

  HeadRule HR;

  // Inner 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -2 3 -4 5 -6");
  makeCell("Inner",System,cellIndex++,mat,0.0,HR);
  
  addOuterSurf(HR);
  return; 
}

void
xrayTarget::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("xrayTarget","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  
  FixedComp::setConnect(0,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+2));


  return;
}

  
void
xrayTarget::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point
    \param sideIndex :: sideIndex for link point
   */
{
  ELog::RegMethod RegA("xrayTarget","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xrayHutSystem
