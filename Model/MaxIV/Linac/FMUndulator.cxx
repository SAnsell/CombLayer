/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/FMUndulator.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <array>

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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "FMUndulator.h"

namespace xraySystem
{

FMUndulator::FMUndulator(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

FMUndulator::~FMUndulator() 
  /*!
    Destructor
  */
{}

void
FMUndulator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("FMUndulator","populate");
  
  FixedRotate::populate(Control);

  vGap=Control.EvalVar<double>(keyName+"VGap");

  vGap=Control.EvalVar<double>(keyName+"VGap");
  length=Control.EvalVar<double>(keyName+"Length");

  poleWidth=Control.EvalVar<double>(keyName+"PoleWidth");
  poleDepth=Control.EvalVar<double>(keyName+"PoleDepth");

  magnetWidth=Control.EvalVar<double>(keyName+"MagnetWidth");
  magnetDepth=Control.EvalVar<double>(keyName+"MagnetDepth");

  baseDepth=Control.EvalVar<double>(keyName+"BaseDepth");
  baseExtraLen=Control.EvalVar<double>(keyName+"BaseExtraLen");

  midWidth=Control.EvalVar<double>(keyName+"MidWidth");
  midDepth=Control.EvalVar<double>(keyName+"MidDepth");
  mainWidth=Control.EvalVar<double>(keyName+"MainWidth");
  mainDepth=Control.EvalVar<double>(keyName+"MainDepth");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  magnetMat=ModelSupport::EvalMat<int>(Control,keyName+"MagnetMat");
  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");

  return;
}
 
void
FMUndulator::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("FMUndulator","createSurfaces");

  // Build Magnet Pole first
  Geometry::Vec3D lowOrg(Origin-Z*(vGap/2.0));
  Geometry::Vec3D topOrg(Origin+Z*(vGap/2.0));

  ModelSupport::buildPlane(SMap,buildIndex+5,lowOrg,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,topOrg,Z);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  // 45 degree cut  [inward]
  ModelSupport::buildPlane(SMap,buildIndex+103,lowOrg-X*(poleWidth/2.0),X-Z);
  ModelSupport::buildPlane(SMap,buildIndex+104,lowOrg+X*(poleWidth/2.0),-X-Z);

  ModelSupport::buildPlane(SMap,buildIndex+113,topOrg-X*(poleWidth/2.0),X+Z);
  ModelSupport::buildPlane(SMap,buildIndex+114,topOrg+X*(poleWidth/2.0),-X+Z);

  ModelSupport::buildPlane(SMap,buildIndex+105,lowOrg-Z*poleDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,topOrg+Z*poleDepth,Z);

  // magnet
  lowOrg-=Z*poleDepth;
  topOrg+=Z*poleDepth;
  ModelSupport::buildPlane(SMap,buildIndex+203,lowOrg-X*(magnetWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,topOrg+X*(magnetWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+205,lowOrg-Z*magnetDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,topOrg+Z*magnetDepth,Z);


  // close support (base)
  lowOrg-=Z*magnetDepth;
  topOrg+=Z*magnetDepth;

  ModelSupport::buildPlane
    (SMap,buildIndex+301,lowOrg-Y*(length/2.0+baseExtraLen),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+302,topOrg+Y*(length/2.0+baseExtraLen),Y);
  ModelSupport::buildPlane(SMap,buildIndex+305,lowOrg-Z*baseDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+306,topOrg+Z*baseDepth,Z);

  // mid support
  lowOrg-=Z*baseDepth;
  topOrg+=Z*baseDepth;
  ModelSupport::buildPlane(SMap,buildIndex+403,Origin-X*(midWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+404,Origin+X*(midWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+405,lowOrg-Z*midDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+406,topOrg+Z*midDepth,Z);

  // mainPlate
  lowOrg-=Z*midDepth;
  topOrg+=Z*midDepth;
  ModelSupport::buildPlane(SMap,buildIndex+503,Origin-X*(mainWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+504,Origin+X*(mainWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+505,lowOrg-Z*mainDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+506,topOrg+Z*mainDepth,Z);
    
  return;
}

void
FMUndulator::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("FMUndulator","createObjects");

  std::string Out;

  // Main inner void
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 503 -504 5 -6");
  makeCell("Void",System,cellIndex++,0,0.0,Out);
  
  // pole plate
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 103 104 -5 105");
  makeCell("lowPole",System,cellIndex++,magnetMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -103 503 -5 105");
  makeCell("lowPoleLVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -104 -504 -5 105");
  makeCell("lowPoleRVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 113 114 6 -106");
  makeCell("topPole",System,cellIndex++,magnetMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -113 503 6 -106");
  makeCell("topPoleLVoid",System,cellIndex++,voidMat,0.0,Out);
								
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -114 -504 6 -106");
  makeCell("topPoleRVoid",System,cellIndex++,voidMat,0.0,Out);

  // magnet
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 203 -204 -105 205");
  makeCell("lowMag",System,cellIndex++,magnetMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 203 -204 106 -206");
  makeCell("topMag",System,cellIndex++,magnetMat,0.0,Out);

  // Base support [EXTEND]
  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -302 203 -204 -205 305");
  makeCell("lowBasePlate",System,cellIndex++,supportMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -302 203 -204 206 -306");
  makeCell("topBasePlate",System,cellIndex++,supportMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 503 -203 -105 305");
  makeCell("lowBaseV",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -504 204 -105 305");
  makeCell("lowBaseV",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 503 -203 106 -306");
  makeCell("topBaseV",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -504 204 106 -306");
  makeCell("topBaseV",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -1 204 -504 -205 305");
  makeCell("topBaseVX",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -1 -203 503 -205 305");
  makeCell("topBaseVX",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -1 204 -504 206 -306");
  makeCell("lowBaseVX",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -1 -203 503 206 -306");
  makeCell("lowBaseVX",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-302 2 204 -504 -205 305");
  makeCell("topBaseVY",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-302 2 -203 503 -205 305");
  makeCell("topBaseVY",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-302 2 204 -504 206 -306");
  makeCell("lowBaseVY",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-302 2 -203 503 206 -306");
  makeCell("lowBaseVY",System,cellIndex++,voidMat,0.0,Out);

  
  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -1 503 -504  205 -206");
  makeCell("frontV",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"2 -302 503 -504  205 -206");
  makeCell("backV",System,cellIndex++,voidMat,0.0,Out);

  // Mid support [EXTEND]

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -302 403 -404 -305 405");
  makeCell("lowMid",System,cellIndex++,supportMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -302 -403 503 -305 405");
  makeCell("lowMidV",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -302 404 -504 -305 405");
  makeCell("lowMidV",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -302 403 -404 306 -406");
  makeCell("topMid",System,cellIndex++,supportMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -302 503 -403 306 -406");
  makeCell("topMidV",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -302 -504 404 306 -406");
  makeCell("topMidV",System,cellIndex++,voidMat,0.0,Out);


  //  Main [EXTEND]
  

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -302 503 -504 -405 505");
  makeCell("lowMain",System,cellIndex++,supportMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"301 -302 503 -504 406 -506");
  makeCell("topMain",System,cellIndex++,supportMat,0.0,Out);

  // EXTERNAL:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 301 -302 503 -504 505 -506 ");
  addOuterSurf(Out);      

  return;
}

void
FMUndulator::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("FMUndulator","createLinks");
  
  setConnect(0,Origin-Y*(length/2.0),-Y);
  setConnect(1,Origin+Y*(length/2.0),Y);
  
  setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  setLinkSurf(1,SMap.realSurf(buildIndex+2));

  setLinkSurf(4,-SMap.realSurf(buildIndex+505));
  setLinkSurf(5,SMap.realSurf(buildIndex+506));
  FixedComp::nameSideIndex(4,"base");
  FixedComp::nameSideIndex(5,"top");
  return;
}

void
FMUndulator::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("FMUndulator","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
