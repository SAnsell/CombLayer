/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formax/MLMWheelPlate.cxx
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "MLMWheelPlate.h"


namespace xraySystem
{

MLMWheelPlate::MLMWheelPlate(const std::string& Key) :
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

MLMWheelPlate::~MLMWheelPlate()
  /*!
    Destructor
   */
{}

void
MLMWheelPlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MLMWheelPlate","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  thick=Control.EvalVar<double>(keyName+"Thick");
  ridgeThick=Control.EvalVar<double>(keyName+"RidgeThick");
  ridgeLen=Control.EvalVar<double>(keyName+"RidgeLen");

  outSlotWidth=Control.EvalVar<double>(keyName+"OutSlotWidth");
  outSlotLength=Control.EvalVar<double>(keyName+"OutSlotLength");

  midSlotXStep=Control.EvalVar<double>(keyName+"MidSlotXStep");
  midSlotLength=Control.EvalVar<double>(keyName+"MidSlotLength");
  midSlotWidth=Control.EvalVar<double>(keyName+"MidSlotWidth");

  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  
  return;
}


void
MLMWheelPlate::createSurfaces()
  /*!
    Create planes for mirror block and support
    +x AWAY from beam
  */
{
  ELog::RegMethod RegA("MLMWheelPlate","createSurfaces");

  // main block 
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*thick,Z);

  if (!ExternalCut::isActive("TopSurf"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*thick,Z);
      setCutSurf("TopSurf",-SMap.realSurf(buildIndex+6));
    }

  // ridges
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(length/2.0-ridgeLen),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,Origin+Y*(length/2.0-ridgeLen),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+105,Origin-Z*(thick-ridgeThick),Z);

  // slot:
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin-Y*(outSlotLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*(outSlotLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+203,Origin+X*(width/2.0-outSlotWidth),X);

  // mid gap
  ELog::EM<<"Mid == "<<midSlotLength<<" "<<midSlotWidth<<ELog::endDiag;
  ELog::EM<<"Mid == "<<midSlotXStep<<ELog::endDiag;

  ModelSupport::buildPlane(SMap,buildIndex+301,Origin-Y*(midSlotLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+302,Origin+Y*(midSlotLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+303,Origin+X*(midSlotXStep-midSlotWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+304,Origin+X*(midSlotXStep+midSlotWidth/2.0),X);
  
  
  return; 
}

void
MLMWheelPlate::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("MLMWheelPlate","createObjects");

  const HeadRule topHR=getRule("TopSurf");
  HeadRule HR;

  // Top plate
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 3 -4 105 (-201:202:-203) (-301:302:-303:304)");
  makeCell("TopPlate",System,cellIndex++,plateMat,0.0,HR*topHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -101 3 -4 -105 5");
  makeCell("Ridge",System,cellIndex++,plateMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 102 3 -4 -105 5");
  makeCell("Ridge",System,cellIndex++,plateMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 3 -4 -105 5");
  makeCell("RidgeVoid",System,cellIndex++,voidMat,0.0,HR);

  // slot
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -202 203 -4 105");
  makeCell("SlotVoid",System,cellIndex++,voidMat,0.0,HR*topHR);

  // mid gap
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -302 303 -304 105");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,HR*topHR);

  

  // VOID VOLUMES : OUTER
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5");  
  addOuterSurf(HR*topHR);
  
  return; 
}

void
MLMWheelPlate::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MLMWheelPlate","createLinks");

  HeadRule HR;

  return;
}

void
MLMWheelPlate::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
/*!
    Build the Radial supporting plate  everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("MLMWheelPlate","createAll");
  populate(System.getDataBase());
  
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem

