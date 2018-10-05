/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/ConnectZone.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "Qhull.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"


#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "LeadPipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "CrossPipe.h"
#include "LeadBox.h"

#include "ConnectZone.h"

namespace xraySystem
{

// Note currently uncopied:
  
ConnectZone::ConnectZone(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  boxA(new xraySystem::LeadBox(newName+"LeadA")),
  pipeA(new constructSystem::LeadPipe(newName+"PipeA")),
  ionPumpA(new constructSystem::PortTube(newName+"IonPumpA")),
  pumpBoxA(new xraySystem::LeadBox(newName+"PumpBoxA")),
  pipeB(new constructSystem::LeadPipe(newName+"PipeB")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  boxB(new xraySystem::LeadBox(newName+"LeadB")),
  pipeC(new constructSystem::LeadPipe(newName+"PipeC")),
  ionPumpB(new constructSystem::PortTube(newName+"IonPumpB")),
  pumpBoxB(new xraySystem::LeadBox(newName+"PumpBoxB")),
  pipeD(new constructSystem::LeadPipe(newName+"PipeD")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  boxC(new xraySystem::LeadBox(newName+"LeadC")),
  outerRadius(0.0)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  
  OR.addObject(bellowA);
  OR.addObject(boxA);
  OR.addObject(pipeA);
  OR.addObject(ionPumpA);
  OR.addObject(pumpBoxA);
  OR.addObject(pipeB);
  OR.addObject(bellowB);
  OR.addObject(boxB);
  OR.addObject(pipeC);
  OR.addObject(ionPumpB);
  OR.addObject(pumpBoxB);
  OR.addObject(pipeD);
  OR.addObject(bellowC);
  OR.addObject(boxC);
}
  
ConnectZone::~ConnectZone()
  /*!
    Destructor
   */
{}

void
ConnectZone::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: DataBase for variables
   */
{
  FixedOffset::populate(Control);
  outerRadius=Control.EvalDefVar<double>(keyName+"OuterRadius",0.0);
  return;
}

void
ConnectZone::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
/*!
    Create the unit vectors
    Note that the FC:in and FC:out are tied to Main
    -- rotate position Main and then Out/In are moved relative

    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("ConnectZone","createUnitVector");

  FixedOffset::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
ConnectZone::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("ConnectZone","createSurfaces");

  if (outerRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);
  
  return;
}

  
int
ConnectZone::createOuterVoidUnit(Simulation& System,
				 const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Construct outer void object
    \param System :: Simulation
    \param FC :: FixedComp
    \param sideIndex :: link point
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("ConnectZone","createOuterVoid");

  static HeadRule divider;
  // construct an cell based on previous cell:
  std::string Out;
  
  if (!divider.hasRule())
    divider=FrontBackCut::getFrontRule();

  const HeadRule& backHR=
    (sideIndex) ? FC.getFullRule(-sideIndex) :
    FrontBackCut::getBackRule();
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  Out+=divider.display()+backHR.display();
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);


  divider=backHR;
  divider.makeComplement();
  
  return cellIndex-1;
}

void
ConnectZone::insertFirstRegion(Simulation& System,const int cellN)
  /*!
    Inserts the first component in the main wall / front wall
    \param System :: Simuation 
    \param cellN :: Cell number to place into region

   */
{
  ELog::RegMethod RegA("ConnectZone","insertFirstRegion");
 
  boxA->insertInCell("FrontWall",System,cellN);
  boxA->insertInCell("MainWall",System,cellN);

  return;
}
  
void
ConnectZone::buildObjects(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
    \parma FC :: Connection poitn
    \param sideIndex :: link piont
  */
{
  ELog::RegMethod RegA("ConnectZone","buildObjects");

  const attachSystem::CellMap* CMPtr=
    dynamic_cast<const attachSystem::CellMap*>(&FC);
  const attachSystem::ContainedComp* CPtr=
    dynamic_cast<const attachSystem::ContainedComp*>(&FC);

  std::string Out;
  
  // create first unit:

  // build jump pipe [fit bellows in between]
  pipeA->createAll(System,FC,sideIndex);
  
  // Now build lead box
  if (CMPtr)
    boxA->addInsertCell("FrontWall",CMPtr->getCell("BackSpaceVoid"));
  boxA->addInsertCell("BackWall",pipeA->getCell("FrontSpaceVoid"));
  boxA->setCutSurf("portCutA",FC,"pipeWall");
  boxA->setCutSurf("portCutB",*pipeA,"pipeWall");
  boxA->createAll(System,FC,sideIndex);
  boxA->splitObjectAbsolute
    (System,1001,
     boxA->getCell("Void"),
     {{FC.getLinkPt(sideIndex),pipeA->getLinkPt(1)}},
     {{FC.getLinkAxis(sideIndex),pipeA->getLinkAxis(-1)}});
  

  int pipeCell=createOuterVoidUnit(System,*boxA,-1);
  CellMap::setCell("firstVoid",getCell("OuterVoid"));

  int boxCell=createOuterVoidUnit(System,*boxA,2);
  boxA->insertInCell("Main",System,boxCell);

  CPtr->insertInCell(System,boxA->getCell("Void",0));
  pipeA->insertInCell(System,boxA->getCell("Void",2));

  // Bellow goes immediately in next unit
  bellowA->addInsertCell(boxA->getCell("Void",1));
  bellowA->setFront(FC,sideIndex);  
  bellowA->setBack(*pipeA,1);
  bellowA->createAll(System,FC,sideIndex);


  // SKIP :: pipe B is placed and the ion pump bridges

  pipeB->createAll(System,*pipeA,2);

  pumpBoxA->addInsertCell("FrontWall",pipeA->getCell("BackSpaceVoid"));
  pumpBoxA->addInsertCell("BackWall",pipeB->getCell("FrontSpaceVoid"));

  pumpBoxA->setCutSurf("portCutA",*pipeA,"pipeWall");
  pumpBoxA->setCutSurf("portCutB",*pipeB,"pipeWall");
  pumpBoxA->createAll(System,*pipeA,2);    
  pumpBoxA->splitObjectAbsolute
    (System,1001,
     pumpBoxA->getCell("Void"),
     {{pipeA->getLinkPt(2),pipeB->getLinkPt(1)}},
     {{pipeA->getLinkAxis(2),pipeB->getLinkAxis(-1)}});

  pipeCell=createOuterVoidUnit(System,*pumpBoxA,-1);
  pipeA->insertInCell(System,pipeCell);

  boxCell=createOuterVoidUnit(System,*pumpBoxA,2);
  pumpBoxA->insertInCell("Main",System,boxCell);
  pipeA->insertInCell(System,pumpBoxA->getCell("Void",0));
  pipeB->insertInCell(System,pumpBoxA->getCell("Void",2));

  ionPumpA->delayPorts();
  ionPumpA->setInsertCell(pumpBoxA->getCell("Void",1));
  ionPumpA->setFront(*pipeA,2);
  ionPumpA->setBack(*pipeB,1);
  ionPumpA->createAll(System,*pipeA,2);
  // ionPumpA->createPorts(System);

  // SKIP PIPE
  pipeC->createAll(System,*pipeB,2);

  // Now build lead box
  boxB->addInsertCell("FrontWall",pipeB->getCell("BackSpaceVoid"));
  boxB->addInsertCell("BackWall",pipeC->getCell("FrontSpaceVoid"));
  boxB->setCutSurf("portCutA",*pipeB,"pipeWall");
  boxB->setCutSurf("portCutB",*pipeC,"pipeWall");
  boxB->createAll(System,*pipeB,2);
  boxB->splitObjectAbsolute(System,1001,
		    boxB->getCell("Void"),
		    {{pipeB->getLinkPt(2),pipeC->getLinkPt(1)}},
		    {{pipeB->getLinkAxis(2),pipeC->getLinkAxis(-1)}});
  
  pipeCell=createOuterVoidUnit(System,*boxB,-1);
  pipeB->insertInCell(System,pipeCell);
  
  boxCell=createOuterVoidUnit(System,*boxB,2);
  boxB->insertInCell("Main",System,boxCell);

  
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
  pumpBoxB->createAll(System,*pipeC,2);  
  pumpBoxB->splitObjectAbsolute
    (System,1001,
     pumpBoxB->getCell("Void"),
     {{pipeC->getLinkPt(2),pipeD->getLinkPt(1)}},
     {{pipeC->getLinkAxis(2),pipeD->getLinkAxis(-1)}});

  pipeCell=createOuterVoidUnit(System,*pumpBoxB,-1);
  pipeC->insertInCell(System,pipeCell);

  boxCell=createOuterVoidUnit(System,*pumpBoxB,2);
  pumpBoxB->insertInCell("Main",System,boxCell);
  pipeC->insertInCell(System,pumpBoxB->getCell("Void",0));
  pipeD->insertInCell(System,pumpBoxB->getCell("Void",2));

  ionPumpB->delayPorts();
  ionPumpB->setInsertCell(pumpBoxB->getCell("Void",1));
  ionPumpB->setFront(*pipeC,2);
  ionPumpB->setBack(*pipeD,1);
  ionPumpB->createAll(System,*pipeC,2);
  // ionPumpB->createPorts(System);

  // SKIP :: Join PipeC skips bellows
  JPipe->createAll(System,*pipeD,2);

  // Now build lead box
  boxC->addInsertCell("FrontWall",pipeD->getCell("BackSpaceVoid"));
  boxC->addInsertCell("BackWall",JPipe->getCell("FrontSpaceVoid"));
  boxC->setCutSurf("portCutA",*pipeD,"pipeWall");
  boxC->setCutSurf("portCutB",*JPipe,"pipeWall");
  boxC->createAll(System,*pipeD,2);
  boxC->splitObjectAbsolute(System,1001,
		    boxC->getCell("Void"),
		    {{pipeD->getLinkPt(2),JPipe->getLinkPt(1)}},
		    {{pipeD->getLinkAxis(2),JPipe->getLinkAxis(-1)}});

  pipeCell=createOuterVoidUnit(System,*boxC,-1);
  pipeD->insertInCell(System,pipeCell);
  
  boxCell=createOuterVoidUnit(System,*boxC,2);
  boxC->insertInCell("Main",System,boxCell);

  pipeD->insertInCell(System,boxC->getCell("Void",0));
  JPipe->insertInCell(System,boxC->getCell("Void",2));
  
    // Bellow goes immediately in next unit
  bellowC->addInsertCell(boxC->getCell("Void",1));
  bellowC->setFront(*pipeD,2);  
  bellowC->setBack(*JPipe,1);
  bellowC->createAll(System,*pipeD,2);

  boxCell=createOuterVoidUnit(System,*this,0);
  JPipe->insertInCell(System,boxCell);
  return;
}

void
ConnectZone::createLinks()
  /*!
    Create a front/back link
  */
{
  setLinkSignedCopy(0,*bellowA,1);
  setLinkSignedCopy(1,*bellowC,2);
  return;
}
  
  
void 
ConnectZone::createAll(Simulation& System,
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
  ELog::RegMethod RControl("ConnectZone","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  //  createOuterVoid(System,FC.sideIndex);

  buildObjects(System,FC,sideIndex);

  std::string Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  Out+=frontRule()+backRule();
  addOuterSurf(Out);
    
  createLinks();
  insertObjects(System);
  return;
}


}   // NAMESPACE xraySystem

