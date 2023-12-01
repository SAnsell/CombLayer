/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimax/M1Detail.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PointMap.h"

#include "M1Mirror.h"
#include "M1BackPlate.h"
#include "M1FrontShield.h"
#include "M1Connectors.h"
#include "M1ElectronShield.h"
#include "M1Frame.h"
#include "M1Ring.h"
#include "M1Pipe.h"
#include "M1Detail.h"

#include "Importance.h"
#include "Object.h"

namespace xraySystem
{

M1Detail::M1Detail(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  mirror(new M1Mirror(keyName+"Mirror")),
  cClamp(new M1BackPlate(keyName+"CClamp")),
  connectors(new M1Connectors(keyName+"Connect")),
  frontShield(new M1FrontShield(keyName+"FPlate")),
  elecShield(new M1ElectronShield(keyName+"ElectronShield")),
  mainPipe(new M1Pipe(keyName+"MainPipe")),
  ringA(new M1Ring(keyName+"RingA")),
  ringB(new M1Ring(keyName+"RingB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(mirror);
  OR.addObject(cClamp);
  OR.addObject(connectors);
  OR.addObject(frontShield);
  OR.addObject(elecShield);
  OR.addObject(mainPipe);
  OR.addObject(ringA);
  OR.addObject(ringB);
}

M1Detail::~M1Detail()
  /*!
    Destructor
   */
{}

void
M1Detail::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1Detail","populate");

  FixedRotate::populate(Control);

  return;
}


void
M1Detail::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1Detail","createSurfaces");

  if (!isActive("TubeRadius"))
    {
      ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,20.0);
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*100.0,Y);
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*100.0,Y);
    }
  return; 
}

void
M1Detail::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1Detail","createObjects");

  if (!isActive("TubeRadius"))
    {
      HeadRule HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7");
      makeCell("MainVoid",System,cellIndex++,0,0.0,HR);
      setCutSurf("TubeRadius",HeadRule(SMap,buildIndex,-7));
      addOuterSurf(HR);
    }
  else
    setCells("MainVoid",getInsertCells());

  mirror->createAll(System,*this,0);

  cClamp->copyCutSurf("TubeRadius",*this,"TubeRadius");

  cClamp->setCutSurf("FarEnd",*mirror,"back");
  cClamp->setCutSurf("NearEnd",*mirror,"front");
  cClamp->setCutSurf("Back",*mirror,"outSide");
  cClamp->setCutSurf("Mirror",*mirror,"mirrorSide");
  cClamp->setCutSurf("Top",*mirror,"top");
  cClamp->setCutSurf("Base",*mirror,"base");
  cClamp->createAll(System,*mirror,"backPlateOrg");

  // connects join the mirror to the back plane
  connectors->setCell("slotA",mirror->getCell("Slot",0));
  connectors->setCell("slotB",mirror->getCell("Slot",1));
  
  connectors->setCell("gapA",cClamp->getCell("PlateGap",1));
  connectors->setCell("gapB",cClamp->getCell("PlateGap",0));
  
  connectors->setCutSurf("slotBase",*mirror,"slotBase");
  connectors->setCutSurf("slotAMid",*mirror,"slotAMid");
  connectors->setCutSurf("slotBMid",*mirror,"slotBMid");
  connectors->setCutSurf("MTop",*mirror,"top");
  connectors->setCutSurf("MBase",*mirror,"base");

  connectors->setCutSurf("CInnerTop",*cClamp,"innerTop");
  connectors->setCutSurf("CInnerBase",*cClamp,"innerBase");
  
  connectors->createAll(System,*mirror,"backPlateOrg");
  frontShield->addInsertCell("Main",getCells("MainVoid"));
  if (CellMap::hasCell("FrontVoid"))
    frontShield->addInsertCell("Extra",getCell("FrontVoid"));
  else
    frontShield->addInsertCell("Extra",getCell("MainVoid"));
  frontShield->setCutSurf("Front",*cClamp,"front");
  frontShield->setCutSurf("Base",*cClamp,"innerSide");
  frontShield->createAll(System,*cClamp,"front");


  ringA->copyCutSurf("TubeRadius",*this,"TubeRadius");
  ringA->addInsertCell(getCells("MainVoid"));
  ringA->addInsertCell(cClamp->getCell("OuterVoid",0));
  ringA->addInsertCell(cClamp->getCell("OuterVoid",1));
  ringA->addInsertCell(cClamp->getCell("BackVoid"));
  ringA->createAll(System,*mirror,0);
  cClamp->joinRing(System,ringA->getRule("RingGap"),
		   ringA->getFullRule("innerRing"));

  ringB->copyCutSurf("TubeRadius",*this,"TubeRadius");
  ringB->addInsertCell(getCells("MainVoid"));
  ringB->addInsertCell(cClamp->getCell("OuterVoid",0));
  ringB->addInsertCell(cClamp->getCell("OuterVoid",1));
  ringB->addInsertCell(cClamp->getCell("BackVoid"));
  ringB->createAll(System,*mirror,0);
  cClamp->joinRing(System,ringB->getRule("RingGap"),
		   ringB->getFullRule("innerRing"));

  elecShield->setCell("TopVoid",*cClamp,"InnerVoid",0);
  elecShield->setCell("BaseVoid",*cClamp,"InnerVoid",1);
  elecShield->setCell("BaseEndVoid",*cClamp,"HeatVoid",2);
  elecShield->setCell("TopEndVoid",*cClamp,"HeatVoid",3);

  elecShield->copyCutSurf("TubeRadius",*this,"TubeRadius");
  elecShield->setCutSurf("Mirror",*mirror,"mirrorSide");

  elecShield->setCutSurf("ringAFront",*ringA,"front");
  elecShield->setCutSurf("ringACyl",*ringA,"innerRing");
  elecShield->setCutSurf("ringABack",*ringA,"back");
  elecShield->setCutSurf("ringBFront",*ringB,"front");
  elecShield->setCutSurf("ringBCyl",*ringB,"innerRing");
  elecShield->setCutSurf("ringBBack",*ringB,"back");
  

  elecShield->addInsertCell(getCells("MainVoid"));
  elecShield->createAll(System,*mirror,0);

  
  ELog::EM<<"MPipe A == "<<mirror->getLinkPt("downPipeA")<<ELog::endDiag;
  ELog::EM<<"MPipe B == "<<mirror->getLinkPt("downPipeB")<<ELog::endDiag;
  mainPipe->createAll(System,*mirror,"downPipeA");

  
  cClamp->insertInCell(System,getCells("MainVoid"));

  return;
}

void
M1Detail::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1Detail","createLinks");

  
  return;
}

void
M1Detail::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
  */
{
  ELog::RegMethod RegA("M1Detail","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}

}  // NAMESPACE xraySystem
