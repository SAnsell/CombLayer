/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment45.cxx
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
#include "Line.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "BlockZone.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "YagUnitBig.h"
#include "YagScreen.h"
#include "CeramicGap.h"
#include "FlangePlate.h"
#include "EBeamStop.h"

#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Importance.h"
#include "Object.h"

#include "AttachSupport.h"
#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "InjectionHall.h"
#include "Segment45.h"


namespace tdcSystem
{

// Note currently uncopied:

Segment45::Segment45(const std::string& Key) :
  TDCsegment(Key,4),
  ceramic(new tdcSystem::CeramicGap(keyName+"Ceramic")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  yagUnit(new tdcSystem::YagUnitBig(keyName+"YagUnit")),
  yagScreen(new tdcSystem::YagScreen(keyName+"YagScreen")),
  adaptor(new constructSystem::FlangePlate(keyName+"Adaptor")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  beamStop(new tdcSystem::EBeamStop(keyName+"EBeam"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(ceramic);
  OR.addObject(pipeA);
  OR.addObject(yagUnit);
  OR.addObject(yagScreen);
  OR.addObject(adaptor);
  OR.addObject(pipeB);
  OR.addObject(pipeC);
  OR.addObject(beamStop);

  setFirstItems(ceramic);
}

Segment45::~Segment45()
  /*!
    Destructor
   */
{}

void
Segment45::populate(const FuncDataBase& Control)
  /*!
    Get required variable
    \param Control :: DatatBase
   */
{
  ELog::RegMethod RegA("Segment45","populate");

  FixedRotate::populate(Control);

  cutRadius=Control.EvalVar<double>(keyName+"CutRadius");
  return;
}


void
Segment45::setFrontSurfs(const std::vector<HeadRule>& HRvec)
  /*!
    Set the front surface if need to join
    \param HR :: Front head rule
  */
{
  ELog::RegMethod RegA("Segment45","setFrontSurfs");

  if (!HRvec.empty())
    {
      for(size_t i=0;i<firstItemVec.size();i++)
	{
	  attachSystem::ExternalCut* FPtr=firstItemVec[i];
	  if (FPtr)
	    FPtr->setCutSurf("front",HRvec.back());
	}
    }
  return;
}

void
Segment45::createSurfaces()
  /*!
    Build the surfaces for the injection floor cut volume
   */
{
  ELog::RegMethod RegA("Segment45","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,cutRadius);

  return;
}

void
Segment45::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment45","buildObjects");

  int outerCell;

  if (isActive("front"))
    ceramic->copyCutSurf("front",*this,"front");

  ceramic->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*ceramic,2);
  ceramic->insertInCell(System,outerCell);
  pipeTerminate(System,*buildZone,ceramic);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*ceramic,"back",*pipeA);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*yagUnit);


  yagScreen->setBeamAxis(*yagUnit,1);
  yagScreen->createAll(System,*yagUnit,4);
  yagScreen->insertInCell("Outer",System,outerCell);
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Plate"));
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
  yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

  constructSystem::constructUnit
    (System,*buildZone,*yagUnit,"back",*adaptor);

  constructSystem::constructUnit
    (System,*buildZone,*adaptor,"back",*pipeB);

  pipeC->setCutSurf("front",*pipeB,"back");
  pipeC->createAll(System,*pipeB, "back");

  beamStop->setCutSurf("front",*pipeC,"back");
  beamStop->createAll(System,*pipeC, "back");

  if (beamStop->isShieldActive())
    {
      attachSystem::addToInsertControl(System,*beamStop,*pipeC,"FlangeB");
      attachSystem::addToInsertControl(System,*beamStop,*pipeC,"Main");
    }

  CellMap::addCells("Unit",buildZone->getCells("Unit"));
  return;
}

void
Segment45::constructHole(Simulation& System)
  /*!
    Construct the hole in the wall
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("Segment45","constructHole");

  if (IHall)
    {
      std::string Out;
      const HeadRule fbHR=IHall->combine("#Floor BDRoomRoof");

      Out=ModelSupport::getComposite(SMap,buildIndex," -7 " );
      makeCell("FloorVoid",System,cellIndex++,0,0.0,Out+fbHR.display());
      pipeB->insertAllInCell(System,this->getCell("FloorVoid"));
      // tip of pipeB enters the main beam dump room
      pipeB->insertInCell("Main",System,IHall->getCell("BDSPF"));
      pipeB->insertInCell("FlangeB",System,IHall->getCell("BDSPF"));

      MonteCarlo::Object *obj = System.findObject(IHall->getCell("BD"));
      obj->addIntersection(HeadRule(pipeB->getLinkString(10)));
      // same but less efficient (side surfaces also added into BD):
      //  pipeB->insertInCell("Main",System,IHall->getCell("BD"));

      pipeC->insertAllInCell(System,IHall->getCell("BDSPF"));
      beamStop->insertAllInCell(System,IHall->getCell("BDSPF"));

      Out=ModelSupport::getComposite(SMap,buildIndex," 7 " );
      IHall->insertComponent(System,"HatchSPF",Out); // concrete roof
      IHall->insertComponent(System,"BDRoofSPF",Out); // steel roof
    }
  return;
}

void
Segment45::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment45","createLinks");

  setLinkCopy(0,*ceramic,1);
  setLinkCopy(1,*pipeB,2);
  nameSideIndex(1,"buildZoneCut");
  joinItems.push_back(FixedComp::getFullRule("back"));


  return;
}

void
Segment45::writePoints() const
  /*!
    Writes out points to allow tracking through magnets
  */
{
  ELog::RegMethod RegA("Segment46","writePoints");
  const std::vector<std::shared_ptr<attachSystem::FixedComp>> Items
    (
     {ceramic,pipeA,yagUnit,pipeB}
     );
  TDCsegment::writeBasicItems(Items);
  return;
}

void
Segment45::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment45","build");

  IHall=dynamic_cast<const InjectionHall*>(&FC);

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  buildObjects(System);
  constructHole(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
