/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/L2SPFsegment11.cxx
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
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "Bellows.h"
#include "LQuadF.h"
#include "BPM.h"
#include "CorrectorMag.h"
#include "YagUnit.h"
#include "YagScreen.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "L2SPFsegment11.h"

namespace tdcSystem
{

// Note currently uncopied:

  
L2SPFsegment11::L2SPFsegment11(const std::string& Key) :
  TDCsegment(Key,2),

  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  bpm(new tdcSystem::BPM(keyName+"BPM")),  
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  QuadA(new tdcSystem::LQuadF(keyName+"QuadA")),
  yagUnit(new tdcSystem::YagUnit(keyName+"YagUnit")),
  yagScreen(new tdcSystem::YagScreen(keyName+"YagScreen")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagHorA(new tdcSystem::CorrectorMag(keyName+"CMagHorA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(bpm);
  OR.addObject(pipeA);
  OR.addObject(QuadA);
  OR.addObject(yagUnit);
  OR.addObject(pipeB);
  OR.addObject(cMagHorA);

  setFirstItem(bellowA);
}
  
L2SPFsegment11::~L2SPFsegment11()
  /*!
    Destructor
   */
{}

void
L2SPFsegment11::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("L2SPFsegment11","buildObjects");

  int outerCell;

  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  if (isActive("front"))
    bellowA->copyCutSurf("front",*this,"front");
  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*bellowA,"back",*bpm);

  pipeA->createAll(System,*bpm,"back");
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",QuadA);
  pipeTerminate(System,*buildZone,pipeA);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*yagUnit);

  
  yagScreen->setBeamAxis(*yagUnit,1);
  yagScreen->createAll(System,*yagUnit,-3);
  yagScreen->insertInCell("Outer",System,outerCell);
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("PlateA"));
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
  yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));
    
  pipeB->createAll(System,*yagUnit,"back");
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",cMagHorA);
  pipeTerminate(System,*buildZone,pipeB);
  
  buildZone->removeLastMaster(System);  
  return;
}

void
L2SPFsegment11::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*bellowA,1);
  setLinkSignedCopy(1,*pipeB,2);

  TDCsegment::setLastSurf(FixedComp::getFullRule(2));
  return;
}

void 
L2SPFsegment11::createAll(Simulation& System,
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
  ELog::RegMethod RControl("L2SPFsegment11","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();
  
  return;
}


}   // NAMESPACE tdcSystem

