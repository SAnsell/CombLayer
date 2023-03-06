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

  // main xstal CENTRE AT ORIGIN
  const Geometry::Quaternion QXA
    (Geometry::Quaternion::calcQRotDeg(-theta,Z));

  Geometry::Vec3D PX(X);
  Geometry::Vec3D PY(Y);
  Geometry::Vec3D PZ(Z);

  QXA.rotate(PX);
  QXA.rotate(PY);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-PY*(length/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+PY*(length/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-PX*width,PX);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin,PX);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-PZ*(height/2.0),PZ);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+PZ*(height/2.0),PZ);


  // support cuts:
  Geometry::Vec3D slotOrg=Origin-X*slotXStep;
  ModelSupport::buildPlane(SMap,buildIndex+13,slotOrg-PX*(slotWidth/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+14,slotOrg+PX*(slotWidth/2.0),PX);
  ModelSupport::buildPlane
    (SMap,buildIndex+15,slotOrg-PZ*(height/2.0-slotDepth),PZ);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,slotOrg+PZ*(height/2.0-slotDepth),PZ);

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
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 3 -4 5 -6");
  makeCell("M1Mirror",System,cellIndex++,mirrorMat,0.0,HR);


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
