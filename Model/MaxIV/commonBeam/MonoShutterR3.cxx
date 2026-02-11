/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MonoShutterR3.cxx
 *
 * Copyright (c) 2026 by Udo Friman-Gayer
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
#include "objectRegister.h"
#include "Vec3D.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "FixedGroup.h"
#include "FixedRotateGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "ShutterUnit.h"
#include "MonoShutterR3.h"

namespace xraySystem
{

MonoShutterR3::MonoShutterR3(const std::string& Key) :
  attachSystem::FixedRotate(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::SurfMap(),
  attachSystem::CellMap(),
  
  entryAdapter(std::make_shared<constructSystem::PipeTube>(keyName+"EntryAdapter")),
  exitAdapter(std::make_shared<constructSystem::PipeTube>(keyName+"ExitAdapter")),
  shutterPipe(std::make_shared<constructSystem::PipeTube>(keyName+"Pipe")),
  monoShutterA(std::make_shared<xraySystem::ShutterUnit>(keyName+"UnitA")),
  monoShutterB(std::make_shared<xraySystem::ShutterUnit>(keyName+"UnitB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(shutterPipe);
  OR.addObject(monoShutterA);
  OR.addObject(monoShutterB);
}

void
MonoShutterR3::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MonoShutterR3","populate");

  FixedRotate::populate(Control);

  apertureBackLength=Control.EvalVar<double>(keyName+"ApertureBackLength");
  apertureInnerRadius=Control.EvalVar<double>(keyName+"ApertureInnerRadius");
  apertureOuterRadius=Control.EvalVar<double>(keyName+"ApertureOuterRadius");
  apertureThick=Control.EvalVar<double>(keyName+"ApertureThick");
  apertureMat=ModelSupport::EvalMat<int>(Control,keyName+"ApertureMat");
  apertureToBlockGap=Control.EvalVar<double>(keyName+"ApertureToBlockGap");

  blockHeight=Control.EvalVar<double>(keyName+"BlockHeight");
  blockLength=Control.EvalVar<double>(keyName+"BlockLength");
  blockWidth=Control.EvalVar<double>(keyName+"BlockWidth");
  shutterDistance=Control.EvalVar<double>(keyName+"ShutterDistance");

  return;
}


void
MonoShutterR3::createSurfaces()
{
  ELog::RegMethod RegA("MonoShutterR3","createSurfaces");

  // Plane along the optical axis that divides the shutter into two (almost equal)
  // parts.
  const int optAxisCentralPlane = shutterPipe->getPort(1).getLinkSurf(1);

  // Upstream and downstream aperture have the nominal 4 mm distance to the shutter 
  // blocks (Fig. 2.5 in [1]).
  // For the central aperture, the distance is slightly larger by default, because 
  // it is determined by the distance of the two shutter blocks. In order to position
  // them at 4 mm distance, the blocks would need to be so close to each other that 
  // their flanges overlap.
  // In Tab. 2.3 in Ref. [1], "clearance between blocks and apertures in z-dir <= 5 mm"
  // is given, showing that the uncertainty of the 4 mm is relatively large.

  // Upstream aperture
  ModelSupport::buildShiftedPlane(
    SMap,buildIndex+1,optAxisCentralPlane,Z,
    0.5*(shutterDistance+blockLength)+apertureToBlockGap+apertureThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+2,buildIndex+1,Z,-apertureThick);

  // Central aperture
  ModelSupport::buildShiftedPlane(SMap,buildIndex+11,
    optAxisCentralPlane,Z,+0.5*apertureThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+12,
    optAxisCentralPlane,Z,-0.5*apertureThick);

  // Downstream aperture
  ModelSupport::buildShiftedPlane(
    SMap,buildIndex+21,optAxisCentralPlane,
    Z,-0.5*(blockLength+shutterDistance)-apertureToBlockGap);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+22,buildIndex+21,Z,-apertureThick);
  ModelSupport::buildShiftedPlane(
    SMap,buildIndex+32,buildIndex+21,Z,-apertureBackLength);

  ModelSupport::buildCylinder(
    SMap,buildIndex+7,shutterPipe->getPort(0).getLinkPt(0),
    shutterPipe->getPort(0).getY(),apertureInnerRadius);
  ModelSupport::buildCylinder(
    SMap,buildIndex+17,shutterPipe->getPort(0).getLinkPt(0),
    shutterPipe->getPort(0).getY(),apertureOuterRadius);

  // For simplicity, build a plane with no shift to create a new index.
  ModelSupport::buildShiftedPlane(SMap,buildIndex+101,
    optAxisCentralPlane,Y,0.0);
  ModelSupport::buildPlane(SMap,buildIndex+103,
    shutterPipe->getLinkPt(0),
    shutterPipe->getPort(2).getLinkPt(0),
    shutterPipe->getPort(2).getLinkPt(2),
    X
  );
  ModelSupport::buildShiftedPlane(SMap,buildIndex+3,buildIndex+103,X,14.0);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+4,buildIndex+103,X,-14.0);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+5,
    shutterPipe->getPort(2).getLinkSurf(1),Z,40.0);

  return; 
}

void
MonoShutterR3::createObjects(Simulation& System)
  /*!
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("MonoShutterR3","createObjects");

  makeCell("ApertureFront",System,cellIndex++,apertureMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-1 2 7 -17"));

  makeCell("ApertureCenter",System,cellIndex++,apertureMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-11 12 7 -17"));

  makeCell("ApertureBack",System,cellIndex++,apertureMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-21 22 7 -17"));

  const HeadRule apertureBackPlug = HeadRule(shutterPipe->getPort(0).getLinkSurf(3));
  makeCell("ApertureBack",System,cellIndex++,apertureMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22 32 7")
    *apertureBackPlug);

  HeadRule mainVoid = (
    shutterPipe->getSurfRules("VoidCyl")
    *shutterPipe->getSurfRules("-FlangeACapInside")
    *shutterPipe->getSurfRules("FlangeBCapInside"));

  makeCell("Void0",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"1")*mainVoid
	   );
  makeCell("Void1",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-1 2 (-7:17)")*mainVoid
	   );
  makeCell("Void2",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-2 11")*mainVoid
	   );
  makeCell("Void3",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-11 12 (-7:17)")*mainVoid
	   );
  makeCell("Void4",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-12 21")*mainVoid
	   );
  makeCell("Void5",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-21 22 (-7:17)")*mainVoid
	   );
  makeCell("Void6",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22 -7")*mainVoid
	   );
  makeCell("Void7",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22")
    *apertureBackPlug.complement()*mainVoid
	   );
  makeCell("Void8",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-7 32 -22")
    *apertureBackPlug*shutterPipe->getSurfRules("VoidCyl").complement()
	   );
  makeCell("Void9",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-32")
    *apertureBackPlug*HeadRule(shutterPipe->getPort(1).getLinkSurf(2)).complement()
	   );
  
  System.removeCell(shutterPipe->getCell("Void"));
  System.removeCell(shutterPipe->getPort(1).getCell("Void"));
  monoShutterA->insertInCell("Inner",System,getCell("Void2"));
  monoShutterB->insertInCell("Inner",System,getCell("Void4"));

  // Planes that define the outer void
  const HeadRule frontHR = entryAdapter->getFrontRule();
  const HeadRule backHR = exitAdapter->getBackRule();
  const HeadRule bottomHR = shutterPipe->getFrontRule();
  const HeadRule topHR = ModelSupport::getHeadRule(SMap,buildIndex,"-5");
  const HeadRule leftRightHR = ModelSupport::getHeadRule(SMap,buildIndex,"-3 4");
  // Planes that split the shutter into two (almost) symmetric parts along 
  // the optical axis.
  const HeadRule frontPartHR = ModelSupport::getHeadRule(SMap,buildIndex,"-101");
  const HeadRule backPartHR = ModelSupport::getHeadRule(SMap,buildIndex,"101");

  addOuterSurf(frontHR*backHR*bottomHR*topHR*leftRightHR);
  makeCell("EntryAdapterVoid",System,cellIndex++,0,0.0,
    frontHR*entryAdapter->getBackRule()*bottomHR*topHR*leftRightHR
	);
  entryAdapter->insertAllInCell(System,getCell("EntryAdapterVoid"));

  makeCell("ShutterPipeVoid",System,cellIndex++,0,0.0,
    entryAdapter->getBackRule().complement()*exitAdapter->getFrontRule().complement()
    *bottomHR*shutterPipe->getBackRule()*leftRightHR
	);
  shutterPipe->insertAllInCell(System,getCell("ShutterPipeVoid"));
  makeCell("ShutterPipeTopPortsFrontVoid",System,cellIndex++,0,0.0,
    entryAdapter->getBackRule().complement()*shutterPipe->getBackRule().complement()
    *HeadRule(-shutterPipe->getPort(2).getLinkSurf(2))*leftRightHR*frontPartHR
	);
  shutterPipe->getPort(2).insertInCell(System,getCell("ShutterPipeTopPortsFrontVoid"));
  makeCell("ShutterPipeTopPortsBackVoid",System,cellIndex++,0,0.0,
    exitAdapter->getFrontRule().complement()*shutterPipe->getBackRule().complement()
    *HeadRule(-shutterPipe->getPort(2).getLinkSurf(2))*leftRightHR*backPartHR
	);
  shutterPipe->getPort(3).insertInCell(System,getCell("ShutterPipeTopPortsBackVoid"));

  makeCell("ShutterVoidFront",System,cellIndex++,0,0.0,
    entryAdapter->getBackRule().complement()
    *HeadRule(shutterPipe->getPort(2).getLinkSurf(2))
    *topHR*leftRightHR*frontPartHR
	);
  monoShutterA->insertInCell("Outer",System,getCell("ShutterVoidFront"));
  makeCell("ShutterVoidBack",System,cellIndex++,0,0.0,
    exitAdapter->getFrontRule().complement()
    *HeadRule(shutterPipe->getPort(2).getLinkSurf(2))*topHR
    *leftRightHR*backPartHR
	);
  monoShutterB->insertInCell("Outer",System,getCell("ShutterVoidBack"));

  makeCell("ExitAdapterVoid",System,cellIndex++,0,0.0,
    exitAdapter->getFrontRule()*exitAdapter->getBackRule()*bottomHR*topHR*leftRightHR
	);
  exitAdapter->insertAllInCell(System,getCell("ExitAdapterVoid"));

  return; 
}

void
MonoShutterR3::buildComponents(Simulation& System)
  /*!
    Construct the object to be built
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("MonoShutterR3","buildComponents");

  entryAdapter->createAll(System,*this,0);

  shutterPipe->setOuterVoid();
	shutterPipe->setPortRotation(3,Geometry::Vec3D(0,0,1));

  shutterPipe->createAll(System,*entryAdapter,"back");

  exitAdapter->createAll(System,shutterPipe->getPort(1),2);

  constructSystem::portItem shutterPort = shutterPipe->getPort(2);
  monoShutterA->addInsertCell("Inner",shutterPipe->getCell("Void"));
  monoShutterA->addInsertCell("Inner",shutterPort.getCell("Void"));
  monoShutterA->createAll(System,shutterPipe->getPort(1),0,shutterPort,2);

  shutterPort = shutterPipe->getPort(3);
  monoShutterB->addInsertCell("Inner",shutterPipe->getCell("Void"));
  monoShutterB->addInsertCell("Inner",shutterPort.getCell("Void"));
  monoShutterB->createAll(System,shutterPipe->getPort(1),0,shutterPort,2);
  
  return;
}

void
MonoShutterR3::createLinks()
{
  ELog::RegMethod RControl("MonoShutterR3","createLinks");

  if(!isActive("front")){
    ExternalCut::setCutSurf("front",entryAdapter->getFrontRule());
  }
  if(!isActive("back")){
    ExternalCut::setCutSurf("back",exitAdapter->getBackRule());
  }

  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);

  return;
}

void
MonoShutterR3::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    External build everything
    \param System :: Simulation
    \param FC :: FixedComp for beam origin
    \param sideIndex :: link point of centre [and axis]
   */
{
  ELog::RegMethod RegA("MonoShutterR3","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildComponents(System);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
