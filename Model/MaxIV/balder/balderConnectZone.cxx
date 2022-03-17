/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/balderConnectZone.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "BlockZone.h"
#include "generateSurf.h"

#include "SplitFlangePipe.h"
#include "LeadPipe.h"
#include "Bellows.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "LeadBox.h"

#include "balderConnectZone.h"

namespace xraySystem
{

// Note currently uncopied:
  
balderConnectZone::balderConnectZone(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  buildZone(newName+"BuildZone"),
  innerMat(0),
  
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  boxA(new xraySystem::LeadBox(newName+"LeadA")),
  pipeA(new constructSystem::LeadPipe(newName+"PipeA")),
  ionPumpA(new constructSystem::PipeTube(newName+"IonPumpA")),
  pumpBoxA(new xraySystem::LeadBox(newName+"PumpBoxA")),
  pipeB(new constructSystem::LeadPipe(newName+"PipeB")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  boxB(new xraySystem::LeadBox(newName+"LeadB")),
  pipeC(new constructSystem::LeadPipe(newName+"PipeC")),
  ionPumpB(new constructSystem::PipeTube(newName+"IonPumpB")),
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
  
balderConnectZone::~balderConnectZone()
  /*!
    Destructor
   */
{}

void
balderConnectZone::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: DataBase for variables
   */
{
  ELog::RegMethod RegA("balderConnectZone","populate");
  FixedOffset::populate(Control);

  outerRadius=Control.EvalDefVar<double>(keyName+"OuterRadius",0.0);
  return;
}

void
balderConnectZone::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("balderConnectZone","createSurfaces");

  if (outerRadius>Geometry::zeroTol)
    {
      ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);
      buildZone.setSurround(HeadRule(-SMap.realSurf(buildIndex+7)));

      buildZone.setFront(getRule("front"));
      buildZone.setMaxExtent(getRule("back"));
      buildZone.setInnerMat(innerMat);
    }
  return;
}

template<typename MidTYPE>
void
balderConnectZone::makeBox(Simulation& System,
			   const constructSystem::LeadPipe& pipeAUnit,
			   const long int pipeASide,
			   LeadBox& boxUnit,
			   MidTYPE& bellowUnit,
			   constructSystem::LeadPipe& pipeBUnit)
  /*!
    Build a box/pipe assemply:
    Assumption pipeA :: Already built
  */
{
  ELog::RegMethod RegA("balderConnectZone","makeBox");
    

  int outerCell;
  
  boxUnit.setCutSurf("portCutA",pipeAUnit,"pipe");
  boxUnit.setCutSurf("portCutB",pipeBUnit,"pipe");
  boxUnit.setCutSurf("leadRadiusA",pipeAUnit,"outer");
  boxUnit.setCutSurf("leadRadiusB",pipeBUnit,"outer");

  // create first unit:

  // First build to set bellows/Box
  //  pipeAUnit->createAll(System,*this,0);
  boxUnit.createAll(System,pipeAUnit,pipeASide);
  boxUnit.insertInCell("FrontWall",System,pipeAUnit.getCell("LeadBGap"));
  boxUnit.insertInCell("BackWall",System,pipeBUnit.getCell("LeadAGap"));

  //  boxA->addInsertCell("BackWall",pipeAUnit->getCell("OuterVoid"));

  outerCell=buildZone.createUnit(System,boxUnit,-1);
  pipeAUnit.insertInCell("Main",System,outerCell);
  outerCell=buildZone.createUnit(System,boxUnit,2);
  boxUnit.insertInCell("Main",System,outerCell);

  boxUnit.splitObjectAbsolute
    (System,1001,
     boxUnit.getCell("Void"),
     {{pipeAUnit.getLinkPt(pipeASide),pipeBUnit.getLinkPt(1)}},
     {{pipeAUnit.getLinkAxis(pipeASide),pipeBUnit.getLinkAxis(-1)}});
  
  pipeAUnit.insertInCell("Main",System,boxUnit.getCell("Void",0));
  pipeAUnit.insertInCell("FlangeB",System,boxUnit.getCell("Void",0));
  pipeBUnit.insertInCell("Main",System,boxUnit.getCell("Void",1));
  pipeBUnit.insertInCell("FlangeA",System,boxUnit.getCell("Void",1));

  if constexpr (std::is_same<constructSystem::Bellows,MidTYPE>::value)
    {
      // Bellow goes immediately in next unit
      bellowUnit.addInsertCell(boxUnit.getCell("Void",2));
    }
  else
    {
      bellowUnit.addAllInsertCell(boxUnit.getCell("Void",2));
    }
  bellowUnit.setFront(pipeAUnit,"back");  
  bellowUnit.setBack(pipeBUnit,"front");
  bellowUnit.createAll(System,pipeAUnit,"back");

  return;
}
			   
  
void
balderConnectZone::buildObjects(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
    \parma FC :: Connection point
    \param sideIndex :: link piont
  */
{
  ELog::RegMethod RegA("balderConnectZone","buildObjects");

  const constructSystem::LeadPipe* leadPipePtr=
    dynamic_cast<const constructSystem::LeadPipe*>(&FC);
    

  buildZone.addInsertCells(this->getInsertCells());

  
  // offset from pipe
  pipeA->createAll(System,*this,0);  

  makeBox(System,*leadPipePtr,2,*boxA,*bellowA,*pipeA);

    // offset from pipe
  pipeB->createAll(System,*pipeA,"back");  
  
  makeBox(System,*pipeA,2,*pumpBoxA,*ionPumpA,*pipeB);

  pipeC->createAll(System,*pipeB,"back");  

  makeBox(System,*pipeB,2,*boxB,*bellowB,*pipeC);

  pipeD->createAll(System,*pipeC,"back");
  
  makeBox(System,*pipeC,2,*pumpBoxB,*ionPumpB,*pipeD);

  JPipe->createAll(System,*pipeD,"back");

  makeBox(System,*pipeD,2,*boxC,*bellowC,*JPipe);


  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);
  setCells("InnerVoid",buildZone.getCells("Unit"));
  setCell("FirstVoid",buildZone.getCell("Unit",0));
  setCell("LastVoid",buildZone.getCells("Unit").back());

  JPipe->insertInCell("Main",System,getCell("LastVoid"));

  return;
}

void
balderConnectZone::createLinks()
  /*!
    Create a front/back link
  */
{
  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*bellowC,2);
  return;
}
  
  
void 
balderConnectZone::createAll(Simulation& System,
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
  ELog::RegMethod RControl("balderConnectZone","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System,FC,sideIndex);    
  createLinks();
  insertObjects(System);
  return;
}


///\cond TEMPLATE

template void
balderConnectZone::makeBox(Simulation&,
			   const constructSystem::LeadPipe&,
			   const long int,
			   LeadBox&,
			   constructSystem::Bellows&,
			   constructSystem::LeadPipe&);

template void
balderConnectZone::makeBox(Simulation&,
			   const constructSystem::LeadPipe&,
			   const long int,
			   LeadBox&,
			   constructSystem::PipeTube&,
			   constructSystem::LeadPipe&);




///\endcond TEMPLATE

}   // NAMESPACE xraySystem

