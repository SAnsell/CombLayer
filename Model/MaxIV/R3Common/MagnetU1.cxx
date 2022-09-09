/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/MagnetU1.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "FixedRotateUnit.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "Importance.h"
#include "Object.h"

#include "Dipole.h"
#include "Quadrupole.h"
#include "Sexupole.h"
#include "CorrectorMag.h"
#include "CornerPipe.h"
#include "EntryPipe.h"
#include "MagnetU1.h"


namespace xraySystem
{

MagnetU1::MagnetU1(const std::string& Key) : 
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedGroup("Main"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  entryPipe(new xraySystem::EntryPipe(keyName+"EntryPipe",-1)),
  QFm1(new xraySystem::Quadrupole(keyName+"QFm1")),
  SFm(new xraySystem::Sexupole(keyName+"SFm")),
  QFm2(new xraySystem::Quadrupole(keyName+"QFm2")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"cMagVA")),
  cMagHA(new xraySystem::CorrectorMag(keyName+"cMagHA")),
  SD1(new xraySystem::Sexupole(keyName+"SD1")),
  DIPm(new xraySystem::Dipole(keyName+"DIPm")),
  SD2(new xraySystem::Sexupole(keyName+"SD2")),
  exitPipe(new xraySystem::EntryPipe(keyName+"ExitPipe",1))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(1,"Flange");
  nameSideIndex(2,"Electron");
  nameSideIndex(3,"Photon");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(entryPipe);
  OR.addObject(QFm1);
  OR.addObject(SFm);
  OR.addObject(QFm2);
  OR.addObject(cMagVA);
  OR.addObject(cMagHA);
  OR.addObject(SD1);
  OR.addObject(DIPm);
  OR.addObject(SD2);
  OR.addObject(exitPipe);
}


MagnetU1::~MagnetU1() 
  /*!
    Destructor
  */
{}

void
MagnetU1::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("MagnetU1","populate");

  FixedRotate::populate(Control);

  blockYStep=Control.EvalVar<double>(keyName+"BlockYStep");
  length=Control.EvalVar<double>(keyName+"Length");

  frontVoid=Control.EvalVar<double>(keyName+"FrontVoid");
  backVoid=Control.EvalVar<double>(keyName+"BackVoid");
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
MagnetU1::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MagnetU1","createSurface");


  // Do outer surfaces (vacuum ports)
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*blockYStep,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length+blockYStep),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outerVoid,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*ringVoid,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*baseVoid,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*topVoid,Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin+Y*(blockYStep-frontVoid),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(length+blockYStep+backVoid),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(outerVoid+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(ringVoid+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(baseVoid+baseThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(topVoid+baseThick),Z);

  
  return;
}

void
MagnetU1::createUnit(Simulation& System,
		     size_t& index,
		     const attachSystem::FixedComp& preUnit,
		     const attachSystem::FixedComp& mainUnit)
/*!
    Create a magnet-magnet units with the intermediate segment
    (metal etc) between the two components. Both pre and main
    are expected to have been created.
    \param System :: Simulation to create objects in
    \param preUnit :: Pre-uint
  */
{
  ELog::RegMethod RegA("MagnetU1","createUnit");

  HeadRule frontHR,backHR;

  const HeadRule HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");

  frontHR=preUnit.getFullRule("back");
  backHR=mainUnit.getFullRule("front");  
  makeCell("Seg"+std::to_string(index++),
	   System,cellIndex++,wallMat,0.0,HR*frontHR*backHR);

  frontHR=mainUnit.getFullRule("#front");
  backHR=mainUnit.getFullRule("#back");
  makeCell("Seg"+std::to_string(index++),
	   System,cellIndex++,wallMat,0.0,HR*frontHR*backHR);

  return;
}
  

void
MagnetU1::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MagnetU1","createObjects");

  HeadRule HR,frontHR,backHR;

  // Outer Metal
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex," 1 -2 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("Outer",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"11 -1 13 -14 15 -16 ");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"2 -12 13 -14 15 -16 ");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR);

  // this is same as electron incoming

  DIPm->createAll(System,*this,0);
  
  entryPipe->setFront(SMap.realSurf(buildIndex+11));
  entryPipe->setBack(*DIPm,"front");
  entryPipe->createAll(System,*this,0);  
  entryPipe->insertInCell("Flange",System,getCell("FrontVoid"));
  entryPipe->insertInCell("Pipe",System,getCell("FrontVoid"));
  
  exitPipe->setFront(*DIPm,"back");
  exitPipe->setBack(-SMap.realSurf(buildIndex+12));  
  exitPipe->createAll(System,*DIPm,"back");
  exitPipe->insertInCell("Flange",System,getCell("BackVoid"));
  exitPipe->insertInCell("Pipe",System,getCell("BackVoid"));

  QFm1->setCutSurf("Inner",entryPipe->getSurfRule("OuterRadius"));
  QFm1->createAll(System,*this,0);

  SFm->setCutSurf("Inner",entryPipe->getSurfRule("OuterRadius"));
  SFm->createAll(System,*this,0);
  QFm2->setCutSurf("Inner",entryPipe->getSurfRule("OuterRadius"));
  QFm2->createAll(System,*this,0);

  cMagVA->setCutSurf("Inner",entryPipe->getSurfRule("OuterRadius"));
  cMagHA->setCutSurf("Inner",entryPipe->getSurfRule("OuterRadius"));
  cMagVA->createAll(System,*this,0);
  cMagHA->createAll(System,*this,0);

  SD1->setCutSurf("Inner",entryPipe->getSurfRule("OuterRadius"));
  SD1->createAll(System,*this,0);

  SD2->setCutSurf("Inner",exitPipe->getSurfRule("OuterRadius"));
  SD2->createAll(System,*DIPm,"back");

  
  backHR=QFm1->getFullRule(1);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1  3 -4 5 -6");
  makeCell("Seg1",System,cellIndex++,wallMat,0.0,HR*backHR);  
  entryPipe->insertInCell("Pipe",System,getCell("Seg1"));

  frontHR=backHR.complement();
  backHR=QFm1->getFullRule(-2);
  /*
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
    makeCell("Seg2",System,cellIndex++,wallMat,0.0,HR*frontHR*backHR);
    QFm1->insertInCell(System,getCell("Seg2"));
  */
  size_t segIndex(3);
  createUnit(System,segIndex,*QFm1,*SFm);
  SFm->insertInCell(System,getCell("Seg4"));
  entryPipe->insertInCell("Pipe",System,getCell("Seg3"));
  
  createUnit(System,segIndex,*SFm,*QFm2);
  QFm2->insertInCell(System,getCell("Seg6"));
  entryPipe->insertInCell("Pipe",System,getCell("Seg5"));  

  createUnit(System,segIndex,*QFm2,*cMagVA);
  cMagVA->insertInCell(System,getCell("Seg8"));
  entryPipe->insertInCell("Pipe",System,getCell("Seg7"));

  createUnit(System,segIndex,*cMagVA,*cMagHA);
  cMagHA->insertInCell(System,getCell("Seg10"));
  entryPipe->insertInCell("Pipe",System,getCell("Seg9"));

  createUnit(System,segIndex,*cMagHA,*SD1);
  SD1->insertInCell(System,getCell("Seg12"));
  entryPipe->insertInCell("Pipe",System,getCell("Seg11"));

  createUnit(System,segIndex,*SD1,*DIPm);
  DIPm->insertInCell(System,getCell("Seg14"));
  entryPipe->insertInCell("Pipe",System,getCell("Seg13"));
  
  createUnit(System,segIndex,*DIPm,*SD2);
  SD2->insertInCell(System,getCell("Seg16"));
  exitPipe->insertInCell("Pipe",System,getCell("Seg15"));
  
  backHR=SD2->getFullRule(2);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 3 -4 5 -6");
  makeCell("Seg17",System,cellIndex++,wallMat,0.0,HR*backHR);  
  exitPipe->insertInCell("Pipe",System,getCell("Seg17"));
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 13 -14 15 -16 ");
  addOuterSurf("Main",HR);
  
  return;
}

void 
MagnetU1::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("MagnetU1","createLinks");

  // Tempory needs an entrance pipe:

  setConnect(0,Origin+Y*blockYStep,-Y);
  setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  setConnect(1,Origin+Y*(length+blockYStep),Y);
  setLinkSurf(1,-SMap.realSurf(buildIndex+2));

  setConnect(2,Origin+Y*(blockYStep-frontVoid),-Y);
  setLinkSurf(2,-SMap.realSurf(buildIndex+11));
  nameSideIndex(2,"voidFront");

  setConnect(3,Origin+Y*(blockYStep+length+backVoid),Y);
  setLinkSurf(3,SMap.realSurf(buildIndex+12));
  nameSideIndex(3,"voidBack");

  return;
}

void
MagnetU1::insertDipolePipe(Simulation& System,
			   const constructSystem::CornerPipe& dipolePipe)
{
  ELog::RegMethod RegA("MagnetU1","insertDipolePipe");

  const HeadRule pipeHR=
    dipolePipe.getCC("Tube").getOuterSurf().complement();
  
  // get corners
  std::vector<Geometry::Vec3D> Pts;
  const Geometry::Vec3D Axis(dipolePipe.getLinkAxis("cornerA"));
			     
  Pts.push_back(dipolePipe.getLinkPt("cornerA"));
  Pts.push_back(dipolePipe.getLinkPt("cornerB"));
  Pts.push_back(dipolePipe.getLinkPt("cornerC"));
  Pts.push_back(dipolePipe.getLinkPt("cornerD"));

  System.populateCells();
  System.validateObjSurfMap();

  // SETS of cells : (maybe should use CellMap here)
  //  SD1(new xraySystem::Sexupole(keyName+"SD1")),
  //  DIPm(new xraySystem::Dipole(keyName+"DIPm")),
  //  SD2(new xraySystem::Sexupole(keyName+"SD2"))
  const std::set<std::string> OItems({
      "QFm1","SFm","QFm2","cMagVA","cMagHA","SD1","DIPm"});
  std::set<int> CSet=System.getObjectRange(keyName);
  for(const std::string& objName : OItems)
    {
      const std::set<int> PSet=System.getObjectRange(keyName+objName);
      CSet.insert(PSet.begin(),PSet.end());
    }
  
  for(const int CN : CSet)
    {
      MonteCarlo::Object* OPtr=
	System.findObjectThrow(CN,"CN from CSet");
      size_t index=0;
      size_t flag(0);
      do
	{
	  flag=OPtr->hasIntercept(Pts[index],Y);

	  index++;
	} while(index<4 && !flag);
      
      if (flag)
	OPtr->addIntersection(pipeHR);
    }
  dipolePipe.insertInCell("Tube",System,getCell("FrontVoid"));
  dipolePipe.insertInCell("Tube",System,getCell("BackVoid"));
  return;
}

void
MagnetU1::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("MagnetU1","createAll");


  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  insertObjects(System);


  // creation of links
  
  createLinks();
  return;
}
  
}  // NAMESPACE xraySystem
