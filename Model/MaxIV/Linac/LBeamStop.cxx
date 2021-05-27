/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/LBeamStop.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#include "SurfMap.h"
#include "surfDBase.h"
#include "surfDivide.h"
#include "mergeTemplate.h"
#include "ExternalCut.h"
#include "Exception.h"
#include "BaseModVisit.h"
#include "Importance.h"
#include "Object.h"

#include "LBeamStop.h"

namespace tdcSystem
{

LBeamStop::LBeamStop(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,7),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()

 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

LBeamStop::~LBeamStop()
  /*!
    Destructor
  */
{}

void
LBeamStop::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("LBeamStop","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");

  innerVoidLen=Control.EvalVar<double>(keyName+"InnerVoidLen");
  innerLength=Control.EvalVar<double>(keyName+"InnerLength");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");

  midVoidLen=Control.EvalVar<double>(keyName+"MidVoidLen");
  midLength=Control.EvalVar<double>(keyName+"MidLength");
  midRadius=Control.EvalVar<double>(keyName+"MidRadius");
  midNLayers=Control.EvalVar<size_t>(keyName+"MidNLayers");

  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerNLayers=Control.EvalVar<size_t>(keyName+"OuterNLayers");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  midMat=ModelSupport::EvalMat<int>(Control,keyName+"MidMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");

  return;
}

void
LBeamStop::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("LBeamStop","createSurfaces");

  // pipe and flanges
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,midRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,outerRadius);

  // full outer
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*length,Y);


  // mid
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*midVoidLen,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+12,Origin+Y*(midVoidLen+midLength),Y);

  // inner
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*innerVoidLen,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+2,Origin+Y*(innerVoidLen+innerLength),Y);

  return;
}

void
LBeamStop::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("LBeamStop","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -1 -7 ");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -7 ");
  makeCell("CarbonInner",System,cellIndex++,innerMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -11 -17 7");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -2 7 -17 ");
  makeCell("MidSide",System,cellIndex++,midMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 -17 ");
  makeCell("MidFront",System,cellIndex++,midMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -12 17 -27 ");
  makeCell("OuterSide",System,cellIndex++,outerMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -22 -27 ");
  makeCell("OuterFront",System,cellIndex++,outerMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -22 -27 ");
  addOuterSurf(Out);

  layerProcess(System,"MidFront",
	       SMap.realSurf(buildIndex+2),
	       -SMap.realSurf(buildIndex+12),
	       midNLayers);

  layerProcess(System,"OuterFront",
	       SMap.realSurf(buildIndex+12),
	       -SMap.realSurf(buildIndex+22),
	       outerNLayers);

  return;
}

void
LBeamStop::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("LBeamStop","createLinks");

  FixedComp::setConnect(0,Origin,Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+21));
  FixedComp::nameSideIndex(0,"front");

  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+22));
  FixedComp::nameSideIndex(1,"back");

  // inner
  FixedComp::setConnect(2,Origin+Y*innerRadius,Y);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::nameSideIndex(2,"InnerSide");

  FixedComp::setConnect(3,Origin+Y*(innerVoidLen+innerLength),Y);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+2));
  FixedComp::nameSideIndex(3,"InnerBack");

  // mid
  FixedComp::setConnect(4,Origin+Y*midRadius,Y);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+17));
  FixedComp::nameSideIndex(4,"MidSide");

  FixedComp::setConnect(5,Origin+Y*(midVoidLen+midLength),Y);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+12));
  FixedComp::nameSideIndex(5,"MidBack");

  // outer
  FixedComp::setConnect(6,Origin+Y*outerRadius,Y);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+27));
  FixedComp::nameSideIndex(6,"OuterSide");

  // OuterBack is the same as "back"

  return;
}

void
LBeamStop::layerProcess(Simulation& System,
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

void
LBeamStop::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("LBeamStop","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
