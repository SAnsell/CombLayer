/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/MagnetM1.cxx
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
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
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"

#include "EntryPipe.h"
#include "HalfElectronPipe.h"
#include "EPCombine.h"
#include "Dipole.h"
#include "Quadrupole.h"
#include "Octupole.h"
#include "MagnetM1.h"

namespace xraySystem
{

MagnetM1::MagnetM1(const std::string& Key) : 
  attachSystem::FixedOffset(Key,8),
  attachSystem::ContainedGroup("Main","FPipe","BPipe"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  entryPipe(new xraySystem::EntryPipe(keyName+"EntryPipe")),
  halfPipe(new xraySystem::HalfElectronPipe(keyName+"HalfElectron")),
  epCombine(new xraySystem::EPCombine(keyName+"EPCombine")),
  
  Oxx(new xraySystem::Octupole(keyName+"OXX")),
  QFend(new xraySystem::Quadrupole(keyName+"QFend")),
  Oxy(new xraySystem::Octupole(keyName+"OXY")),
  QDend(new xraySystem::Quadrupole(keyName+"QDend")),
  DIPm(new xraySystem::Dipole(keyName+"DIPm")),
  Oyy(new xraySystem::Octupole(keyName+"OYY"))

  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(1,"Flange");
  nameSideIndex(2,"Photon");
  nameSideIndex(3,"Electron");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(entryPipe);
  OR.addObject(halfPipe);
  OR.addObject(epCombine);
  OR.addObject(Oxx);
  OR.addObject(QFend);
  OR.addObject(Oxy);
  OR.addObject(QDend);
  OR.addObject(DIPm);
  OR.addObject(Oyy);
}


MagnetM1::~MagnetM1() 
  /*!
    Destructor
  */
{}

void
MagnetM1::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("MagnetM1","populate");

  FixedOffset::populate(Control);

  blockYStep=Control.EvalVar<double>(keyName+"BlockYStep");
  length=Control.EvalVar<double>(keyName+"Length");

  outerVoid=Control.EvalVar<double>(keyName+"OuterVoid");
  ringVoid=Control.EvalVar<double>(keyName+"RingVoid");
  topVoid=Control.EvalVar<double>(keyName+"TopVoid");
  baseVoid=Control.EvalVar<double>(keyName+"BaseVoid");
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
    
  return;
}

void
MagnetM1::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MagnetM1","createSurface");

  // Do outer surfaces (vacuum ports)
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*blockYStep,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length+blockYStep),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outerVoid,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*ringVoid,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*baseVoid,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*topVoid,Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(outerVoid+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(ringVoid+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(baseVoid+baseThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(topVoid+baseThick),Z);


  return;
}

void
MagnetM1::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MagnetM1","createObjects");

  
  HeadRule HR,frontHR,backHR;

  // Outer Metal
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex," 1 -2 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("Outer",System,cellIndex++,wallMat,0.0,HR);


  frontHR=entryPipe->getFullRule(-1);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -1 13 -14 15 -16");
  makeCell("FrontVoid",System,cellIndex++,0,0.0,HR*frontHR);
  entryPipe->insertInCell("Main",System,getCell("FrontVoid"));
  entryPipe->insertInCell("Flange",System,getCell("FrontVoid"));
  
  // First zone:
  backHR=Oxx->getFullRule(1);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 3 -4 5 -6");
  makeCell("Seg1",System,cellIndex++,0,0.0,HR*backHR);
  entryPipe->insertInCell("Main",System,getCell("Seg1"));

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");

  // Main segments:
  // Oxx magnet:
  frontHR=backHR.complement();
  backHR=Oxx->getFullRule(-2);
  makeCell("SegOxx",System,cellIndex++,0,0.0,HR*frontHR*backHR);
  Oxx->insertInCell(System,getCell("SegOxx"));

  // Oxx -> QFend
  frontHR=backHR.complement();
  backHR=QFend->getFullRule(1);
  makeCell("Seg2",System,cellIndex++,0,0.0,HR*frontHR*backHR);
  entryPipe->insertInCell("Main",System,getCell("Seg2"));  

  // QFend
  frontHR=backHR.complement();
  backHR=QFend->getFullRule(-2);
  //  makeCell("SegQF",System,cellIndex++,0,0.0,HR*frontHR*backHR);
  //  QFend->insertInCell(System,getCell("SegQF"));

  // QFend -> Oxy
  frontHR=backHR.complement();
  backHR=Oxy->getFullRule(1);
  makeCell("Seg3",System,cellIndex++,0,0.0,HR*frontHR*backHR);
  entryPipe->insertInCell("Main",System,getCell("Seg3"));

  // Oxy magnet:
  frontHR=backHR.complement();
  backHR=Oxy->getFullRule(-2);
  makeCell("SegOxy",System,cellIndex++,0,0.0,HR*frontHR*backHR);
  Oxy->insertInCell(System,getCell("SegOxy"));

  // Oxy -> QDend 
  frontHR=backHR.complement();
  backHR=QDend->getFullRule(1);
  makeCell("Seg4",System,cellIndex++,0,0.0,HR*frontHR*backHR);
  entryPipe->insertInCell("Main",System,getCell("Seg4"));

  // QDend
  frontHR=backHR.complement();
  backHR=QDend->getFullRule(-2);
  //  makeCell("SegQD",System,cellIndex++,0,0.0,HR*frontHR*backHR);
  //  QDend->insertInCell(System,getCell("SegQD"));

  // QDend -> end
  frontHR=backHR.complement();
  backHR=entryPipe->getFullRule(-2);
  makeCell("Seg5",System,cellIndex++,0,0.0,HR*frontHR*backHR);
  entryPipe->insertInCell("Main",System,getCell("Seg5"));

  // HALF PIPE
  
  frontHR=entryPipe->getFullRule(2);
  backHR=halfPipe->getFullRule("#midPlane");
  makeCell("Seg7",System,cellIndex++,0,0.0,HR*frontHR*backHR);
  halfPipe->insertInCell("Half",System,getCell("Seg7"));

  frontHR=halfPipe->getFullRule("midPlane");
  backHR=halfPipe->getFullRule(-3);
  makeCell("Seg8",System,cellIndex++,0,0.0,HR*frontHR*backHR);
  halfPipe->insertInCell("Full",System,getCell("Seg8"));

  frontHR=halfPipe->getFullRule(3);
  backHR=epCombine->getFullRule(-2);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6 -2 ");
  makeCell("Seg9",System,cellIndex++,0,0.0,HR*frontHR);
  epCombine->insertInCell(System,getCell("Seg9"));

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 13 -14 15 -16");
  makeCell("Seg10",System,cellIndex++,0,0.0,HR*backHR);
  epCombine->insertInCell(System,getCell("Seg10"));

  DIPm->insertInCell(System,getCell("Seg7"));
  DIPm->insertInCell(System,getCell("Seg8"));
  
  //
  // MAIN OUTER VOID:
  //

  frontHR=entryPipe->getFullRule(-1);
  backHR=epCombine->getFullRule(-2);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 13 -14 15 -16 ");
  addOuterSurf("Main",HR*frontHR*backHR);

  
  // Construct the inner zone a a innerZone
  // HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6  ");
  // makeCell("Void",System,cellIndex++,voidMat,0.0,HR);



  return;
}

void 
MagnetM1::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("MagnetM1","createLinks");

  // link 0 / 1 from PreDipole / EPCombine
  setLinkCopy(0,*entryPipe,1);

  setLinkCopy(1,*epCombine,epCombine->getSideIndex("Flange"));
  setLinkCopy(2,*epCombine,epCombine->getSideIndex("Photon"));
  setLinkCopy(3,*epCombine,epCombine->getSideIndex("Electron"));
  
  setConnect(4,Origin+Y*blockYStep,-Y);
  setLinkSurf(4,-SMap.realSurf(buildIndex+1));

  setConnect(5,Origin+Y*(length+blockYStep),Y);
  setLinkSurf(6,SMap.realSurf(buildIndex+2));

  return;
}

void
MagnetM1::createEndPieces(Simulation& System)
  /*!
    Create the end piece cutting system for ContainedGroup
  */
{
  ELog::RegMethod RegA("MagnetM1","createEndPieces");
  
  HeadRule HR;
  const HeadRule frontHR=entryPipe->getFullRule(-1);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -1 13 -14 15 -16");
  makeCell("Front",System,cellIndex++,0,0.0,HR*frontHR);

  entryPipe->insertInCell("Main",System,getCell("Front"));
  entryPipe->insertInCell("Flange",System,getCell("Front"));
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-2 13 -14 15 -16 ");

  addOuterSurf("Main",HR*frontHR);


  return;
}

void
MagnetM1::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("MagnetM1","createAll");

  int outerCell;
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();

  // puts in 74123 etc.
  //  preDipole->addInsertCell("FlangeA",this->getInsertCells());
  //  epCombine->addInsertCell(this->getInsertCells());
 
  //  entryPipe->addInsertCell("Main",getCell("Void"));
  entryPipe->createAll(System,FC,sideIndex);

  halfPipe->setCutSurf("front",*entryPipe,"back");
  halfPipe->createAll(System,*entryPipe,"back");

  epCombine->setEPOriginPair(*halfPipe,"Photon","Electron"); 
  epCombine->setCutSurf("front",*halfPipe,"back");
  epCombine->createAll(System,*halfPipe,"back");

  Oxx->setCutSurf("Inner",entryPipe->getSurfRule("OuterRadius"));
  Oxx->createAll(System,*this,0);
  
  QFend->setCutSurf("Inner",entryPipe->getSurfRule("OuterRadius"));
  QFend->createAll(System,*this,0);

  Oxy->setCutSurf("Inner",entryPipe->getSurfRule("OuterRadius"));
  Oxy->createAll(System,*this,0);

  QDend->setCutSurf("Inner",entryPipe->getSurfRule("OuterRadius"));
  QDend->createAll(System,*this,0);

  // 102
  DIPm->setCutSurf("MidSplit",halfPipe->getSurf("electronCut")); //102
  DIPm->copyCutSurf("InnerA",*halfPipe,"HalfElectron");
  DIPm->copyCutSurf("InnerB",*halfPipe,"FullElectron");
  DIPm->createAll(System,*this,0);

  createObjects(System);
  insertObjects(System);
  createLinks();
  return;
}
  
}  // NAMESPACE xraySystem
