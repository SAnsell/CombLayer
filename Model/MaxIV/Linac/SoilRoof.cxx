/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/SoilRoof.cxx
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
#include "Exception.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "HeadRule.h"
#include "BaseModVisit.h"
#include "surfDivide.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "Importance.h"
#include "Object.h"
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
#include "SurfMap.h"
#include "ExternalCut.h"

#include "SoilRoof.h"

namespace tdcSystem
{

SoilRoof::SoilRoof(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

void
SoilRoof::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SoilRoof","populate");

  FixedRotate::populate(Control);

  height=Control.EvalVar<double>(keyName+"Height");
  frontLength=Control.EvalDefVar<double>(keyName+"FrontLength",-1.0);
  ringRadius=Control.EvalVar<double>(keyName+"RingRadius");
  ringCentre=Control.EvalVar<Geometry::Vec3D>(keyName+"RingCentre");

  unitGap=Control.EvalDefVar<double>(keyName+"UnitGap",1.0);

  soilMat=ModelSupport::EvalMat<int>(Control,keyName+"SoilMat");
  soilNLayers=Control.EvalDefVar<size_t>(keyName+"NLayers", 1);

  return;
}

void
SoilRoof::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    This creates an shifted origin -- the normal origin is calculated
    BUT then the origin is shifted up so it impacts the roof surface.
    \param FC :: Fixed Component
    \param sideIndex :: link point 
   */
{
  ELog::RegMethod RegA("SoilRoof","createUnitVector");
  FixedRotate::createUnitVector(FC,sideIndex);
  Origin=ExternalCut::getRule("Roof").trackPoint(Origin,Z);
  return;
}

void
SoilRoof::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SoilRoof","createSurfaces");

  // we have Pre-calculated the origin at the roof so this works:  
  SurfMap::makePlane("SoilTop",SMap,buildIndex+6,Origin+Z*height,Z);
  SurfMap::makePlane("Top",SMap,buildIndex+16,
		     Origin+Z*(height+unitGap),Z);

  ExternalCut::makeShiftedSurf(SMap,"Roof",buildIndex+5,Z,unitGap);
  SurfMap::setSurf("RoofThin",SMap.realSurf(buildIndex+5));

  SurfMap::makePlane("Extention",SMap,buildIndex+2,
		     Origin+Y*(frontLength),Y);

  // Note Ring Centre is ABSOLUTLE (relative to TDC)
  SurfMap::makePlane("RingDivide",SMap,buildIndex+100,ringCentre,Y);
  SurfMap::makeCylinder("RingCyl",SMap,buildIndex+7,ringCentre,Z,ringRadius);

  return;
}

void
SoilRoof::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SoilRoof","createObjects");

  HeadRule HR;
  // these must exist:
  const HeadRule roofHR=ExternalCut::getRule("Roof");
  const HeadRule frontHR=ExternalCut::getRule("Front");
  const HeadRule backHR=ExternalCut::getRule("Back");
  const HeadRule leftHR=ExternalCut::getRule("Left");
  const HeadRule rightHR=ExternalCut::getRule("Right");
  const HeadRule boxHR(leftHR*rightHR*roofHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 7 -5 -2");
  makeCell("BermThin",System,cellIndex++,soilMat,0.0,HR*backHR*boxHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 7 5 -6 -2");
  makeCell("Berm",System,cellIndex++,soilMat,0.0,HR*backHR*rightHR*leftHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -6");
  makeCell("BackVoid",System,cellIndex++,0,0.0,HR*boxHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -6");
  makeCell("FrontVoid",System,cellIndex++,0,0.0,HR*boxHR*frontHR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"6 -16");
  makeCell("FullVoid",System,cellIndex++,0,0.0,
	   HR*frontHR*backHR*rightHR*leftHR);

  HR=HeadRule(SMap,buildIndex,-16);
  addOuterSurf(HR*frontHR*backHR*boxHR);

  layerProcess(System,"Berm",
	       SMap.realSurf(buildIndex+5),
	       -SMap.realSurf(buildIndex+6),
	       soilNLayers);
  return;
}


void
SoilRoof::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("SoilRoof","createLinks");

  FixedComp::setConnect(0,Origin+Z*height,Z);
  FixedComp::setNamedLinkSurf(0, "SoilTop", SurfMap::getSignedSurf("SoilTop"));

  return;
}

void
SoilRoof::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("SoilRoof","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}

void
SoilRoof::layerProcess(Simulation& System,
		       const std::string& cellName,
		       const int primSurf,
		       const int sndSurf,
		       const size_t NLayers)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
    \param cellName :: cell name
    \param primSurf :: primary surface
    \param sndSurf  :: secondary surface
    \param NLayers :: number of layers to divide to
  */
{
    ELog::RegMethod RegA("InjectionHall","layerProcess");

    if (NLayers<=1) return;

    // cellmap -> material
    const int wallCell=this->getCell(cellName);
    const MonteCarlo::Object* wallObj=System.findObject(wallCell);
    if (!wallObj)
      throw ColErr::InContainerError<int>
	(wallCell,"Cell '" + cellName + "' not found");

    const int mat=wallObj->getMatID();
    double baseFrac = 1.0/static_cast<double>(NLayers);
    ModelSupport::surfDivide DA;
    for(size_t i=1;i<NLayers;i++)
      {
	DA.addFrac(baseFrac);
	DA.addMaterial(mat);
	baseFrac += 1.0/static_cast<double>(NLayers);
      }
    DA.addMaterial(mat);

    DA.setCellN(wallCell);
    // CARE here :: buildIndex + X should be so that X+NLayer does not
    // interfer.
    DA.setOutNum(cellIndex, buildIndex+8000);

    ModelSupport::mergeTemplate<Geometry::Plane,
				Geometry::Plane> surroundRule;

    surroundRule.setSurfPair(primSurf,sndSurf);

    surroundRule.setInnerRule(primSurf);
    surroundRule.setOuterRule(sndSurf);

    DA.addRule(&surroundRule);
    DA.activeDivideTemplate(System,this);

    cellIndex=DA.getCellNum();

    return;
}


}  // tdcSystem
