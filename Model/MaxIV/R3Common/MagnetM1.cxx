/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/MagnetM1.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
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
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "InnerZone.h" 

#include "PreDipole.h"
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
  buildZone(*this,cellIndex),
  preDipole(new xraySystem::PreDipole(keyName+"PreDipole")),
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
  
  OR.addObject(preDipole);
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

  std::string Out;
  // Construct the inner zone a a innerZone
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6  ");

  buildZone.setSurround(HeadRule(Out));
  buildZone.setFront(HeadRule(SMap.realSurf(buildIndex+1)));
  buildZone.setBack(HeadRule(-SMap.realSurf(buildIndex+2)));
  buildZone.setInnerMat(voidMat);
  buildZone.constructMasterCell(System);

  CellMap::setCell("Void",buildZone.getMaster()->getName());
  // Out=ModelSupport::getComposite
  //   (SMap,buildIndex,"1 -2 3 -4 5 -6 ");
  // makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("Outer",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 13 -14 15 -16 ");
  addOuterSurf("Main",Out);

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
  setLinkSignedCopy(0,*preDipole,1);
  setLinkSignedCopy(1,*epCombine,epCombine->getSideIndex("Flange"));
  setLinkSignedCopy(2,*epCombine,epCombine->getSideIndex("Photon"));
  setLinkSignedCopy(3,*epCombine,epCombine->getSideIndex("Electron"));
  
  setConnect(4,Origin+Y*blockYStep,-Y);
  setLinkSurf(4,-SMap.realSurf(buildIndex+1));

  setConnect(5,Origin+Y*(length+blockYStep),Y);
  setLinkSurf(6,SMap.realSurf(buildIndex+2));

  return;
}

void
MagnetM1::createEndPieces()
  /*!
    Create the end piece cutting system for ContainedGroup
  */
{
  ELog::RegMethod RegA("MagnetM1","createEndPieces");
  
  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex," -1 ");
  Out+=preDipole->getSurfString("frontFlangeTube");  
  addOuterSurf("FPipe",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 ");
  Out+=epCombine->getSurfString("voidCyl");  
  addOuterSurf("BPipe",Out);
 	       
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
  createObjects(System);


  MonteCarlo::Object* masterCell=buildZone.getMaster();
  // puts in 74123 etc.
  //  preDipole->addInsertCell("FlangeA",this->getInsertCells());
  //  epCombine->addInsertCell(this->getInsertCells());
  insertObjects(System);

  if (isActive("front"))
    preDipole->copyCutSurf("front",*this,"front");
  
  preDipole->createAll(System,*this,0);

  epCombine->addInsertCell(getCells("Void"));  // needed ????
  epCombine->setEPOriginPair(*preDipole,"Photon","Electron");
  epCombine->createAll(System,*preDipole,2);

  attachSystem::InnerZone& IZ=preDipole->getBuildZone();
  MonteCarlo::Object* pipeCell=IZ.getMaster();

  attachSystem::InnerZone& BZ=preDipole->getBendZone();
  MonteCarlo::Object* bendCell=BZ.getMaster();

  attachSystem::InnerZone& EZ=preDipole->getExitZone();
  MonteCarlo::Object* exitCell=EZ.getMaster();
    
  Oxx->setCutSurf("Inner",preDipole->getFullRule(5));
  Oxx->createAll(System,*this,0);
  IZ.cutVoidUnit(System,pipeCell,Oxx->getMainRule(-1), Oxx->getMainRule(-2));
  outerCell=buildZone.triVoidUnit(System,masterCell,
				  Oxx->getMainRule(-1), Oxx->getMainRule(-2));
  //  Oxx->insertInCell(System,getCell("Void"));

  Oxx->insertInCell(System,outerCell);
  preDipole->insertInCell("Tube",System,outerCell-1);

  
  QFend->setInnerTube(preDipole->getFullRule(5));
  QFend->createAll(System,*this,0);
  IZ.cutVoidUnit(System,pipeCell,QFend->getMainRule(-1),QFend->getMainRule(-2));
  outerCell=buildZone.cutVoidUnit
    (System,masterCell,QFend->getMainRule(-1),QFend->getMainRule(-2));
  preDipole->insertInCell("Tube",System,outerCell);


  Oxy->setCutSurf("Inner",preDipole->getFullRule(5));
  Oxy->createAll(System,*this,0);
  IZ.cutVoidUnit(System,pipeCell,Oxy->getMainRule(-1), Oxy->getMainRule(-2));
  outerCell=buildZone.triVoidUnit
    (System,masterCell,Oxy->getMainRule(-1), Oxy->getMainRule(-2));

  Oxy->insertInCell(System,outerCell);
  preDipole->insertInCell("Tube",System,outerCell-1);

    
  QDend->setInnerTube(preDipole->getFullRule(5));
  QDend->createAll(System,*this,0);
  IZ.cutVoidUnit(System,pipeCell,QDend->getMainRule(-1),QDend->getMainRule(-2));

    
  outerCell=buildZone.cutVoidUnit
    (System,masterCell,QDend->getMainRule(-1),QDend->getMainRule(-2));
  preDipole->insertInCell("Tube",System,outerCell);
  // move to next bend object
  DIPm->setCutSurf("MidSplit",preDipole->getSurf("electronCut"));
  DIPm->setCutSurf("InnerA",preDipole->getFullRule(6));
  DIPm->setCutSurf("InnerB",preDipole->getFullRule(7));
  DIPm->createAll(System,*this,0);
  outerCell=
    BZ.cutVoidUnit(System,bendCell,DIPm->getMainRule(-1),
		   DIPm->getMainRule(-2));
  // DIPm extends into exit cell:
  outerCell=
    EZ.singleVoidUnit(System,exitCell,DIPm->getMainRule(2));
  System.removeCell(outerCell);
  outerCell=buildZone.triVoidUnit
    (System,masterCell,DIPm->getMainRule(-1),DIPm->getMainRule(-2));
  DIPm->insertInCell(System,outerCell);

  
  preDipole->insertInCell("Tube",System,outerCell-1);
  
  // Insert OYY
  Oyy->setCutSurf("Inner",preDipole->getFullRule(7));
  Oyy->createAll(System,*this,0);
  EZ.cutVoidUnit(System,exitCell,Oyy->getMainRule(-1), Oyy->getMainRule(-2));
  outerCell=buildZone.triVoidUnit
    (System,masterCell,Oyy->getMainRule(-1), Oyy->getMainRule(-2));

  Oyy->insertInCell(System,outerCell);
  preDipole->insertInCell("Tube",System,outerCell-1);

  createEndPieces();
  // end insert  


  outerCell=buildZone.singleVoidUnit(System,masterCell,
				     preDipole->getSurfRule("endFlange"));
  preDipole->insertInCell("Tube",System,outerCell);
  preDipole->insertInCell("Tube",System,masterCell->getName());
  EZ.singleVoidUnit(System,exitCell,preDipole->getSurfRule("endFlange"));
  preDipole->insertInCell("Tube",*bendCell);

  // flange of EC:

  outerCell=buildZone.singleVoidUnit
    (System,masterCell,epCombine->getMainRule(-1));
  preDipole->insertInCell("Tube",System,outerCell);
  System.minimizeObject(preDipole->getKeyName());  
    
  epCombine->insertInCell(*masterCell);
  //  EZ.singleVoidUnit(System,exitCell, preDipole->getSurfRule("endFlange"));
  //  exitZone

  
  // creation of links 
  createLinks();
  return;
}
  
}  // NAMESPACE xraySystem
