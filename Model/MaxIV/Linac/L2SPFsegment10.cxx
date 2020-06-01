/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/L2SPFsegment10.cxx
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "BlankTube.h"
#include "Bellows.h"
#include "LQuadF.h"
#include "BPM.h"
#include "CorrectorMag.h"
#include "CylGateValve.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "L2SPFsegment10.h"

namespace tdcSystem
{

// Note currently uncopied:

  
L2SPFsegment10::L2SPFsegment10(const std::string& Key) :
  TDCsegment(Key,2),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  gateValve(new xraySystem::CylGateValve(keyName+"GateValve")),
  pumpA(new constructSystem::BlankTube(keyName+"PumpA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),

  QuadA(new tdcSystem::LQuadF(keyName+"QuadA")),
  cMagVertA(new tdcSystem::CorrectorMag(keyName+"CMagVertA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(pipeA);
  OR.addObject(gateValve);
  OR.addObject(pipeB);
  OR.addObject(bellowB);
  OR.addObject(pipeC);
  OR.addObject(QuadA);
  OR.addObject(cMagVertA);
}
  
L2SPFsegment10::~L2SPFsegment10()
  /*!
    Destructor
   */
{}

void
L2SPFsegment10::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("L2SPFsegment10","buildObjects");

  int outerCell;

  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  if (isActive("front"))
    pipeA->copyCutSurf("front",*this,"front");
  pipeA->createAll(System,*this,0);
  outerCell=masterCell->getName();
  //  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*bellowA,2);
  pipeA->insertInCell(System,outerCell);

  //  buildZone->removeLastMaster(System);  
  return;
  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*bellowA);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*bellowA,"back",*gateValve);


  // FAKE INSERT REQUIRED
  pumpA->addAllInsertCell(masterCell->getName());
  pumpA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpA->createAll(System,*bellowA,"back");

  const constructSystem::portItem& VPB=pumpA->getPort(1);
  outerCell=buildZone->createOuterVoidUnit
    (System,masterCell,VPB,VPB.getSideIndex("OuterPlate"));
  pumpA->insertAllInCell(System,outerCell);
  
  constructSystem::constructUnit
    (System,*buildZone,masterCell,VPB,"OuterPlate",*pipeB);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeB,"back",*bellowB);

  pipeC->createAll(System,*bellowB,"back");
  pipeMagUnit(System,*buildZone,pipeC,"#front","outerPipe",QuadA);
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",cMagVertA);
  pipeTerminate(System,*buildZone,pipeC);
  
  buildZone->removeLastMaster(System);  
  return;
}

void
L2SPFsegment10::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*pipeA,1);
  setLinkSignedCopy(1,*pipeA,2);
  //  setLinkSignedCopy(1,*pipeC,2);

  TDCsegment::setLastSurf(FixedComp::getFullRule(2));
  return;
}

void 
L2SPFsegment10::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
   */
{
  // For output stream
  ELog::RegMethod RControl("L2SPFsegment10","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();
  
  return;
}


}   // NAMESPACE tdcSystem

