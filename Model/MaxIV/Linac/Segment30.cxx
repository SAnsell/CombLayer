/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment30.cxx
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BlockZone.h"
#include "generalConstruct.h"
#include "generateSurf.h"

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumPipe.h"
#include "CorrectorMag.h"
#include "GaugeTube.h"
#include "IonPumpTube.h"
#include "LocalShielding.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment30.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment30::Segment30(const std::string& Key) :
  TDCsegment(Key,2),
  IZThin(new attachSystem::BlockZone(keyName+"IZThin")),
  gauge(new xraySystem::GaugeTube(keyName+"Gauge")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),
  bellow(new constructSystem::Bellows(keyName+"Bellow")),
  ionPump(new xraySystem::IonPumpTube(keyName+"IonPump")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"CMagVA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(gauge);
  OR.addObject(pipeA);
  OR.addObject(shieldA);
  OR.addObject(bellow);
  OR.addObject(ionPump);
  OR.addObject(pipeB);
  OR.addObject(cMagVA);

  setFirstItems(gauge);
}

Segment30::~Segment30()
  /*!
    Destructor
   */
{}

void
Segment30::createSplitInnerZone(Simulation& System)
  /*!
    Split the innerZone into two parts (assuming segment13 built)
    \param System :: Simulatio to use
   */
{
  ELog::RegMethod RegA("Segment30","createSplitInnerZone");

  const double orgFrac(2.3);
  const double axisFrac(4.0);

  *IZThin = *buildZone;
  if (!sideVec.empty())
    {
      const TDCsegment* sideSegment=sideVec.front();
      
      const Geometry::Vec3D sideOrg(sideSegment->getCentre());
      const Geometry::Vec3D sideY((sideSegment->getY()+Y*axisFrac).unit());

      const Geometry::Vec3D midX=(sideY*Z);

      ModelSupport::buildPlane(SMap,buildIndex+5005,
			       (sideOrg+Origin*orgFrac)/(orgFrac+1.0),midX);

      if (sideSegment->getKeyName()=="L2SPF13")
	{
	  int SNremoved(0);
	  for(const TDCsegment* sidePtr : sideVec)
	    {
	      ELog::EM<<"SidePtr = "<<sidePtr->getKeyName()<<ELog::endDiag;
	      if (sidePtr->getKeyName()!="TDC15")
		{
		  for(const int CN : sidePtr->getCells("Unit"))
		    {
		      MonteCarlo::Object* OPtr=System.findObject(CN);
		      HeadRule HA=OPtr->getHeadRule();   // copy
		      SNremoved=HA.removeOuterPlane(Origin+Y*10.0,-X,0.9);
		      HA.addIntersection(SMap.realSurf(buildIndex+5005));
		      OPtr->procHeadRule(HA);
		    }
		}
	    }
	  HeadRule HSurroundB=buildZone->getSurround();
	  HSurroundB.removeOuterPlane(Origin,X,0.9);
	  HSurroundB.addIntersection(-SMap.realSurf(buildIndex+5005));
	  IZThin->setSurround(HSurroundB);
	}
    }
  return;
}

void
Segment30::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment30","buildObjects");

  int outerCell;


  if (isActive("front"))
    gauge->copyCutSurf("front", *this, "front");
  gauge->createAll(System,*this,0);
  outerCell=IZThin->createUnit(System,*gauge,2);
  gauge->insertInCell(System,outerCell);

  pipeA->createAll(System,*gauge,"back");
  pipeMagUnit(System,*IZThin,pipeA,"#front","outerPipe",shieldA);
  pipeTerminate(System,*IZThin,pipeA);

  constructSystem::constructUnit
    (System,*IZThin,*pipeA,"back",*bellow);

  constructSystem::constructUnit
    (System,*IZThin,*bellow,"back",*ionPump);


  pipeB->createAll(System,*ionPump,"back");
  pipeMagUnit(System,*IZThin,pipeB,"#front","outerPipe",cMagVA);
  pipeTerminate(System,*IZThin,pipeB);

  return;
}

void
Segment30::postBuild(Simulation& System)
/*!
    Add additonal stuff after building based on
    relative segments
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment46","postBuild");

  typedef std::map<std::string,const TDCsegment*> mapTYPE;
  if (!sideVec.empty())
    {
      mapTYPE segNames;
      for(const TDCsegment* sidePtr : sideVec)
	{
	  segNames.emplace(sidePtr->getKeyName(),sidePtr);
	  ELog::EM<<"Segment == "<<sidePtr->getKeyName()<<ELog::endDiag;
	}

      HeadRule surHR=buildZone->getSurround();
      surHR.removeOuterPlane(Origin+Y*10.0,-X,0.9);
      surHR.addIntersection(SMap.realSurf(buildIndex+5005));

      if (segNames.find("L2SPF13")!=segNames.end() &&
	  segNames.find("TDC14")==segNames.end())
	{
	  mapTYPE::const_iterator mc=segNames.find("L2SPF13");
	  const TDCsegment* sideSegment=mc->second;
	  const HeadRule frontHR=sideSegment->getFullRule("back");
	  const HeadRule backHR=getFullRule("back");
	  surHR *= frontHR * backHR.complement();
	}
      if (segNames.find("L2SPF13")!=segNames.end() &&
	  segNames.find("TDC14")!=segNames.end())
	{
	  mapTYPE::const_iterator mc=segNames.find("TDC14");
	  const TDCsegment* sideSegment=mc->second;
	  const HeadRule frontHR=sideSegment->getFullRule("back");
	  const HeadRule backHR=getFullRule("back");
	  surHR *= frontHR * backHR.complement();
	}
      if (segNames.find("TDC15")!=segNames.end())
	{
	  mapTYPE::const_iterator mc=segNames.find("TDC15");
	  const TDCsegment* sideSegment=mc->second;
	  HeadRule SRX=sideSegment->getSurround();
	  const int SN=SRX.findAxisPlane(X,0.9);
	  surHR.addIntersection(-SMap.realSurf(SN));
	}

      makeCell("ExtraVoid",System,cellIndex++,0,0.0,surHR.display());
    }
  return;

}

void
Segment30::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment30","createLinks");

  setLinkCopy(0,*gauge,1);
  setLinkCopy(1,*pipeB,2);

  joinItems.push_back(FixedComp::getFullRule(2));

  buildZone->setBack(FixedComp::getFullRule("back"));
  return;
}

void
Segment30::registerPrevSeg(const TDCsegment* PSPtr,
			   const size_t indexPoint)
  /*!
   Process previous segments [Used second join Item]
   This segment is register in previous segment by: joinItems
   It is set in createLinks. Manditory to set at least 1.
   It is captured in the next segment by
   TDCsegment::setFrontSurfaces -- it used firstItemVec
   which is set in segment constructor.
   \param PSPtr :: previous segment
  */
{
  ELog::RegMethod RegA("TDCsegment","processPrevSeg");

  prevSegPtr=PSPtr;
  if (prevSegPtr)
    {
      const std::vector<HeadRule>& prevJoinItems=
	prevSegPtr->getJoinItems();
      if (buildZone && indexPoint && indexPoint<=prevJoinItems.size())
	buildZone->setFront(prevJoinItems[indexPoint-1]);
      this->setFrontSurfs(prevJoinItems);
    }
  return;
}

void
Segment30::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment30","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSplitInnerZone(System);
  buildObjects(System);
  createLinks();
  postBuild(System);

  return;
}


}   // NAMESPACE tdcSystem
