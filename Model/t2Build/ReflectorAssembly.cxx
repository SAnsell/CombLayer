/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2Build/makeReflector.cxx
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
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "AttachSupport.h"

#include "TargetBase.h"
#include "TS2target.h"
#include "Groove.h"
#include "Hydrogen.h"
#include "OrthoInsert.h"
#include "VacVessel.h"
#include "FlightLine.h"
#include "World.h"
#include "PreMod.h"
#include "HWrapper.h"
#include "Decoupled.h"
#include "DecLayer.h"
#include "DecFileMod.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "CouplePipe.h"
#include "DecouplePipe.h"
#include "Bucket.h"

#include "CoolPad.h"
#include "RefCutOut.h"
#include "RefBolts.h"
#include "Reflector.h"
#include "insertObject.h"
#include "insertPlate.h"
#include "ReflectorAssembly.h"

namespace moderatorSystem
{

ReflectorAssembly::ReflectorAssembly(const std::string& Key)  :
  attachSystem::FixedRotate(Key,12),
  attachSystem::ContainedComp(),

  decType("standard"),decFile(""),orthoHFlag(0),
  
  TarObj(new TMRSystem::TS2target("t2Target")),
  GrooveObj(new Groove("groove")),
  HydObj(new Hydrogen("hydrogen")),
  VacObj(new VacVessel("cvac")),
  PMgroove(new PreMod("groovePM")),
  PMhydro(new PreMod("hydroPM")),
  Horn(new HWrapper("hornPM")),
  DVacObj(new VacVessel("dvac")),
  PMdec(new PreMod("decPM")),
  IRcut(new RefCutOut("chipIRCut")),
  CdBucket(new Bucket("cdBucket")),
  RefObj(new Reflector("Reflector"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(TarObj);
  OR.addObject(GrooveObj);
  OR.addObject(HydObj);
  OR.addObject(VacObj);
  OR.addObject(PMgroove);
  OR.addObject(PMhydro);
  OR.addObject(Horn);
  OR.addObject(DVacObj);
  OR.addObject(PMdec);
  OR.addObject(IRcut);
  OR.addObject(CdBucket);
  OR.addObject(RefObj);
}



ReflectorAssembly::~ReflectorAssembly() 
  /*!
    Destructor
  */
{}
  

void
ReflectorAssembly::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ReflectorAssembly","createObjects");

  
  RefObj->createAll(System,*this,0);
  const int refCell=RefObj->getCell("Reflector");

  TarObj->addInsertCell(refCell);  
  TarObj->setCutSurf("FrontPlate",RefObj->getSurf("CornerA"));
  TarObj->setCutSurf("BackPlate",RefObj->getSurf("CornerB"));
  TarObj->createAll(System,*this,0);
  TarObj->addProtonLineInsertCell(RefObj->getCell("Reflector"));
  TarObj->addProtonLine(System);
  
  GrooveObj->createAll(System,*RefObj,0);
  HydObj->setCutSurf("innerWall",GrooveObj->getLinkSurf(1));
  HydObj->createAll(System,*GrooveObj,1);

  if (orthoHFlag)
    {
      OrthoInsert OI("ortho");
      OI.build(System,*HydObj,*GrooveObj);
    }

  VacObj->setCutSurf("Internal",GrooveObj->getExcludeSurf()*
		     HydObj->getExcludeSurf());
  VacObj->addInsertCell(refCell);
  VacObj->buildPair(System,*GrooveObj,*HydObj);

  RefObj->insertComponent(System,"FLGroove",VacObj->getMainRule("front"));
  RefObj->insertComponent(System,"FLHydro",VacObj->getMainRule("back"));

  PMgroove->addInsertCell(refCell);
  PMgroove->setTargetSurf(TarObj->getLinkSurf(1));
  PMgroove->setDivideSurf(VacObj->getDivideSurf());
  PMgroove->setEdge();
  PMgroove->createAll(System,*VacObj,6); 

  PMhydro->addInsertCell(refCell);
  PMhydro->setTargetSurf(TarObj->getLinkSurf(1));
  PMhydro->setDivideSurf(-VacObj->getDivideSurf());
  PMhydro->setEdge();
  PMhydro->setRotate();
  PMhydro->createAll(System,*VacObj,6);  

  Horn->addInsertCell(refCell);
  Horn->setCutSurf("DivideSurf",-VacObj->getDivideSurf());

  const HeadRule VUnit=
    attachSystem::intersectionLink(*VacObj,{-2,3,-4,5,-6});

  Horn->setCutSurf("VacCan",VUnit);

  Horn->setLinkCopy("FLhydro",*RefObj,"FLhydro");
  Horn->setLinkCopy("FLhydroNeg",*RefObj,"FLhydroNeg");
  Horn->setLinkCopy("FLhydroPlus",*RefObj,"FLhydroPlus");
  Horn->setLinkCopy("FLhydroDown",*RefObj,"FLhydroDown");
  Horn->setLinkCopy("FLhydroUp",*RefObj,"FLhydroUp");

  Horn->setLinkCopy("VacFront",*VacObj,"VacDivider");
  Horn->setLinkCopy("VacBack",*VacObj,"VacBack");
  Horn->setLinkCopy("VacNeg",*VacObj,"VacNeg");
  Horn->setLinkCopy("VacPlus",*VacObj,"VacPlus");
  Horn->setLinkCopy("VacDown",*VacObj,"VacDown");
  Horn->setLinkCopy("VacUp",*VacObj,"VacUp");


  // This can be optimised to a smaller surface:
  Horn->setCutSurf("BaseCut",*PMhydro,"minusZ");
  Horn->setCutSurf("BaseFullCut",PMhydro->getOuterSurf());
  Horn->setCutSurf("BaseFrontCut",*PMhydro,"back");
  Horn->createAll(System,*HydObj,0);

  processDecoupled(System);
  
  const attachSystem::ContainedComp* CMod=
    System.getObjectThrow<attachSystem::ContainedComp>
    (DMod->getKeyName(),"DMod to CC failed");

  DVacObj->addInsertCell(refCell);
  DVacObj->setCutSurf("Internal",CMod->getExcludeSurf());
  DVacObj->buildSingle(System,*DMod);
  RefObj->insertComponent(System,"FLNarrow",DVacObj->getMainRule("front"));
  RefObj->insertComponent(System,"FLWish",DVacObj->getMainRule("back"));

  PMdec->addInsertCell(refCell);
  PMdec->setTargetSurf(TarObj->getLinkSurf(1));
  PMdec->setRotate();
  PMdec->createAll(System,*DVacObj,5); 

  IRcut->addInsertCell(refCell);
  IRcut->setCutSurf("RefEdge",RefObj->getSurfRules("Left"));
  IRcut->createAll(System,*TarObj,0);

  CdBucket->addInsertCell(refCell);
  CdBucket->setCutSurf("FLwish",*RefObj,"FLwish");
  CdBucket->setCutSurf("FLnarrow",*RefObj,"FLnarrow");
  CdBucket->createAll(System,*RefObj,"Origin");


  addOuterSurf(*RefObj);
  return;
}

void
ReflectorAssembly::processDecoupled(Simulation& System)				
  /*!
    Create the decoupled item and build it
    \param System :: Simulation 

   */
{
  ELog::RegMethod RegA("ReflectorAssembly","processDecoupled");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  if (decType=="decFile")
    {
      // This strange construct :
      DecFileMod* DFPtr=new DecFileMod("decoupled");
      DFPtr->createAllFromFile(System,*RefObj,0,decFile);
      DMod=std::shared_ptr<Decoupled>(DFPtr);  
    }
  else if (decType=="standard")  // Standard one
    {
      std::shared_ptr<Decoupled> DModPtr(new Decoupled("decoupled"));
      //      OR.addObject(DModPtr);
      DModPtr->createAll(System,World::masterOrigin(),0);
      DMod=DModPtr;
    }
  else if (decType=="layer")  // layer
    {
      std::shared_ptr<Decoupled> DModPtr(new DecLayer("decoupled","decLayer")); 
      DModPtr->createAll(System,*RefObj,0);
      DMod=DModPtr;
    }
  else if (decType=="plate")  // layer
    {
      std::shared_ptr<insertSystem::insertPlate>
	PPtr(new insertSystem::insertPlate("decPlate")); 
       PPtr->setNoInsert();
       PPtr->addInsertCell(RefObj->getCell("Reflector"));
       PPtr->createAll(System,*RefObj,0);
       DMod=PPtr;
    }
  else 
    {
      throw ColErr::InContainerError<std::string>
	(decType,"Decoupled moderator type");
    }
  OR.addObject(DMod);

  return;
}

void
ReflectorAssembly::insertPipeObjects(Simulation& System)

/*!
    Calls all the objects that own a pipe to be
    inserted
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ReflectorAssembly","insertPipeObjects");
  
  CouplePipe CP("cplPipe");
  System.createObjSurfMap();
  CP.build(System,*HydObj,4,*VacObj);

  DecouplePipe DP("decPipe");
  DP.build(System,*DMod,0,*DVacObj,1);

  if (boltFlag)
    {
      RefBolts RB("refBolt");
      RB.createAll(System,*RefObj,0);
    }

  return;
}
  


void
ReflectorAssembly::createAll(Simulation& System,
			     const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param IParam :: Parameter table
    \param excludeCell :: exclude Cell
  */
{
  ELog::RegMethod RegA("ReflectorAssembly","build");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createObjects(System);
  //  createLinks();
  insertObjects(System);
  return;
}

}  // NAMESPACE moderatorSystem
