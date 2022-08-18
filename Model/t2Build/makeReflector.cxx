/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/makeReflector.cxx
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
#include "FixedOffset.h"
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
#include "makeReflector.h"
#include "insertObject.h"
#include "insertPlate.h"

namespace moderatorSystem
{

makeReflector::makeReflector()  :
  TarObj(new TMRSystem::TS2target("t2Target")),
  GrooveObj(new Groove("groove")),
  HydObj(new Hydrogen("hydrogen")),
  VacObj(new VacVessel("cvac")),
  FLgroove(new FlightLine("grooveFlight")),
  FLhydro(new FlightLine("hydroFlight")),
  PMgroove(new PreMod("groovePM")),
  PMhydro(new PreMod("hydroPM")),
  Horn(new HWrapper("hornPM")),
  DVacObj(new VacVessel("dvac")),
  FLwish(new FlightLine("wishFlight")),
  FLnarrow(new FlightLine("narrowFlight")),
  PMdec(new PreMod("decPM")),
  IRcut(new RefCutOut("chipIRCut")),
  CdBucket(new Bucket("cdBucket")),
  RefObj(new moderatorSystem::Reflector("Reflector"))
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
  OR.addObject(FLgroove);
  OR.addObject(FLhydro);
  OR.addObject(PMgroove);
  OR.addObject(PMhydro);
  OR.addObject(DVacObj);
  OR.addObject(FLwish);
  OR.addObject(FLnarrow);
  OR.addObject(PMdec);
  OR.addObject(CdBucket);
  OR.addObject(RefObj);
}



makeReflector::~makeReflector() 
  /*!
    Destructor
  */
{}
  


void
makeReflector::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("makeReflector","createObjects");

  RefObj->createAll(System,World::masterOrigin(),0);

  const int refCell=RefObj->getCell("Reflector");

  TarObj->addInsertCell(refCell);
  VacObj->addInsertCell(refCell);
  
  FLgroove->addInsertCell("outer",refCell);
  FLhydro->addInsertCell("outer",refCell);
  PMgroove->addInsertCell(refCell);
  PMhydro->addInsertCell(refCell);
  Horn->addInsertCell(refCell);

  DVacObj->addInsertCell(refCell);
  FLwish->addInsertCell("outer",refCell);
  FLnarrow->addInsertCell("outer",refCell);
  PMdec->addInsertCell(refCell);

  IRcut->addInsertCell(refCell);
  CdBucket->addInsertCell(refCell);
  // torpedoCell=refCell;

  
  return;
}

void
makeReflector::processDecoupled(Simulation& System,				
			    const mainSystem::inputParam& IParam)
  /*!
    Create the decoupled item and build it
    \param System :: Simulation 
    \param IParam :: Parameters
   */
{
  ELog::RegMethod RegA("makeReflector","processDecoupled");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();


  if (IParam.flag("decFile"))
    {
      // This strange construct :
      DecFileMod* DFPtr=new DecFileMod("decoupled");
      DFPtr->createAllFromFile(System,*RefObj,0,
          IParam.getValue<std::string>("decFile"));
      DMod=std::shared_ptr<Decoupled>(DFPtr);  
      return;
    }

  const std::string DT=IParam.getValue<std::string>("decType");
  if (DT=="standard")  // Standard one
    {
      std::shared_ptr<Decoupled> DModPtr(new Decoupled("decoupled"));
      //      OR.addObject(DModPtr);
      DModPtr->createAll(System,World::masterOrigin(),0);
      DMod=DModPtr;
    }
  else if (DT=="layer")  // layer
    {
      std::shared_ptr<Decoupled> DModPtr(new DecLayer("decoupled","decLayer")); 
      DModPtr->createAll(System,*RefObj,0);
      DMod=DModPtr;
    }
  else if (DT=="plate")  // layer
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
      ELog::EM<<"Unable to understand type decType required:"
	      <<DT<<ELog::endErr;
      throw ColErr::InContainerError<std::string>(DT,"DT value");
    }
  OR.addObject(DMod);

  return;
}

void
makeReflector::createInternalObjects
   (Simulation& System,const mainSystem::inputParam& IParam)
  /*!
    Build the inner objects
    \param System :: Simulation to use
    \param IParam :: Input paramters for decoupled type
  */
{
  ELog::RegMethod RegA("makeReflector","createInternalObjects");

  const std::string TarName=
    IParam.getValue<std::string>("targetType",0);
  const std::string DT=IParam.getValue<std::string>("decType");

  TarObj->setCutSurf("FrontPlate",RefObj->getSurf("CornerB"));
  TarObj->setCutSurf("BackPlate",RefObj->getSurf("CornerA"));
  TarObj->createAll(System,World::masterOrigin(),0);
  TarObj->addProtonLineInsertCell(RefObj->getCell("Reflector"));
  TarObj->addProtonLine(System);
  
  GrooveObj->createAll(System,*RefObj,0);
  HydObj->setCutSurf("innerWall",GrooveObj->getLinkSurf(1));
  HydObj->createAll(System,*GrooveObj,1);
  if (IParam.flag("orthoH"))
    {
      OrthoInsert OI("ortho");
      OI.build(System,*HydObj,*GrooveObj);
    }
  VacObj->buildPair(System,*GrooveObj,*HydObj);

  RefObj->insertComponent(System,"FLGroove",VacObj->getMainRule("front"));
  RefObj->insertComponent(System,"FLHydro",VacObj->getMainRule("back"));

  PMgroove->setTargetSurf(TarObj->getLinkSurf(1));
  PMgroove->setDivideSurf(VacObj->getDivideSurf());
  PMgroove->setEdge();
  PMgroove->createAll(System,*VacObj,6); 

  PMhydro->setTargetSurf(TarObj->getLinkSurf(1));
  PMhydro->setDivideSurf(-VacObj->getDivideSurf());
  PMhydro->setEdge();
  PMhydro->setRotate();
  PMhydro->createAll(System,*VacObj,6);  

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

  processDecoupled(System,IParam);
  

  const attachSystem::ContainedComp* CMod=
    System.getObjectThrow<attachSystem::ContainedComp>
    (DMod->getKeyName(),"DMod to CC failed");

  DVacObj->buildSingle(System,*DMod,CMod->getExclude());
  RefObj->insertComponent(System,"FLNarrow",DVacObj->getMainRule("front"));
  RefObj->insertComponent(System,"FLWish",DVacObj->getMainRule("back"));
    
  PMdec->setTargetSurf(TarObj->getLinkSurf(1));
  PMdec->setRotate();
  PMdec->createAll(System,*DVacObj,5); 

  IRcut->setCutSurf("RefEdge",RefObj->getSurfRules("Left"));
  IRcut->createAll(System,*TarObj,0);

  CdBucket->setCutSurf("FLwish",*RefObj,"FLwish");
  CdBucket->setCutSurf("FLnarrow",*RefObj,"FLnarrow");
  CdBucket->createAll(System,*RefObj,"Origin");

  return;
}

void
makeReflector::insertPipeObjects(Simulation& System,
			     const mainSystem::inputParam& IParam)
  /*!
    Calls all the objects that own a pipe to be
    inserted
    \param System :: Simulation to use
    \param IParam :: Parameter table
  */
{
  ELog::RegMethod RegA("makeReflector","insertPipeObjects");
  
  CouplePipe CP("cplPipe");
  System.createObjSurfMap();
  CP.build(System,*HydObj,4,*VacObj);

  DecouplePipe DP("decPipe");
  DP.build(System,*DMod,0,*DVacObj,1);

  if (IParam.flag("bolts"))
    {
      RefBolts RB("refBolt");
      RB.createAll(System,*RefObj,0);
    }

  return;
}
  
int
makeReflector::calcModeratorPlanes(const int BeamLine,
			       std::vector<int>& Window,
			       int& dSurf) const
  /*!
    Given the beamline calculate the defining view and the 
    active surface 
    \param BeamLine :: BeamLine number to use
    \param Window :: Window of surface to use
    \param dSurf :: divide surface
    \return Master Plane
  */
{
  ELog::RegMethod RegA("makeReflector","calcModeratorPlanes");

  if (BeamLine<0 || BeamLine>17)
    throw ColErr::IndexError<int>(BeamLine,18," <beamline number> ");
				  
  
  dSurf=0;
  if (BeamLine<4)       // NARROW
    {
      FLnarrow->getInnerVec(Window);
      return DVacObj->getLinkSurf(2);
    }
  if (BeamLine<9)      // H2
    {
      FLhydro->getInnerVec(Window);
      return VacObj->getLinkSurf(2);
    }
  if (BeamLine<14)      // Groove
    {
      FLgroove->getInnerVec(Window);
      dSurf=GrooveObj->getDividePlane();
      return VacObj->getLinkSurf(1);
    }
  // WISH
      FLwish->getInnerVec(Window);
      //      dSurf=DMod->getDividePlane(0);
      return DVacObj->getLinkSurf(2);
}

Geometry::Vec3D
makeReflector::getViewOrigin(const int BeamLine) const
  /*!
    Given the beamline calculate the defining view and the 
    active surface 
    \param BeamLine :: BeamLine number to use
    \return View point
  */
{
  ELog::RegMethod RegA("makeReflector","getViewOrigin");

  if (BeamLine<0 || BeamLine>17)
    throw ColErr::IndexError<int>(BeamLine,18,"BeamLine");
  
  if (BeamLine<4)       // NARROW
    {
      return DVacObj->getSurfacePoint(0,1);
    }
  if (BeamLine<9)      // H2
    {
      return VacObj->getSurfacePoint(0,2);
    }
  if (BeamLine<14)      // Groove
    {
      // Need to get correct Z:
      return GrooveObj->getViewPoint();
    }
  // WISH
  return DVacObj->getSurfacePoint(0,2);
}


void
makeReflector::build(Simulation& System,
		     const mainSystem::inputParam& IParam,
		     int& excludeCell)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param IParam :: Parameter table
    \param excludeCell :: exclude Cell
  */
{
  ELog::RegMethod RegA("makeReflector","build");

  RefObj->addInsertCell(excludeCell);
  createObjects(System);
  createInternalObjects(System,IParam);
  
  return;
}

}  // NAMESPACE shutterSystem