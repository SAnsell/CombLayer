/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1Mirror.cxx
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
#include "BaseVisit.h"
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
#include "M1Mirror.h"


namespace xraySystem
{

M1Mirror::M1Mirror(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}


M1Mirror::~M1Mirror()
  /*!
    Destructor
   */
{}

void
M1Mirror::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1Mirror","populate");

  FixedRotate::populate(Control);

  theta=Control.EvalVar<double>(keyName+"Theta");
  phi=Control.EvalVar<double>(keyName+"Phi");
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");

  slotXStep=Control.EvalVar<double>(keyName+"SlotXStep");
  slotWidth=Control.EvalVar<double>(keyName+"SlotWidth");
  slotDepth=Control.EvalVar<double>(keyName+"SlotDepth");

  pipeXStep=Control.EvalVar<double>(keyName+"PipeXStep");
  pipeYStep=Control.EvalVar<double>(keyName+"PipeYStep");
  pipeZStep=Control.EvalVar<double>(keyName+"PipeZStep");
  pipeSideRadius=Control.EvalVar<double>(keyName+"PipeSideRadius");
  pipeWallThick=Control.EvalVar<double>(keyName+"PipeWallThick");
  pipeBaseLen=Control.EvalVar<double>(keyName+"PipeBaseLen");
  pipeBaseRadius=Control.EvalVar<double>(keyName+"PipeBaseRadius");
  pipeOuterLen=Control.EvalVar<double>(keyName+"PipeOuterLen");
  pipeOuterRadius=Control.EvalVar<double>(keyName+"PipeOuterRadius");

  mirrorMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  

  return;
}


void
M1Mirror::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1Mirror","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1000,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);


  // support cuts:
  Geometry::Vec3D slotOrg=Origin-X*slotXStep;
  ModelSupport::buildPlane(SMap,buildIndex+13,slotOrg-X*(slotWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,slotOrg+X*(slotWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+15,slotOrg-Z*(height/2.0-slotDepth),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,slotOrg+Z*(height/2.0-slotDepth),Z);


  // pipes:
  const Geometry::Vec3D pBaseA=Origin-X*pipeXStep-Y*(length/2.0-pipeYStep);
  const Geometry::Vec3D pBaseB=Origin-X*pipeXStep+Y*(length/2.0-pipeYStep);

  const Geometry::Vec3D pMidA=Origin-Z*(height/2.0-pipeZStep)
    -Y*(length/2.0-pipeYStep);
  const Geometry::Vec3D pMidB=Origin-Z*(height/2.0-pipeZStep)
    +Y*(length/2.0-pipeYStep);


  ModelSupport::buildCylinder(SMap,buildIndex+107,pBaseA,Z,pipeBaseRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,pBaseB,Z,pipeBaseRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+117,pBaseA,Z,pipeBaseRadius+pipeWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,pBaseB,Z,pipeBaseRadius+pipeWallThick);

  ModelSupport::buildCylinder
    (SMap,buildIndex+127,pBaseA,Z,pipeOuterRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+227,pBaseB,Z,pipeOuterRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+157,pMidA,X,pipeSideRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+257,pMidB,X,pipeSideRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+167,pMidA,X,pipeSideRadius+pipeWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+267,pMidB,X,pipeSideRadius+pipeWallThick);


  // pipe plane separators
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Z*(height/2.0-pipeBaseLen),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+111,Origin-Z*(height/2.0-pipeOuterLen),Z);

  ModelSupport::buildPlane(SMap,buildIndex+154,pBaseA,X);

  return;
}

void
M1Mirror::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1Mirror","createObjects");

  HeadRule HR;
  // xstal
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
     "1 -1000  3 -4 5 -6 (-13:14:(-16 15)) (101:117) (154:167) (111:127)");
  makeCell("Mirror",System,cellIndex++,mirrorMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
     "1000 -2 3 -4 5 -6 (-13:14:(-16 15)) (101:217) (154:267) (111:227)");
  makeCell("Mirror",System,cellIndex++,mirrorMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -14 16 -6");
  makeCell("Slot",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -14 -15 5");
  makeCell("Slot",System,cellIndex++,voidMat,0.0,HR);

  // pipes
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -101 -107");
  makeCell("pipe",System,cellIndex++,waterMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -101 -117 107 (154:157)");
  makeCell("pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -154 -157 107");
  makeCell("pipe",System,cellIndex++,waterMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -154 -167 157 117");
  makeCell("pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -111 -127 117");
  makeCell("pipe",System,cellIndex++,outerMat,0.0,HR);

  // opposite side
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -101 -207");
  makeCell("pipe",System,cellIndex++,waterMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -101 -217 207 (154:257)");
  makeCell("pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -154 -257 207");
  makeCell("pipe",System,cellIndex++,waterMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -154 -267 257 217");
  makeCell("pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -111 -227 217");
  makeCell("pipe",System,cellIndex++,outerMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(HR);

  return;
}

void
M1Mirror::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1Mirror","createLinks");

  // link points are defined in the end of createSurfaces

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*width,-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  FixedComp::setConnect(6,Origin-X*width,Y);
  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+3));

  nameSideIndex(2,"outSide");
  nameSideIndex(3,"beamSide");
  nameSideIndex(3,"mirrorSide");
  nameSideIndex(4,"base");
  nameSideIndex(5,"top");
  nameSideIndex(6,"backPlateOrg");

  
  return;
}

void
M1Mirror::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1Mirror","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem