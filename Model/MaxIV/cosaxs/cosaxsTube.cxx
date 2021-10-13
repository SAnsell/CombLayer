/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxs/cosaxsTube.cxx
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
#include "objectRegister.h"
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "BlockZone.h"
#include "FrontBackCut.h"
#include "generalConstruct.h"
#include "ContainedGroup.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "portItem.h"
#include "portSet.h"
#include "FlangeDome.h"

#include "GateValveCylinder.h"
#include "cosaxsTubeNoseCone.h"
#include "cosaxsTubeStartPlate.h"
#include "MonoBeamStop.h"
#include "cosaxsTubeWAXSDetector.h"
#include "cosaxsTubeAirBox.h"
#include "cosaxsTubeCable.h"


#include "cosaxsTube.h"

namespace xraySystem
{

cosaxsTube::cosaxsTube(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),

  outerMat(0),
  delayPortFlag(0),

  buildZone(Key+"BlockZone"),
  tubeZone(Key+"TubeZone"),

  noseCone(new xraySystem::cosaxsTubeNoseCone(keyName+"NoseCone")),
  gateA(new constructSystem::GateValveCylinder(keyName+"GateA")),
  startPlate(new xraySystem::cosaxsTubeStartPlate(keyName+"StartPlate")),
  backPlate(new constructSystem::FlangeDome(keyName+"BackDome")),
  
  beamDump(new xraySystem::MonoBeamStop(keyName+"BeamDump")),
  waxs(new xraySystem::cosaxsTubeWAXSDetector(keyName+"WAXS")),
  airBox(new xraySystem::cosaxsTubeAirBox(keyName+"AirBox")),
  cable(new xraySystem::cosaxsTubeCable(keyName+"Cable"))
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(noseCone);
  OR.addObject(gateA);
  OR.addObject(startPlate);

  for(size_t i=0;i<seg.size();i++)
    {
      seg[i] = std::make_shared<constructSystem::PipeTube>
	(keyName+"Segment"+std::to_string(i+1));
      OR.addObject(seg[i]);
    }

  OR.addObject(beamDump);
  OR.addObject(waxs);
  OR.addObject(airBox);
  OR.addObject(cable);
}

cosaxsTube::~cosaxsTube()
  /*!
    Destructor
  */
{}

void
cosaxsTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("cosaxsTube","populate");

  FixedRotate::populate(Control);

  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerLength=Control.EvalVar<double>(keyName+"OuterLength");
  outerMat=ModelSupport::EvalDefMat(Control,keyName+"OuterMat",outerMat);
  return;
}

void
cosaxsTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("cosaxsTube","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(outerLength),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);


  const HeadRule HR=
    ModelSupport::getHeadRule(SMap,buildIndex," -7 ");

  buildZone.setSurround(HR);
  buildZone.setFront(getRule("front"));
  buildZone.setInnerMat(outerMat);

  return;
}

void
cosaxsTube::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("cosaxsTube","createObjects");


  int outerCell;

  buildZone.addInsertCells(this->getInsertCells());
  
  noseCone->createAll(System, *this, 0);
  outerCell=buildZone.createUnit(System,*noseCone,2);
  noseCone->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*noseCone,"back",*gateA);
    
  constructSystem::constructUnit
    (System,buildZone,*gateA,"back",*startPlate);

  constructSystem::constructUnit
    (System,buildZone,*startPlate,"back",*seg[0]);
  seg[0]->deleteCell(System,"Void");

  for (size_t i=1; i<seg.size(); i++)
    {
      constructSystem::constructUnit
	(System,buildZone,*seg[i-1],"back",*seg[i]);
      seg[i]->deleteCell(System,"Void");
    }

  backPlate->setCutSurf("plate",*seg[7],"back");
  backPlate->createAll(System,*seg[7],"back");
  const constructSystem::portItem& BPI=backPlate->getPort(1);
  outerCell=buildZone.createUnit(System,BPI,"OuterPlate");
  backPlate->insertInCell(System,outerCell);
  

  std::vector<int> cellVec;
  cellVec=seg[5]->splitObject
    (System,3001,buildZone.getCell("Unit",7),
     Geometry::Vec3D(0,0,0),Geometry::Vec3D(-1,0,0.5));

  cellVec=seg[5]->splitObject
    (System,3002,buildZone.getCell("Unit",7),
     Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0.5));

  
  // cellVec=seg[5]->splitObject
  //   (System,3003,buildZone.getCell("Unit",7),
  //    Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0.5));
  // //  this->addCell("OuterVoid",CellVec.back());
  
  cellIndex+=2;
  

  return;
}

void
cosaxsTube::createInnerObjects(Simulation& System)
  /*!
    Build the inner objects
  */
{
  ELog::RegMethod RegA("cosaxsTube","createInnerObjects");

  int outerCell;
  // No Insert need because it completely replaces the main cells
  tubeZone.setSurround(seg.back()->getFullRule("InnerSide"));
  tubeZone.setFront(seg[0]->getFullRule("InnerFront"));
  tubeZone.setMaxExtent(seg.back()->getFullRule("#back"));
  beamDump->createAll(System,*seg[0],"#front");
  outerCell=tubeZone.createUnit(System,*beamDump,2);
  beamDump->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,tubeZone,*beamDump,"back",*waxs);

  constructSystem::constructUnit
    (System,tubeZone,*waxs,"back",*airBox);

  constructSystem::constructUnit
    (System,tubeZone,*airBox,"back",*cable);

  tubeZone.createUnit(System);
  return;
}
  
void
cosaxsTube::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("cosaxsTube","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  setCells("tubeVoid",buildZone.getCells("Unit"));
  return;
}

void
cosaxsTube::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("cosaxsTube","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createInnerObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
