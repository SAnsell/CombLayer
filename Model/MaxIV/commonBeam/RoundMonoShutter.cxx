/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/RoundMonoShutter.cxx
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
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
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
#include "FixedOffset.h"
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
#include "FourPortTube.h"

#include "ShutterUnit.h"
#include "RoundMonoShutter.h"

namespace xraySystem
{

RoundMonoShutter::RoundMonoShutter(const std::string& Key) :
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB",
			       "ShutterA","ShutterB"),
  attachSystem::FixedRotate(Key,3),
  attachSystem::ExternalCut(),
  attachSystem::SurfMap(),
  attachSystem::CellMap(),
  
  shutterPipe(new xraySystem::FourPortTube(keyName+"Pipe")),
  portA(new constructSystem::portItem(keyName+"PortA")),
  portB(new constructSystem::portItem(keyName+"PortB")),
  monoShutterA(new xraySystem::ShutterUnit(keyName+"UnitA")),
  monoShutterB(new xraySystem::ShutterUnit(keyName+"UnitB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(shutterPipe);
  OR.addObject(portA);
  OR.addObject(portB);
  OR.addObject(monoShutterA);
  OR.addObject(monoShutterB);
}

RoundMonoShutter::~RoundMonoShutter()
  /*!
    Destructor
   */
{}

void
RoundMonoShutter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("RoundMonoShutter","populate");

  FixedRotate::populate(Control);

  divideStep=Control.EvalVar<double>(keyName+"DivideStep");

  divideThick=Control.EvalVar<double>(keyName+"DivideThick");
  divideRadius=Control.EvalVar<double>(keyName+"DivideRadius");
  
  dMat=ModelSupport::EvalMat<int>(Control,keyName+"DivideMat");
  
  return;
}


void
RoundMonoShutter::createSurfaces()
  /*!
    Create planes for mid plane divider
  */
{
  ELog::RegMethod RegA("RoundMonoShutter","createSurfaces");

  Geometry::Vec3D CPoint(shutterPipe->getLinkPt(0));
  const Geometry::Vec3D beamPoint(shutterPipe->getLinkPt(1));

  
  makePlane("MidCutA",SMap,buildIndex+1,CPoint-Y*divideThick,Y);
  makePlane("MidCutB",SMap,buildIndex+2,CPoint,Y);

  makePlane("EndCutA",SMap,buildIndex+11,CPoint+Y*divideStep,Y);
  makePlane("EndCutB",SMap,buildIndex+12,
	    CPoint+Y*(divideStep+divideThick),Y);
  
  ModelSupport::buildCylinder(SMap,buildIndex+7,beamPoint,Y,divideRadius);
  return; 
}

void
RoundMonoShutter::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("RoundMonoShutter","createObjects");

  std::string Out;

  HeadRule HR;

  const HeadRule voidCyl=shutterPipe->getSurfRule("#SideCyl");
  const HeadRule baseHR=shutterPipe->getSurfRule("InnerA");
  const HeadRule topHR=shutterPipe->getSurfRule("#InnerB");

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7 ");
  makeCell("DivideAVoid",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 7 ");
  makeCell("DivideA",System,cellIndex++,dMat,0.0,HR*voidCyl*baseHR*topHR);

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 -7");
  makeCell("DivideBVoid",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 7");
  makeCell("DivideB",System,cellIndex++,dMat,0.0,HR*voidCyl*baseHR*topHR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1");
  makeCell("FrontVoid",System,cellIndex++,0,0.0,HR*voidCyl*baseHR*topHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -11");
  makeCell("MidVoid",System,cellIndex++,0,0.0,HR*voidCyl*baseHR*topHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12");
  makeCell("BackVoid",System,cellIndex++,0,0.0,HR*voidCyl*baseHR*topHR);


  const int CN=shutterPipe->getCell("Void");
  
  System.removeCell(CN);
  monoShutterA->insertInCell("Inner",System,getCell("FrontVoid"));
  monoShutterB->insertInCell("Inner",System,getCell("MidVoid"));

  return; 
}

void
RoundMonoShutter::buildComponents(Simulation& System)
  /*!
    Construct the object to be built
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("RoundMonoShutter","buildComponents");
 
  shutterPipe->addInsertCell(getCC("Main").getInsertCells());
  shutterPipe->setSideVoid();
  
  
  if (isActive("front"))
    shutterPipe->setFront(this->getRuleStr("front"));

  // need rotation : --
  shutterPipe->createAll(System,*this,0);
  MonteCarlo::Object* plate=
    shutterPipe->getCellObject(System,"RightPlate");
  const HeadRule innerHR=shutterPipe->getSurfRule("InnerB");
  const HeadRule outerHR=shutterPipe->getSurfRule("OuterB");

  portA->createAll(System,*shutterPipe,0);
  portA->constructTrack(System,plate,innerHR,outerHR);
  portA->insertInCell(System,getCC("Main").getInsertCells());

  portB->createAll(System,*shutterPipe,0);
  portB->constructTrack(System,plate,innerHR,outerHR);
  portB->insertInCell(System,getCC("Main").getInsertCells());

  monoShutterA->addInsertCell("Inner",shutterPipe->getCell("Void"));
  monoShutterA->addInsertCell("Inner",portA->getCell("Void"));
  monoShutterA->addInsertCell("Outer",getCC("Main").getInsertCells());
  monoShutterA->createAll(System,*shutterPipe,0,*portA,2);
  
  monoShutterB->addInsertCell("Inner",shutterPipe->getCell("Void"));
  monoShutterB->addInsertCell("Inner",portB->getCell("Void"));
  monoShutterB->addInsertCell("Outer",getCC("Main").getInsertCells());
  monoShutterB->createAll(System,*shutterPipe,0,*portB,2);

  ContainedGroup::addOuterSurf("Main",*shutterPipe);
  ContainedGroup::addOuterUnionSurf("Main",*portA);
  ContainedGroup::addOuterUnionSurf("Main",*portB);
  //  ContainedGroup::addOuterSurf("FlangeA",shutterPipe->getCC("FlangeA"));
  //  ContainedGroup::addOuterSurf("FlangeB",shutterPipe->getCC("FlangeB"));
  ContainedGroup::addOuterSurf("ShutterA",monoShutterA->getCC("Outer"));
  ContainedGroup::addOuterSurf("ShutterB",monoShutterB->getCC("Outer"));

  SurfMap::addSurf("TopPlate",shutterPipe->getSignedSurf("OuterB"));

  return;
}

void
RoundMonoShutter::createLinks()
  /*!
    Create a front/back links and centre origin
   */
{
  ELog::RegMethod RControl("RoundMonoShutter","createLinks");
  
  setLinkCopy(0,*shutterPipe,1);
  setLinkCopy(1,*shutterPipe,2);

  const Geometry::Vec3D CP=(portA->getCentre()+portB->getCentre())/2.0;
  setConnect(2,CP,Y);   // center origin between plates

  return;
}

void
RoundMonoShutter::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp for beam origin
    \param sideIndex :: link point of centre [and axis]
   */
{
  ELog::RegMethod RegA("RoundMonoShutter","createAll");
 
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildComponents(System);

  createSurfaces();
  createObjects(System);
  createLinks();

  return;
}

}  // NAMESPACE xraySystem
