/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment30.cxx
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
#include "InnerZone.h"
#include "generalConstruct.h"
#include "generateSurf.h"

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "VacuumPipe.h"
#include "portItem.h"
#include "BlankTube.h"
#include "CorrectorMag.h"
#include "LObjectSupport.h"

#include "TDCsegment.h"
#include "Segment30.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment30::Segment30(const std::string& Key) :
  TDCsegment(Key,2),
  IZThin(new attachSystem::InnerZone(*this,cellIndex)),
  gauge(new constructSystem::PipeTube(keyName+"Gauge")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bellow(new constructSystem::Bellows(keyName+"Bellow")),
  ionPump(new constructSystem::BlankTube(keyName+"IonPump")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagV(new tdcSystem::CorrectorMag(keyName+"CMagV"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(gauge);
  OR.addObject(pipeA);
  OR.addObject(bellow);
  OR.addObject(ionPump);
  OR.addObject(pipeB);
  OR.addObject(cMagV);

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

  *IZThin = *buildZone;

  const double orgFrac(2.3);
  const double axisFrac(4.0);
  if (!sideVec.empty())
    {
      const TDCsegment* sideSegment=sideVec.front();

      const Geometry::Vec3D sideOrg(sideSegment->getCentre());
      const Geometry::Vec3D sideY((sideSegment->getY()+Y*axisFrac).unit());
      
      const Geometry::Vec3D midX=(sideY*Z);
            
      ModelSupport::buildPlane(SMap,buildIndex+5005,
			       (sideOrg+Origin*orgFrac)/(orgFrac+1.0),midX);

      int SNremoved(0);
      for(const TDCsegment* sidePtr : sideVec)
	{
	  for(const int CN : sidePtr->getCells("BuildVoid"))
	    {
	      MonteCarlo::Object* OPtr=System.findObject(CN);
	      HeadRule HA=OPtr->getHeadRule();   // copy
	      SNremoved=HA.removeOuterPlane(Origin+Y*10.0,-X,0.9);
	      HA.addIntersection(SMap.realSurf(buildIndex+5005));
	      OPtr->procHeadRule(HA);
	    }
	}
      if (sideVec.size()>=2)
	{
	  HeadRule TriCut=buildZone->getSurround();
	  TriCut.removeOuterPlane(Origin,X,0.9);
	  TriCut.removeOuterPlane(Origin,-X,0.9);
	  TriCut*=sideVec[1]->getFullRule(-2);
	  TriCut.addIntersection(-SNremoved);
	  TriCut.addIntersection(SMap.realSurf(buildIndex+5005));
	  for(const int CN : buildZone->getInsertCell())
	    {
	      MonteCarlo::Object* outerObj=System.findObject(CN);
	      if (outerObj)
		outerObj->addIntersection(TriCut.complement());
	    }
	}
      HeadRule HSurroundB=buildZone->getSurround();
      HSurroundB.removeOuterPlane(Origin,X,0.9);
      HSurroundB.addIntersection(-SMap.realSurf(buildIndex+5005));
      IZThin->setSurround(HSurroundB);
      IZThin->setInsertCells(buildZone->getInsertCell());
      IZThin->constructMasterCell(System);
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
  MonteCarlo::Object* masterCell=IZThin->getMaster();
  if (!masterCell)
    {
      ELog::EM<<"Building master cell"<<ELog::endDiag;
      masterCell=IZThin->constructMasterCell(System);
    }

  gauge->addAllInsertCell(masterCell->getName());
  // Gauge
  if (isActive("front"))
    gauge->copyCutSurf("front", *this, "front");
  gauge->createAll(System,*this,0);
  outerCell=IZThin->createOuterVoidUnit(System,masterCell,*gauge,2);
  gauge->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,*gauge,"back",*pipeA);
  
  constructSystem::constructUnit
    (System,*IZThin,masterCell,*pipeA,"back",*bellow);


  const constructSystem::portItem& ionPumpBackPort =
    buildIonPump2Port(System,*IZThin,masterCell,*bellow,"back",*ionPump);


  pipeB->createAll(System,ionPumpBackPort,"OuterPlate");
  pipeMagUnit(System,*IZThin,pipeB,"#front","outerPipe",cMagV);
  pipeTerminate(System,*IZThin,pipeB);

  IZThin->removeLastMaster(System);
  return;

  return;
}

void
Segment30::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment30","createLinks");

  setLinkSignedCopy(0,*gauge,1);
  setLinkSignedCopy(1,*pipeB,2);

  joinItems.push_back(FixedComp::getFullRule(2));

  return;
}

void
Segment30::regiseterPrevSeg(const TDCsegment* PSPtr)
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
      if (prevJoinItems.size()>1)
	{
	  if (buildZone)
	    buildZone->setFront(prevJoinItems[1]);
	  this->setFrontSurfs(prevJoinItems);
	}
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
  return;
}


}   // NAMESPACE tdcSystem
