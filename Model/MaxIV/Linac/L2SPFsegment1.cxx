/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/L2SPFsegment1.cxx
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
#include "FixedOffsetGroup.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "BlankTube.h"
#include "LQuad.h"
#include "CorrectorMag.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "L2SPFsegment1.h"

namespace tdcSystem
{

// Note currently uncopied:
  
L2SPFsegment1::L2SPFsegment1(const std::string& Key) :
  TDCsegment(Key,2),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagHorrA(new tdcSystem::CorrectorMag(keyName+"CMagHorrA")),
  cMagVertA(new tdcSystem::CorrectorMag(keyName+"CMagVertA")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  pipeD(new constructSystem::VacuumPipe(keyName+"PipeD")),
  cMagHorrB(new tdcSystem::CorrectorMag(keyName+"CMagHorrB")),
  cMagVertB(new tdcSystem::CorrectorMag(keyName+"CMagVertB")),
  QuadA(new tdcSystem::LQuad(keyName+"QuadA")),
  pipeE(new constructSystem::VacuumPipe(keyName+"PipeE")),
  pipeF(new constructSystem::VacuumPipe(keyName+"PipeF")),
  cMagHorrC(new tdcSystem::CorrectorMag(keyName+"CMagHorrC")),
  cMagVertC(new tdcSystem::CorrectorMag(keyName+"CMagVertC")),
  pumpA(new constructSystem::BlankTube(keyName+"PumpA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(pipeB);
  OR.addObject(cMagHorrA);
  OR.addObject(cMagVertA);
  OR.addObject(pipeC);
  OR.addObject(pipeD);
  OR.addObject(cMagHorrB);
  OR.addObject(cMagVertB);
  OR.addObject(QuadA);
  OR.addObject(pipeE);
  OR.addObject(pumpA);
}
  
L2SPFsegment1::~L2SPFsegment1()
  /*!
    Destructor
   */
{}

void
L2SPFsegment1::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("L2SPFsegment1","buildObjects");

  int outerCell;
  
  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  pipeA->createAll(System,*this,0);
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*pipeA,2);
  pipeA->insertInCell(System,outerCell);
  
  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*bellowA);

  //
  // build pipe + corrector magnets together:
  // THIS becomes a function:
  //
  pipeB->createAll(System,*bellowA,"back");
  correctorMagnetPair(System,*buildZone,pipeB,cMagHorrA,cMagVertA);
  
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*pipeB,2);
  pipeB->insertInCell(System,outerCell);


  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeB,"back",*pipeC);

  pipeD->createAll(System,*pipeC,"back");  
  correctorMagnetPair(System,*buildZone,pipeD,cMagHorrB,cMagVertB);
 
  pipeMagUnit(System,*buildZone,pipeD,"#front",QuadA);
  pipeTerminate(System,*buildZone,pipeD);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeD,"back",*pipeE);

  pipeF->createAll(System,*pipeE,"back");  
  correctorMagnetPair(System,*buildZone,pipeF,cMagHorrC,cMagVertC);
  pipeTerminate(System,*buildZone,pipeF);

    // FAKE INSERT REQUIRED
  pumpA->addAllInsertCell(masterCell->getName());
  pumpA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpA->createAll(System,*pipeF,"back");

  const constructSystem::portItem& VPB=pumpA->getPort(1);
  outerCell=buildZone->createOuterVoidUnit
    (System,masterCell,VPB,VPB.getSideIndex("OuterPlate"));
  pumpA->insertAllInCell(System,outerCell);

  buildZone->removeLastMaster(System);  
  return;
}

void
L2SPFsegment1::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*pipeA,1);

  
  const constructSystem::portItem& VPB=pumpA->getPort(1);
  setLinkSignedCopy(1,VPB,VPB.getSideIndex("OuterPlate"));

  TDCsegment::setLastSurf(FixedComp::getFullRule(2));
  return;
}

void 
L2SPFsegment1::createAll(Simulation& System,
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
  ELog::RegMethod RControl("L2SPFsegment1","build");

  FixedRotate::populate(System.getDataBase());	  
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem

