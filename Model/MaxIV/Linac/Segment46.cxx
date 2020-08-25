/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment46.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#include "OutputLog.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "InnerZone.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "Object.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "generalConstruct.h"
#include "LObjectSupport.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "generateSurf.h"

#include "VacuumPipe.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "BlankTube.h"
#include "CleaningMagnet.h"
#include "PortTube.h"
#include "JawFlange.h"
#include "portItem.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "CylGateValve.h"
#include "CrossWayTube.h"



#include "Surface.h"

#include "TDCsegment.h"
#include "Segment46.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment46::Segment46(const std::string& Key) :
  TDCsegment(Key,4),
  IZThin(new attachSystem::InnerZone(*this,cellIndex)),
  
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  gateA(new xraySystem::CylGateValve(keyName+"GateA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  prismaChamber(new constructSystem::BlankTube(keyName+"PrismaChamber")),
  mirrorChamberA(new tdcSystem::CrossWayTube(keyName+"MirrorChamberA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cleaningMag(new tdcSystem::CleaningMagnet(keyName+"CleaningMagnet")),
  slitTube(new constructSystem::PortTube(keyName+"SlitTube")),
  jaws({
	std::make_shared<constructSystem::JawFlange>(keyName+"SlitTubeJawUnit0"),
	std::make_shared<constructSystem::JawFlange>(keyName+"SlitTubeJawUnit1")
    }),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  mirrorChamberB(new tdcSystem::CrossWayTube(keyName+"MirrorChamberB")),
  bellowC(new constructSystem::Bellows(keyName+"BellowC")),
  gateB(new xraySystem::CylGateValve(keyName+"GateB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(gateA);
  OR.addObject(bellowA);
  OR.addObject(prismaChamber);
  OR.addObject(mirrorChamberA);
  OR.addObject(pipeB);
  OR.addObject(cleaningMag);
  OR.addObject(slitTube);
  OR.addObject(bellowB);
  OR.addObject(mirrorChamberB);
  OR.addObject(bellowC);
  OR.addObject(gateB);

  setFirstItems(pipeA);
}

Segment46::~Segment46()
  /*!
    Destructor
   */
{}

void
Segment46::insertPrevSegment(Simulation& System,
			     const TDCsegment* prevSegPtr) const
  /*!
    Insert components that need to be in previous
    objects
   */
{
  ELog::RegMethod RegA("Segment46","insertPrevSegment");

  if (prevSegPtr && prevSegPtr->hasCell("LastCell",0))
    pipeA->insertInCell(System,prevSegPtr->getCell("LastCell",0));

  return;
}
  
  
void
Segment46::createSplitInnerZone(Simulation& System)
  /*!
    Split the innerZone into two parts (assuming segment44 built)
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("Segment46","createSplitInnerZone");

  *IZThin = *buildZone;

  if (!sideVec.empty())
    {
      const TDCsegment* sideSegment=sideVec.front();

      const Geometry::Vec3D cutOrg=sideSegment->getLinkPt(5);
      const Geometry::Vec3D cutAxis=sideSegment->getLinkAxis(5);

      const Geometry::Vec3D zAxis=X*cutAxis;
      ModelSupport::buildPlane(SMap,buildIndex+5005,cutOrg,zAxis);

      int SNremoved(0);
      for(const TDCsegment* sidePtr : sideVec)
	{
	  std::vector<int> CNvec=sidePtr->getCells("BuildVoid");
	  // need only the last cell for SPF44
	  if (sidePtr->getKeyName()=="SPF44")
	    CNvec.erase(CNvec.begin(),CNvec.end()-1);

	  for(const int CN : CNvec)
	    {
	      MonteCarlo::Object* OPtr=System.findObject(CN);
	      // remove surface that tracks close to a beam going in the +Z
	      // direction
	      HeadRule HA=OPtr->getHeadRule();   // copy
	      SNremoved=HA.removeOuterPlane(Origin+Y*10.0,Z,0.9);
	      HA.addIntersection(-SMap.realSurf(buildIndex+5005));
	      OPtr->procHeadRule(HA);
	    }
	}
      HeadRule HSurroundB=buildZone->getSurround();
      HSurroundB.removeOuterPlane(Origin+Y*10.0,-Z,0.9);
      HSurroundB.addIntersection(SMap.realSurf(buildIndex+5005));
      
      IZThin->setSurround(HSurroundB);
      IZThin->setInsertCells(buildZone->getInsertCell());
    }
  return;
}

void
Segment46::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment46","buildObjects");

  int outerCell;
  MonteCarlo::Object* masterCell=buildZone->getMaster();

  if (!masterCell)
    masterCell=IZThin->constructMasterCell(System);
  if (isActive("front"))
    pipeA->copyCutSurf("front",*this,"front");

  pipeA->createAll(System,*this,0);
  outerCell=IZThin->createOuterVoidUnit(System,masterCell,*pipeA,2);
  pipeA->insertInCell(System,outerCell);
  
  constructSystem::constructUnit
    (System,*IZThin,masterCell,*pipeA,"back",*gateA);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,*gateA,"back",*bellowA);

  const constructSystem::portItem& PC =
    buildIonPump2Port(System,*IZThin,masterCell,*bellowA,"back",*prismaChamber);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,PC,"OuterPlate",*mirrorChamberA);
  


  pipeB->createAll(System,*mirrorChamberA,"back");
  pipeMagUnit(System,*IZThin,pipeB,"#front","outerPipe",cleaningMag);
  pipeTerminate(System,*IZThin,pipeB);

  // Slit tube and jaws
  slitTube->addAllInsertCell(masterCell->getName());
  slitTube->setFront(*pipeB,"back");
  slitTube->createAll(System,*pipeB,"back");
  outerCell=IZThin->createOuterVoidUnit(System,masterCell,*slitTube,2);
  slitTube->insertAllInCell(System,outerCell);

  for(size_t index=0;index<2;index++)
    {
      const constructSystem::portItem& DPI=slitTube->getPort(index);
      jaws[index]->setFillRadius
	(DPI,DPI.getSideIndex("InnerRadius"),DPI.getCell("Void"));

      jaws[index]->addInsertCell(slitTube->getCell("Void"));
      if (index)
	jaws[index]->addInsertCell(jaws[index-1]->getCell("Void"));
      jaws[index]->createAll
	(System,DPI,DPI.getSideIndex("InnerPlate"),*slitTube,0);
    }

  // simplify the DiagnosticBox inner cell
  slitTube->splitVoidPorts(System,"SplitOuter",2001,
			  slitTube->getCell("Void"),{0,2});
  //////////////////////////////////////////////////////////////////////

  constructSystem::constructUnit
    (System,*IZThin,masterCell,*slitTube,"back",*bellowB);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,*bellowB,"back",*mirrorChamberB);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,*mirrorChamberB,"back",*bellowC);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,*bellowC,"back",*gateB);

  IZThin->removeLastMaster(System);

  return;
}

void
Segment46::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment46","createLinks");

  setLinkSignedCopy(0,*pipeA,1);
  setLinkSignedCopy(1,*gateB,2);

  FixedComp::setConnect(2,Origin,Y);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+5005));
  FixedComp::nameSideIndex(2,"buildZoneCut");

  joinItems.push_back(FixedComp::getFullRule(2));

  return;
}

void
Segment46::writePoints() const
  /*!
    Writes out points to allow tracking through magnets
  */
{
  ELog::RegMethod RegA("Segment46","writePoints");


  const Geometry::Vec3D Org(pipeA->getLinkPt(1)*10.0);
  const Geometry::Vec3D ptPipeA(pipeA->getLinkPt(2)*10.0);
  const Geometry::Vec3D ptGateA(gateA->getLinkPt(2)*10.0);
  const Geometry::Vec3D ptMirrorChamberA(mirrorChamberA->getLinkPt(2)*10.0);
  const Geometry::Vec3D ptPipeB(pipeB->getLinkPt(2)*10.0);
  const Geometry::Vec3D ptSlitTube(slitTube->getLinkPt(2)*10.0);
  const Geometry::Vec3D ptMirrorChamberB(mirrorChamberB->getLinkPt(2)*10.0);
  const Geometry::Vec3D ptBellowC(bellowC->getLinkPt(2)*10.0);
  const Geometry::Vec3D ptGateB(gateB->getLinkPt(2)*10.0);

  ELog::EM<<"Orig           = "<<Org<<ELog::endDiag;
  ELog::EM<<"Pipe A         = "<<ptPipeA-Org<<ELog::endDiag;
  ELog::EM<<"Gate A         = "<<ptGateA-Org<<ELog::endDiag;
  ELog::EM<<"MirrorChamberA = "<<ptMirrorChamberA-Org<<ELog::endDiag;
  ELog::EM<<"Pipe B         = "<<ptPipeB-Org<<ELog::endDiag;
  ELog::EM<<"slittube       = "<<ptSlitTube-Org<<ELog::endDiag;
  ELog::EM<<"MirrorChamberB = "<<ptMirrorChamberB-Org<<ELog::endDiag;
  ELog::EM<<"BellowC        = "<<ptBellowC-Org<<ELog::endDiag;
  ELog::EM<<"Gate B         = "<<ptGateB-Org<<ELog::endDiag;
  
  return;
}

void
Segment46::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment46","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSplitInnerZone(System);
  buildObjects(System);
  createLinks();
  writePoints();
  return;
}


}   // NAMESPACE tdcSystem
