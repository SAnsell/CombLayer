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
ConnectZone::createOuterVoid(Simulation& System)
  /*!
    Construct outer void object
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("ConnectZone","createOuterVoid");

  if (outerRadius>Geometry::zeroTol)
    {
      std::string Out;
      // Trick - make cell and THEN add front/back cell
      ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);
      Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
      Out+=frontRule()+backRule();
      makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);
      addOuterSurf(Out);
      insertObjects(System);
      addInsertCell(getCell("OuterVoid"));
    }
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

  bellowA->addInsertCell(ContainedComp::getInsertCells());
  bellowA->setFront(FC,sideIndex);
  bellowA->registerSpaceCut(1,2);
  bellowA->createAll(System,FC,sideIndex);

  boxA->addInsertCell("Main",bellowA->getCell("OuterSpace"));
  boxA->addInsertCell("Walls",bellowA->getCell("FrontSpaceVoid"));
  boxA->addInsertCell("Walls",bellowA->getCell("BackSpaceVoid"));
  boxA->addBoundarySurf("Main",bellowA->getLinkSurf("outerPipe"));
  boxA->setCutSurf("portCut",*bellowA,"pipeWall");
  boxA->createAll(System,*bellowA,0);
  
  pipeA->addInsertCell(ContainedComp::getInsertCells());
  pipeA->setFront(*bellowA,2);
  pipeA->registerSpaceCut(1,2);
  pipeA->createAll(System,*bellowA,2);

  ionPumpA->addInsertCell(ContainedComp::getInsertCells());
  ionPumpA->registerSpaceCut(1,2);
  ionPumpA->delayPorts();
  ionPumpA->setFront(*pipeA,2);
  ionPumpA->createAll(System,*pipeA,2);

  pumpBoxA->addInsertCell("Main",ionPumpA->getCell("OuterSpace"));
  pumpBoxA->addBoundarySurf("Main",ionPumpA->getLinkSurf("mainPipe"));
  pumpBoxA->setCutSurf("portCutA",*ionPumpA,"portAPipe");
  pumpBoxA->setCutSurf("portCutB",*ionPumpA,"portBPipe");
  pumpBoxA->createAll(System,*ionPumpA,0);
  ionPumpA->createPorts(System);

  pipeB->addInsertCell(ContainedComp::getInsertCells());
  pipeB->registerSpaceCut(1,2);
  pipeB->setFront(*ionPumpA,2);
  pipeB->createAll(System,*ionPumpA,2);

  bellowB->addInsertCell(ContainedComp::getInsertCells());
  bellowB->registerSpaceCut(1,2);
  bellowB->setFront(*pipeB,2);
  bellowB->createAll(System,*pipeB,2);

  boxB->addInsertCell("Main",bellowB->getCell("OuterSpace"));
  boxB->addInsertCell("Walls",bellowB->getCell("FrontSpaceVoid"));
  boxB->addInsertCell("Walls",bellowB->getCell("BackSpaceVoid"));
  boxB->addBoundarySurf("Main",bellowB->getLinkSurf("outerPipe"));
  boxB->setCutSurf("portCut",*bellowB,"pipeWall");
  boxB->createAll(System,*bellowB,0);
  
  pipeC->addInsertCell(ContainedComp::getInsertCells());
  pipeC->registerSpaceCut(1,2);
  pipeC->setFront(*bellowB,2);
  pipeC->createAll(System,*bellowB,2);

  ionPumpB->addInsertCell(ContainedComp::getInsertCells());
  ionPumpB->registerSpaceCut(1,2);
  ionPumpB->delayPorts();
  ionPumpB->setFront(*pipeC,2);
  ionPumpB->createAll(System,*pipeC,2);

  
  pumpBoxB->addInsertCell("Main",ionPumpB->getCell("OuterSpace"));
  pumpBoxB->addBoundarySurf("Main",ionPumpB->getLinkSurf("mainPipe"));
  pumpBoxB->setCutSurf("portCutA",*ionPumpB,"portAPipe");
  pumpBoxB->setCutSurf("portCutB",*ionPumpB,"portBPipe");
  pumpBoxB->createAll(System,*ionPumpB,0);
  ionPumpB->createPorts(System);
    
  pipeD->addInsertCell(ContainedComp::getInsertCells());
  pipeD->registerSpaceCut(1,2);
  pipeD->setFront(*ionPumpB,2);
  pipeD->createAll(System,*ionPumpB,2);

  bellowC->addInsertCell(ContainedComp::getInsertCells());
  bellowC->registerSpaceCut(1,2);
  bellowC->setFront(*pipeD,2);
  bellowC->createAll(System,*pipeD,2);

  
  boxC->addInsertCell("Main",bellowC->getCell("OuterSpace"));
  boxC->addInsertCell("Walls",bellowC->getCell("FrontSpaceVoid"));
  boxC->addInsertCell("Walls",bellowC->getCell("BackSpaceVoid"));
  boxC->addBoundarySurf("Main",bellowC->getLinkSurf("outerPipe"));
  boxC->setCutSurf("portCut",*bellowC,"pipeWall");
  boxC->createAll(System,*bellowC,0);

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
  createOuterVoid(System);
  buildObjects(System,FC,sideIndex);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

