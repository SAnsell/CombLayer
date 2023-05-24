/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1Connectors.cxx
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"

#include "M1Connectors.h"

#include "BaseModVisit.h"
#include "BaseVisit.h"
#include "Surface.h"


namespace xraySystem
{

M1Connectors::M1Connectors(const std::string& Key) :
  attachSystem::FixedRotate(Key,18),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  nameSideIndex({
      {"slotTop",7},{"slotBase",8},{"slotAMid",9},{"slotBMid",10}
    });
}


M1Connectors::~M1Connectors()
  /*!
    Destructor
   */
{}

void
M1Connectors::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1Connectors","populate");

  FixedRotate::populate(Control);

  clipYStep=Control.EvalVar<double>(keyName+"ClipYStep");
  clipLen=Control.EvalVar<double>(keyName+"ClipLen");
  clipSiThick=Control.EvalVar<double>(keyName+"ClipSiThick");
  clipAlThick=Control.EvalVar<double>(keyName+"ClipAlThick");
  clipExtent=Control.EvalVar<double>(keyName+"ClipExtent");

  clipMat=ModelSupport::EvalMat<int>(Control,keyName+"ClipMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  
  return;
}


void
M1Connectors::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1Connectors","createSurfaces");

  makeShiftedSurf(SMap,"slotBase",buildIndex+3,X,clipSiThick);
  // extenstion in clip direction
  ELog::EM<<"Slit Extent "<<clipExtent<<ELog::endDiag;
  makeShiftedSurf(SMap,"slotBase",buildIndex+503,X,-clipExtent);

  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(clipYStep+clipLen/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,Origin-Y*(clipYStep-clipLen/2.0),Y);

  ModelSupport::buildPlane
    (SMap,buildIndex+201,Origin+Y*(clipYStep-clipLen/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+202,Origin+Y*(clipYStep+clipLen/2.0),Y);

  int BI(buildIndex+500);
  for(const Geometry::Vec3D& Pt : standoffPos)
    ModelSupport::buildCylinder
      (SMap,BI+7,Origin+Y*(clipYStep+clipLen/2.0),Y);
    

  return;
}

void
M1Connectors::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1Connectors","createObjects");

  const HeadRule baseHR=getRule("slotBase"); // 13
  const HeadRule midAHR=getRule("slotAMid"); // -15
  const HeadRule midBHR=getRule("slotBMid"); // 16
  const HeadRule mBaseHR=getRule("MBase");   // -5
  const HeadRule mTopHR=getRule("MTop");     // 6 ( 256001)

  const HeadRule cInnerBase=getRule("CInnerBase"); // +5 (from CClamp)
  const HeadRule cInnerTop=getRule("CInnerTop");   // -6 (from CClamp)
  
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -3");
  makeCell("Clip",System,cellIndex++,clipMat,0.0,
	   HR*baseHR*mBaseHR.complement()*midAHR);
  insertComponent(System,"slotA",HR.complement());
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -202 -3");
  makeCell("Clip",System,cellIndex++,clipMat,0.0,
	   HR*baseHR*mBaseHR.complement()*midAHR);

  insertComponent(System,"slotA",HR.complement());

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -3");
  makeCell("Clip",System,cellIndex++,clipMat,0.0,
	   HR*baseHR*mTopHR.complement()*midBHR);
  insertComponent(System,"slotB",HR.complement());

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -202 -3");
  makeCell("Clip",System,cellIndex++,clipMat,0.0,
	   HR*baseHR*mTopHR.complement()*midBHR);
  insertComponent(System,"slotB",HR.complement());

  // ohter part of clip
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -3 503");
  makeCell("Clip",System,cellIndex++,clipMat,0.0,
	   HR*mBaseHR*cInnerBase);
  insertComponent(System,"gapA",HR.complement());

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -202 -3 503");
  makeCell("Clip",System,cellIndex++,clipMat,0.0,
	   HR*mBaseHR*cInnerBase);
  insertComponent(System,"gapA",HR.complement());

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -3 503");
  makeCell("Clip",System,cellIndex++,clipMat,0.0,
	   HR*mTopHR*cInnerTop);
  insertComponent(System,"gapB",HR.complement());

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -202 -3 503");
  makeCell("Clip",System,cellIndex++,clipMat,0.0,
	   HR*mTopHR*cInnerTop);
  insertComponent(System,"gapA",HR.complement());

  // Create Buttons:
  
  

  
  return;
}

void
M1Connectors::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1Connectors","createLinks");

  // link points are defined in the end of createSurfaces

  
  return;
}

void
M1Connectors::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1Connectors","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
