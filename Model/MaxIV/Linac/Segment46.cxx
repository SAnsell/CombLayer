/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment46.cxx
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
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BlockZone.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "Importance.h"
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
#include "CleaningMagnet.h"
#include "PortTube.h"
#include "JawFlange.h"
#include "portItem.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "CylGateValve.h"
#include "CrossWayTube.h"
#include "PrismaChamber.h"
#include "LocalShielding.h"

#include "TDCsegment.h"
#include "Segment46.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment46::Segment46(const std::string& Key) :
  TDCsegment(Key,4),
  IZThin(new attachSystem::BlockZone(keyName+"IZThin")),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),
  gateA(new xraySystem::CylGateValve(keyName+"GateA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  prismaChamber(new tdcSystem::PrismaChamber(keyName+"PrismaChamber")),
  mirrorChamberA(new xraySystem::CrossWayTube(keyName+"MirrorChamberA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cleaningMag(new tdcSystem::CleaningMagnet(keyName+"CleaningMagnet")),
  slitTube(new constructSystem::PortTube(keyName+"SlitTube")),
  jaws({
	std::make_shared<constructSystem::JawFlange>(keyName+"SlitTubeJawUnit0"),
	std::make_shared<constructSystem::JawFlange>(keyName+"SlitTubeJawUnit1")
    }),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  mirrorChamberB(new xraySystem::CrossWayTube(keyName+"MirrorChamberB")),
  bellowC(new constructSystem::Bellows(keyName+"BellowC")),
  gateB(new xraySystem::CylGateValve(keyName+"GateB")),
  bellowD(new constructSystem::Bellows(keyName+"BellowD"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(shieldA);
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
  OR.addObject(bellowD);

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
    pipeA->insertAllInCell(System,prevSegPtr->getCell("LastCell",0));

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

  *IZThin=*buildZone;
  if (!sideVec.empty())
    {
      const TDCsegment* sideSegment=sideVec.front();

      const Geometry::Vec3D cutOrg=sideSegment->getLinkPt(5)+Z*1.0;
      const Geometry::Vec3D cutAxis=sideSegment->getLinkAxis(5);
      const Geometry::Vec3D zAxis=X*cutAxis.unit();

      ModelSupport::buildPlane(SMap,buildIndex+5005,cutOrg,zAxis);

      for(const TDCsegment* sidePtr : sideVec)
	{
	  std::vector<int> CNvec;

	  // need only the last cell for SPF44
	  if (sidePtr->getKeyName()=="SPF44")
	    CNvec=sidePtr->getCells("OverLap");

	  // need only the last cell for SPF45
	  if (sidePtr->getKeyName()=="SPF45")
	    CNvec=sidePtr->getCells("Unit");

	  for(const int CN : CNvec)
	    {
	      MonteCarlo::Object* OPtr=System.findObject(CN);
	      // remove surface that tracks close to a beam going in the +Z
	      // direction
	      HeadRule HA=OPtr->getHeadRule();   // copy
	      HA.removeOuterPlane(Origin+Y*10.0,Z,0.9);
	      HA.addIntersection(-SMap.realSurf(buildIndex+5005));
	      OPtr->procHeadRule(HA);
	    }
	}
      HeadRule HSurroundB=buildZone->getSurround();
      HSurroundB.removeOuterPlane(Origin+Y*10.0,-Z,0.9);
      HSurroundB.addIntersection(SMap.realSurf(buildIndex+5005));

      IZThin->setSurround(HSurroundB);
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

  if (isActive("front"))
    pipeA->copyCutSurf("front",*this,"front");

  pipeA->createAll(System,*this,0);

  pipeMagUnit(System,*IZThin,pipeA,"#front","outerPipe",shieldA);
  pipeTerminate(System,*IZThin,pipeA);

  constructSystem::constructUnit
    (System,*IZThin,*pipeA,"back",*gateA);

  constructSystem::constructUnit
    (System,*IZThin,*gateA,"back",*bellowA);

  constructSystem::constructUnit
    (System,*IZThin,*bellowA,"back",*prismaChamber);

  constructSystem::constructUnit
    (System,*IZThin,*prismaChamber,"back",*mirrorChamberA);

  pipeB->createAll(System,*mirrorChamberA,"back");
  pipeMagUnit(System,*IZThin,pipeB,"#front","outerPipe",cleaningMag);
  pipeTerminate(System,*IZThin,pipeB);

  // Slit tube and jaws

  constructSystem::constructUnit
    (System,*IZThin,*pipeB,"back",*slitTube);

  for(size_t index=0;index<2;index++)
    {
      const constructSystem::portItem& DPI=slitTube->getPort(index);
      jaws[index]->setFillRadius(DPI,"InnerRadius",DPI.getCell("Void"));

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
      (System,*IZThin,*slitTube,"back",*bellowB);


  constructSystem::constructUnit
    (System,*IZThin,*bellowB,"back",*mirrorChamberB);

  constructSystem::constructUnit
    (System,*IZThin,*mirrorChamberB,"back",*bellowC);

  constructSystem::constructUnit
    (System,*IZThin,*bellowC,"back",*gateB);

  constructSystem::constructUnit
    (System,*IZThin,*gateB,"back",*bellowD);
  return;
}

void
Segment46::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment46","createLinks");

  setLinkCopy(0,*pipeA,1);
  setLinkCopy(1,*bellowD,2);

  FixedComp::setConnect(2,Origin,Y);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+5005));
  FixedComp::nameSideIndex(2,"buildZoneCut");

  joinItems.push_back(FixedComp::getFullRule(2));

  buildZone->setBack(bellowD->getFullRule("back"));

  return;
}

void
Segment46::writePoints() const
  /*!
    Writes out points to allow tracking through magnets
  */
{
  ELog::RegMethod RegA("Segment46","writePoints");

  const std::vector<std::shared_ptr<attachSystem::FixedComp>> Items
    (
     {pipeA,gateA,prismaChamber,mirrorChamberA,pipeB,slitTube,bellowB,
      mirrorChamberB,bellowC,gateB}
     );

  TDCsegment::writeBasicItems(Items);

  return;
}

void
Segment46::postBuild(Simulation& System)
  /*!
    Add additonal stuff after building based on
    relative segments
  */
{
  ELog::RegMethod RegA("Segment46","postBuild");

  typedef std::map<std::string,const TDCsegment*> mapTYPE;

  if (!sideVec.empty())
    {
      mapTYPE segNames;
      for(const TDCsegment* sidePtr : sideVec)
	segNames.emplace(sidePtr->getKeyName(),sidePtr);

      if (segNames.find("SPF44")!=segNames.end() &&
	  segNames.find("SPF45")==segNames.end())
	{
	  mapTYPE::const_iterator mc=segNames.find("SPF44");
	  const TDCsegment* sideSegment=mc->second;
	  HeadRule surHR=buildZone->getSurround();
	  const HeadRule frontHR=sideSegment->getFullRule("magnetExit");
	  const HeadRule backHR=IZThin->getBack();
	  surHR.removeOuterPlane(Origin+Y*10.0,Z,0.9);
	  surHR.addIntersection(-SMap.realSurf(buildIndex+5005));
	  surHR *= frontHR * backHR.complement();

	  makeCell("ExtraVoid",System,cellIndex++,0,0.0,surHR.display());
	}
      if (segNames.find("SPF44")!=segNames.end() &&
	  segNames.find("SPF45")!=segNames.end())
	{

	  mapTYPE::const_iterator mc=segNames.find("SPF45");
	  const TDCsegment* sideSegment=mc->second;
	  const HeadRule frontHR=IZThin->getBack();
	  const HeadRule backHR=sideSegment->FixedComp::getFullRule("#back");
	  HeadRule surHR=buildZone->getSurround();
	  surHR.removeOuterPlane(Origin+Y*10.0,-Z,0.9);
	  surHR.addIntersection(SMap.realSurf(buildIndex+5005));
	  surHR *= frontHR * backHR;
	  makeCell("SpaceFiller",System,cellIndex++,0,0.0,surHR.display());
	}
    }
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
  postBuild(System);
  createLinks();
  //  writePoints();
  return;
}


}   // NAMESPACE tdcSystem
