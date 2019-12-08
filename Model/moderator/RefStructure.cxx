/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/RefStructure.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "TargetBase.h"
#include "TS2target.h"
#include "TS2ModifyTarget.h"
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
#include "RefStructure.h"

#include "insertObject.h"
#include "insertPlate.h"

namespace moderatorSystem
{

RefStructure::RefStructure(const std::string& Key)  :
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
  OR.addObject(FLgroove);
  OR.addObject(FLhydro);
  OR.addObject(PMgroove);
  OR.addObject(PMhydro);
  OR.addObject(DVacObj);
  OR.addObject(FLwish);
  OR.addObject(FLnarrow);
  OR.addObject(PMdec);
  OR.addObject(CdBucket);
  OR.addObject(Reflector);
}



RefStructure::~RefStructure() 
  /*!
    Destructor
  */
{}
  


void
RefStructure::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RefStructure","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 11 -12 13 -14");
  addOuterSurf(Out);

  System.addCell(MonteCarlo::Object(cellIndex++,defMat,0.0,Out));

  TarObj->addInsertCell(cellIndex-1);

  VacObj->addInsertCell(cellIndex-1);
  FLgroove->addInsertCell("outer",cellIndex-1);
  FLhydro->addInsertCell("outer",cellIndex-1);
  PMgroove->addInsertCell(cellIndex-1);
  PMhydro->addInsertCell(cellIndex-1);
  Horn->addInsertCell(cellIndex-1);

  DVacObj->addInsertCell(cellIndex-1);
  FLwish->addInsertCell("outer",cellIndex-1);
  FLnarrow->addInsertCell("outer",cellIndex-1);
  PMdec->addInsertCell(cellIndex-1);

  IRcut->addInsertCell(cellIndex-1);
  CdBucket->addInsertCell(cellIndex-1);
  // torpedoCell=cellIndex-1;

  for(CoolPad& PD : Pads)
    PD.addInsertCell(74123);
  
  return;
}

void
RefStructure::processDecoupled(Simulation& System,				
			    const mainSystem::inputParam& IParam)
  /*!
    Create the decoupled item and build it
    \param System :: Simulation 
    \param IParam :: Parameters
   */
{
  ELog::RegMethod RegA("RefStructure","processDecoupled");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();


  if (IParam.flag("decFile"))
    {
      // This strange construct :
      DecFileMod* DFPtr=new DecFileMod("decoupled");
      DFPtr->createAllFromFile(System,*this,0,
          IParam.getValue<std::string>("decFile"));
      DMod=std::shared_ptr<Decoupled>(DFPtr);  
      return;
    }

  const std::string DT=IParam.getValue<std::string>("decType");
  if (DT=="standard")  // Standard one
    {
      std::shared_ptr<Decoupled> DModPtr(new Decoupled("decoupled"));
      //      OR.addObject(DModPtr);
      DModPtr->createAll(System,World::masterTS2Origin(),0);
      DMod=DModPtr;
    }
  else if (DT=="layer")  // layer
    {
      std::shared_ptr<Decoupled> DModPtr(new DecLayer("decoupled","decLayer")); 
      DModPtr->createAll(System,World::masterTS2Origin(),0);
      DMod=DModPtr;
    }
  else if (DT=="plate")  // layer
    {
      std::shared_ptr<insertSystem::insertPlate>
	PPtr(new insertSystem::insertPlate("decPlate")); 
       PPtr->setNoInsert();
       PPtr->addInsertCell(cellIndex-1);
       PPtr->createAll(System,World::masterTS2Origin(),0);
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
RefStructure::createInternalObjects(Simulation& System,
				 const mainSystem::inputParam& IParam)
  /*!
    Build the inner objects
    \param System :: Simulation to use
    \param IParam :: Input paramters for decoupled type
  */
{
  ELog::RegMethod RegA("RefStructure","createInternalObjects");

  GrooveObj->createAll(System,World::masterTS2Origin());
  HydObj->createAll(System,*GrooveObj,0);
  if (IParam.flag("orthoH"))
    {
      OrthoInsert OI("ortho");
      OI.createAll(System,*HydObj,*GrooveObj);
    }
  VacObj->createAllPair(System,*GrooveObj,*HydObj);
  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex,"-14 -2 -4");
  FLgroove->addBoundarySurf("inner",Out);  
  FLgroove->addBoundarySurf("outer",Out);  
  FLgroove->createAll(System,*VacObj,1);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 13");
  FLhydro->addBoundarySurf("inner",Out);  
  FLhydro->addBoundarySurf("outer",Out);  
  FLhydro->createAll(System,*VacObj,2);

  PMgroove->setTargetSurf(TarObj->getLinkSurf(1));
  PMgroove->setDivideSurf(VacObj->getDivideSurf());
  PMgroove->setEdge();
  PMgroove->createAll(System,*VacObj,6,0);

  PMhydro->setTargetSurf(TarObj->getLinkSurf(1));
  PMhydro->setDivideSurf(-VacObj->getDivideSurf());
  PMhydro->setEdge();
  PMhydro->createAll(System,*VacObj,6,1);

  Horn->setDivideSurf(-VacObj->getDivideSurf());
  Horn->createAll(System,*VacObj,*FLhydro,*PMhydro);
  
  processDecoupled(System,IParam);
  const attachSystem::ContainedComp* CMod=
    System.getObjectThrow<attachSystem::ContainedComp>
    (DMod->getKeyName(),"DMod to CC failed");

  if (DT!="plate")
    {
      DVacObj->createAll(System,*DMod,*CMod);

      Out=ModelSupport::getComposite(SMap,buildIndex,"-2 13 3");
      FLnarrow->addBoundarySurf("inner",Out);  
      FLnarrow->addBoundarySurf("outer",Out);  
      FLnarrow->createAll(System,*DVacObj,1);
      
      Out=ModelSupport::getComposite(SMap,buildIndex,"11 -4 -14");
      FLwish->addBoundarySurf("inner",Out);  
      FLwish->addBoundarySurf("outer",Out);
      FLwish->createAll(System,*DVacObj,2);
      
      PMdec->setTargetSurf(TarObj->getLinkSurf(1));
      PMdec->createAll(System,*DVacObj,5,1);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"-2 13 3");
      FLnarrow->addBoundarySurf("inner",Out);  
      FLnarrow->addBoundarySurf("outer",Out);  
      FLnarrow->createAll(System,*DMod,1);
      
      Out=ModelSupport::getComposite(SMap,buildIndex,"11 1 -14");
      FLwish->addBoundarySurf("inner",Out);  
      FLwish->addBoundarySurf("outer",Out);  
      FLwish->createAll(System,*DMod,2);
      
      PMdec->setTargetSurf(TarObj->getLinkSurf(1));
      PMdec->createAll(System,*DMod,6,1);
    }  
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 ");
  IRcut->addBoundarySurf(Out);  
  IRcut->createAll(System,*TarObj);
  
  CdBucket->addBoundarySurf(FLwish->getExclude("outer"));
  CdBucket->addBoundarySurf(FLnarrow->getExclude("outer"));
  CdBucket->addBoundarySurf(TarObj->getExclude());
  CdBucket->createAll(System,*this,0);

  for(CoolPad& PD : Pads)
    PD.createAll(System,*this,3);
      
  return;
}

void
RefStructure::insertPipeObjects(Simulation& System,
			     const mainSystem::inputParam& IParam)
  /*!
    Calls all the objects that own a pipe to be
    inserted
    \param System :: Simulation to use
    \param IParam :: Parameter table
  */
{
  ELog::RegMethod RegA("RefStructure","insertPipeObjects");
  return;
  CouplePipe CP("cplPipe");
  System.createObjSurfMap();
  CP.createAll(System,*HydObj,4,*VacObj);

  DecouplePipe DP("decPipe");
  //  DP.createAll(System,*DMod,5,*DVacObj,DMod->needsHePipe());

  if (IParam.flag("bolts"))
    {
      RefBolts RB("refBolt");
      RB.createAll(System,*this);
    }

  return;
}
  
int
RefStructure::calcModeratorPlanes(const int BeamLine,
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
  ELog::RegMethod RegA("RefStructure","calcModeratorPlanes");

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
RefStructure::getViewOrigin(const int BeamLine) const
  /*!
    Given the beamline calculate the defining view and the 
    active surface 
    \param BeamLine :: BeamLine number to use
    \return View point
  */
{
  ELog::RegMethod RegA("RefStructure","getViewOrigin");

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

std::string
RefStructure::getExclude() const 
  /*!
    Virtual function to add the cooling pads
    \return Full Exlcude path
  */
{
  ELog::RegMethod RegA("RefStructure","getExclude");

  std::string Out=ContainedComp::getExclude();
  for(const CoolPad& PD : Pads)
    Out+=PD.getExclude();
  return Out;
}

void
RefStructure::build(Simulation& System,
		    const mainSystem::inputParam& IParam)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param IParam :: Parameter table
  */
{
  ELog::RegMethod RegA("RefStructure","createAll");

  RefObj->createAll(World::masterOrigin(),0);
  
  const std::string TarName=
    IParam.getValue<std::string>("targetType",0);
  const std::string DT=IParam.getValue<std::string>("decType");

  TarObj->setRefPlates(-SMap.realSurf(buildIndex+12),
		       -SMap.realSurf(buildIndex+11));
  TarObj->createAll(System,World::masterTS2Origin(),0);

  TarObj->addProtonLineInsertCell(cellIndex-1);
  TarObj->addProtonLine(System,*this,-7);
	
  createInternalObjects(System,IParam);
  insertObjects(System);              

  return;
}

}  // NAMESPACE shutterSystem
