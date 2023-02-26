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

  driveZClear=Control.EvalVar<double>(keyName+"DriveZClear");
  driveThick=Control.EvalVar<double>(keyName+"DriveThick");
  driveWidth=Control.EvalVar<double>(keyName+"DriveWidth");
  driveLength=Control.EvalVar<double>(keyName+"DriveLength");
  driveTopLen=Control.EvalVar<double>(keyName+"DriveTopLen");
  driveBaseThick=Control.EvalVar<double>(keyName+"DriveBaseThick");
  driveCutWidth=Control.EvalVar<double>(keyName+"DriveCutWidth");
  driveCutLenth=Control.EvalVar<double>(keyName+"DriveCutLenth");
  driveCutRadius=Control.EvalVar<double>(keyName+"DriveCutRadius");
  driveCutRadLen=Control.EvalVar<double>(keyName+"DriveCutRadLen");

  baseYStep=Control.EvalVar<double>(keyName+"BaseYStep");
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  baseWidth=Control.EvalVar<double>(keyName+"BaseWidth");
  baseLength=Control.EvalVar<double>(keyName+"BaseLength");
  baseCutDepth=Control.EvalVar<double>(keyName+"BaseCutDepth");
  baseCutFLen=Control.EvalVar<double>(keyName+"BaseCutFLen");
  baseCutBLen=Control.EvalVar<double>(keyName+"BaseCutBLen");
  baseCutWidth=Control.EvalVar<double>(keyName+"BaseCutWidth");
  baseMidSlotWidth=Control.EvalVar<double>(keyName+"BaseMidSlotWidth");
  baseMidSlotHeight=Control.EvalVar<double>(keyName+"BaseMidSlotHeight");
  baseSideSlotEdge=Control.EvalVar<double>(keyName+"BaseSideSlotEdge");
  baseSideSlotHeight=Control.EvalVar<double>(keyName+"BaseSideSlotHeight");
  baseSideSlotWidth=Control.EvalVar<double>(keyName+"BaseSideSlotWidth");
  
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

  // Drive support
  const Geometry::Vec3D dOrg=Origin-Z*(thick+driveZClear);

  ModelSupport::buildPlane(SMap,buildIndex+501,dOrg-Y*(driveLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+502,dOrg+Y*(driveLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+503,dOrg-X*(driveWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+504,dOrg+X*(driveWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+505,dOrg-Z*(driveThick/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+506,dOrg,Z);


  ModelSupport::buildPlane(SMap,buildIndex+511,dOrg-Y*(driveTopLen/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+512,dOrg+Y*(driveTopLen/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+515,dOrg-Z*(driveThick/2.0-driveBaseThick),Z);

  // drive cutout:
  
  
  // cutout
  const Geometry::Vec3D bOrg(dOrg-Z*driveThick);
  
  ModelSupport::buildPlane(SMap,buildIndex+1501,bOrg-Y*(baseLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1502,bOrg+Y*(baseLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1503,bOrg-X*(baseWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1504,bOrg+X*(baseWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1505,bOrg-Z*(baseThick/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+1506,bOrg+Z*baseCutDepth,Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+1511,bOrg-Y*(baseLength/2.0-baseCutFLen),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+1512,bOrg+Y*(baseLength/2.0-baseCutBLen),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+1513,bOrg-X*(baseCutWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+1514,bOrg+X*(baseCutWidth/2.0),X);
  
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

  // Drive plate:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"501 -502 503 -504 5 506");
  makeCell("DriveVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"501 -502 503 -504 -505 506");
  makeCell("DrivePlate",System,cellIndex++,plateMat,0.0,HR);
  
  // VOID VOLUMES : OUTER
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5");  
  addOuterSurf(HR*topHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"501 -502 503 -504 -5 505");  
  addOuterUnionSurf(HR);
  
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

