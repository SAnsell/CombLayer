/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/L2SPFsegment4.cxx
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
#include "Line.h"
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
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "CorrectorMag.h"
#include "BPM.h"
#include "LQuadF.h"
#include "LSexupole.h"
#include "CorrectorMag.h"
#include "YagUnit.h"
#include "YagScreen.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "L2SPFsegment4.h"

namespace tdcSystem
{

// Note currently uncopied:
  
L2SPFsegment4::L2SPFsegment4(const std::string& Key) :
  TDCsegment(Key,2),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bpmA(new tdcSystem::BPM(keyName+"BPMA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  QuadA(new tdcSystem::LQuadF(keyName+"QuadA")),
  SexuA(new tdcSystem::LSexupole(keyName+"SexuA")),
  QuadB(new tdcSystem::LQuadF(keyName+"QuadB")),
  yagUnit(new tdcSystem::YagUnit(keyName+"YagUnit")),
  yagScreen(new tdcSystem::YagScreen(keyName+"YagScreen")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  cMagHorC(new tdcSystem::CorrectorMag(keyName+"CMagHorC")),
  cMagVertC(new tdcSystem::CorrectorMag(keyName+"CMagVertC"))
  
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(bpmA);
  OR.addObject(pipeB);
  OR.addObject(QuadA);
  OR.addObject(SexuA);
  OR.addObject(QuadB);
  OR.addObject(yagUnit);
  OR.addObject(yagScreen);
  OR.addObject(bellowA);
  OR.addObject(pipeC);
  OR.addObject(cMagHorC);
  OR.addObject(cMagVertC);  

}
  
L2SPFsegment4::~L2SPFsegment4()
  /*!
    Destructor
   */
{}

void
L2SPFsegment4::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("L2SPFsegment4","buildObjects");

  int outerCell;

  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  if (isActive("front"))
    pipeA->copyCutSurf("front",*this,"front");
  pipeA->createAll(System,*this,0);
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*pipeA,2);
  pipeA->insertInCell(System,outerCell);
  
  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*bpmA);

  pipeB->createAll(System,*bpmA,"back");
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",QuadA);
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",SexuA);
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",QuadB);
  pipeTerminate(System,*buildZone,pipeB);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeB,"back",*yagUnit);

  yagScreen->setBeamAxis(*yagUnit,1);
  yagScreen->createAll(System,*yagUnit,-3);
  yagScreen->insertInCell("Outer",System,outerCell);
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("PlateA"));
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
  yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*yagUnit,"back",*bellowA);

  pipeC->createAll(System,*bellowA,"back");
  correctorMagnetPair(System,*buildZone,pipeC,cMagHorC,cMagVertC);
  pipeTerminate(System,*buildZone,pipeC);  
  
  buildZone->removeLastMaster(System);  
  return;
}

void
L2SPFsegment4::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*pipeA,1);
  setLinkSignedCopy(1,*pipeC,2);

  TDCsegment::setLastSurf(FixedComp::getFullRule(2));
  return;
}

void 
L2SPFsegment4::createAll(Simulation& System,
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
  ELog::RegMethod RControl("L2SPFsegment4","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem

