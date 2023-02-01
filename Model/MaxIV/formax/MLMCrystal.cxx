/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formax/MLMCrystal.cxx
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
#include "ContainedComp.h"
#include "MLMCrystal.h"


namespace xraySystem
{

MLMCrystal::MLMCrystal(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,8),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}



MLMCrystal::~MLMCrystal()
  /*!
    Destructor
   */
{}

void
MLMCrystal::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MLMCrystal","populate");

  FixedRotate::populate(Control);
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");


  topSlotXStep=Control.EvalVar<double>(keyName+"TopSlotXStep");
  topSlotLength=Control.EvalVar<double>(keyName+"TopSlotLength");
  topSlotWidth=Control.EvalVar<double>(keyName+"TopSlotWidth");
  topSlotDepth=Control.EvalVar<double>(keyName+"TopSlotDepth");

  mirrorMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");

  return;
}


void
MLMCrystal::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("MLMCrystal","createSurfaces");

  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width,X);    
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // top cut
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(topSlotLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(topSlotLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin+X*(topSlotXStep-topSlotWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(topSlotXStep+topSlotWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin+Z*(height/2.0-topSlotDepth),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+17,
     Origin+X*topSlotXStep-Y*(topSlotLength/2.0),Z,topSlotWidth/2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+18,
     Origin+X*topSlotXStep+Y*(topSlotLength/2.0),Z,topSlotWidth/2.0);

  
  return; 
}

void
MLMCrystal::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("MLMCrystal","createObjects");

  HeadRule HR;

  // slot:
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(11:-17) (-12:-18) 13 -14 15 -6");  
  makeCell("TopSlot",System,cellIndex++,0,0.0,HR);

  // xstal
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(-11 17):(12 18):-13:14:-15");  
  HR*=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");  
  makeCell("Xstal",System,cellIndex++,mirrorMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");  
  addOuterSurf(HR);

  return; 
}

void
MLMCrystal::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MLMCrystal","createLinks");
  
  return;
}

void
MLMCrystal::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("MLMCrystal","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
