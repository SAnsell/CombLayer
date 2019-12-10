/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: danmax/danmaxConnectLine.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "FixedOffsetUnit.h"
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
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "LeadPipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "SqrShield.h"

#include "danmaxConnectLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
danmaxConnectLine::danmaxConnectLine(const std::string& Key) :
  attachSystem::FixedOffsetUnit(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  buildZone(*this,cellIndex),
  connectShield(new xraySystem::SqrShield(keyName+"ConnectShield")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  ionPumpA(new constructSystem::PortTube(keyName+"IonPumpA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  ionPumpB(new constructSystem::PortTube(keyName+"IonPumpB")),
  pipeD(new constructSystem::VacuumPipe(keyName+"PipeD")),
  bellowC(new constructSystem::Bellows(keyName+"BellowC"))
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(pipeA);
  OR.addObject(ionPumpA);
  OR.addObject(pipeB);
  OR.addObject(bellowB);
  OR.addObject(pipeC);
  OR.addObject(ionPumpB);
  OR.addObject(pipeD);
  OR.addObject(bellowC);
}
  
danmaxConnectLine::~danmaxConnectLine()
  /*!
    Destructor
   */
{}
  
void
danmaxConnectLine::buildObjects(Simulation& System,
				const attachSystem::FixedComp& FC,
				const std::string& sideName,
				const attachSystem::FixedComp& beamFC,
				const std::string& beamName)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
    \parma FC :: Connection point
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("danmaxConnectLine","buildObjects");
  // First build construction zone
  int outerCell;
  buildZone.setFront(getRule("front"));
  buildZone.setBack(getRule("back"));  

  connectShield->setInsertCell(getInsertCells());
  connectShield->setFront(*this);
  connectShield->setBack(*this);
  connectShield->createAll(System,FC,sideName);
  
  buildZone.setSurround(connectShield->getInnerVoid());

  MonteCarlo::Object* masterCell=
    buildZone.constructMasterCell(System);

  buildZone.createOuterVoidUnit(System,masterCell,beamFC,beamName);

  //  ELog::EM<<"Get cell == "<<getCell("OuterVoid")<<ELog::endDiag;
  // insert first tube:
  outerCell=xrayConstruct::constructUnit
    (System,buildZone,masterCell,beamFC,beamName,*pipeA);

  ionPumpA->addAllInsertCell(outerCell);
  ionPumpA->setFront(*pipeA,2);
  ionPumpA->createAll(System,*pipeA,2);

  /*
  // SKIP PIPE
  pipeC->createAll(System,*pipeB,2);
  // Now build lead box
  boxB->addInsertCell("FrontWall",pipeB->getCell("BackSpaceVoid"));
  boxB->addInsertCell("BackWall",pipeC->getCell("FrontSpaceVoid"));
  boxB->setCutSurf("portCutA",*pipeB,"pipeWall");
  boxB->setCutSurf("portCutB",*pipeC,"pipeWall");
  boxB->setCutSurf("leadRadiusA",*pipeB,"outerPipe");
  boxB->setCutSurf("leadRadiusB",*pipeC,"outerPipe");

  boxB->createAll(System,*pipeB,2);
  boxB->splitObjectAbsolute(System,1001,
		    boxB->getCell("Void"),
		    {{pipeB->getLinkPt(2),pipeC->getLinkPt(1)}},
		    {{pipeB->getLinkAxis(2),pipeC->getLinkAxis(-1)}});
  

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*boxB,-1);
  pipeB->insertInCell(System,outerCell);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*boxB,2);
  boxB->insertInCell("Main",System,outerCell);


  // Bellow goes immediately in next unit
  bellowB->addInsertCell(boxB->getCell("Void",1));
  bellowB->setFront(*pipeB,2);  
  bellowB->setBack(*pipeC,1);
  bellowB->createAll(System,*pipeB,2);

  pipeB->insertInCell(System,boxB->getCell("Void",0));
  pipeC->insertInCell(System,boxB->getCell("Void",2));

  // SKIP :: pipeD is placed and the ion pump bridges
  pipeD->createAll(System,*pipeC,2);

  pumpBoxB->addInsertCell("FrontWall",pipeC->getCell("BackSpaceVoid"));
  pumpBoxB->addInsertCell("BackWall",pipeD->getCell("FrontSpaceVoid"));
  
  pumpBoxB->setCutSurf("portCutA",*pipeC,"pipeWall");
  pumpBoxB->setCutSurf("portCutB",*pipeD,"pipeWall");
  pumpBoxB->setCutSurf("leadRadiusA",*pipeC,"outerPipe");
  pumpBoxB->setCutSurf("leadRadiusB",*pipeD,"outerPipe");
  pumpBoxB->createAll(System,*pipeC,2);  
  pumpBoxB->splitObjectAbsolute
    (System,1001,
     pumpBoxB->getCell("Void"),
     {{pipeC->getLinkPt(2),pipeD->getLinkPt(1)}},
     {{pipeC->getLinkAxis(2),pipeD->getLinkAxis(-1)}});

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pumpBoxB,-1);
  pipeC->insertInCell(System,outerCell);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pumpBoxB,2);

  pumpBoxB->insertInCell("Main",System,outerCell);
  pipeC->insertInCell(System,pumpBoxB->getCell("Void",0));
  pipeD->insertInCell(System,pumpBoxB->getCell("Void",2));

  ionPumpB->delayPorts();
  ionPumpB->addAllInsertCell(pumpBoxB->getCell("Void",1));
  // ionPumpB->setFront(*pipeC,2);
  // ionPumpB->setBack(*pipeD,1);
  pumpBoxB->setCutSurf("portCutA",*pipeC,"pipeWall");  // lead line
  pumpBoxB->setCutSurf("portCutB",*pipeD,"pipeWall");
  pumpBoxB->setCutSurf("leadRadiusA",*pipeC,"outerPipe");
  pumpBoxB->setCutSurf("leadRadiusB",*pipeD,"outerPipe");


  ionPumpB->createAll(System,*pipeC,2);
  // ionPumpB->createPorts(System);

  // SKIP :: Join PipeC skips bellows
  JPipe->createAll(System,*pipeD,2);

  // Now build lead box
  boxC->addInsertCell("FrontWall",pipeD->getCell("BackSpaceVoid"));
  boxC->addInsertCell("BackWall",JPipe->getCell("FrontSpaceVoid"));
  boxC->setCutSurf("portCutA",*pipeD,"pipeWall");
  boxC->setCutSurf("portCutB",*JPipe,"pipeWall");
  boxC->setCutSurf("leadRadiusA",*pipeD,"outerPipe");
  boxC->setCutSurf("leadRadiusB",*JPipe,"outerPipe");

  boxC->createAll(System,*pipeD,2);
  boxC->splitObjectAbsolute(System,1001,
		    boxC->getCell("Void"),
		    {{pipeD->getLinkPt(2),JPipe->getLinkPt(1)}},
		    {{pipeD->getLinkAxis(2),JPipe->getLinkAxis(-1)}});


  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*boxC,-1);
  pipeD->insertInCell(System,outerCell);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*boxC,2);
  boxC->insertInCell("Main",System,outerCell);

  pipeD->insertInCell(System,boxC->getCell("Void",0));
  JPipe->insertInCell(System,boxC->getCell("Void",2));
  
    // Bellow goes immediately in next unit
  bellowC->addInsertCell(boxC->getCell("Void",1));
  bellowC->setFront(*pipeD,2);  
  bellowC->setBack(*JPipe,1);
  bellowC->createAll(System,*pipeD,2);

  JPipe->insertInCell(System,masterCell->getName());
  */
  return;
}

// void
// danmaxConnectLine::createLinks()
//   /*!
//     Create a front/back link
//   */
// {
//   setLinkSignedCopy(0,*bellowA,1);
//   setLinkSignedCopy(1,*bellowC,2);
//   return;
// }
  
  
void 
danmaxConnectLine::construct(Simulation& System,
			     const attachSystem::FixedComp& FC,
			     const std::string& sideName,
			     const attachSystem::FixedComp& beamFC,
			     const std::string& beamName)
  
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component (hutch wall)
    \param sideName :: link point (hutch wall)
    \param beamFC :: Fixed component for pipe/beam
    \param beamName :: link point for beam
   */
{
  // For output stream
  ELog::RegMethod RControl("danmaxConnectLine","createAll");

  buildObjects(System,FC,sideName,beamFC,beamName);    
  insertObjects(System);
  return;
}


}   // NAMESPACE xraySystem

