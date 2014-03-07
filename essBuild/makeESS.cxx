/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/makeESS.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <utility>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LayerComp.h"
#include "World.h"
#include "FlightLine.h"
#include "AttachSupport.h"
#include "pipeUnit.h"
#include "PipeLine.h"

#include "WheelBase.h"
#include "Wheel.h"
#include "SegWheel.h"
#include "BeRef.h"
#include "ProtonTube.h"
#include "BeamMonitor.h"
#include "ModBase.h"
#include "ConicInfo.h"
#include "CylMod.h"
#include "BlockAddition.h"
#include "CylPreMod.h"
#include "SupplyPipe.h"
#include "BulkModule.h"
#include "ShutterBay.h"
#include "GuideBay.h"

#include "ConicModerator.h"
#include "essDBMaterial.h"

#include "makeESS.h"

namespace essSystem
{

makeESS::makeESS() :
  Reflector(new BeRef("BeRef")),
  PBeam(new ProtonTube("ProtonTube")),
  BMon(new BeamMonitor("BeamMonitor")),
  LowAFL(new moderatorSystem::FlightLine("LowAFlight")),
  LowBFL(new moderatorSystem::FlightLine("LowBFlight")),
  LowPre(new CylPreMod("LowPre")),
  LowSupplyPipe(new SupplyPipe("LSupply")),
  LowReturnPipe(new SupplyPipe("LReturn")),

  TopAFL(new moderatorSystem::FlightLine("TopAFlight")),
  TopBFL(new moderatorSystem::FlightLine("TopBFlight")),
  TopPre(new CylPreMod("TopPre")),

  Bulk(new BulkModule("Bulk")),
  BulkLowAFL(new moderatorSystem::FlightLine("BulkLAFlight")),
  ShutterBayObj(new ShutterBay("ShutterBay"))
 /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(Reflector);
  OR.addObject(PBeam);
  OR.addObject(BMon);
  OR.addObject(LowAFL);
  OR.addObject(LowBFL);
  OR.addObject(LowPre);
  OR.addObject(TopAFL);
  OR.addObject(TopBFL);
  OR.addObject(TopPre);

  OR.addObject(Bulk);
  OR.addObject(BulkLowAFL);

  OR.addObject(ShutterBayObj);
}


makeESS::~makeESS()
  /*!
    Destructor
  */
{}


void
makeESS::lowFlightLines(Simulation& System)
  /*!
    Build the flight lines of the reflector
    \param System :: Simulation to add to
  */
{
  ELog::RegMethod RegA("makeESS","lowFlightLines");

  std::string Out;

  Out=Reflector->getLinkComplement(0)+LowPre->getBoxCut('A');
  LowAFL->addBoundarySurf("inner",Out);  
  LowAFL->addBoundarySurf("outer",Out);  
  LowAFL->addOuterSurf("outer",LowPre->getBoxCut('A'));  
  LowAFL->createAll(System,1,*LowPre);
  attachSystem::addToInsertSurfCtrl(System,*LowAFL,*LowPre->getBox('A'));
  attachSystem::addToInsertSurfCtrl(System,*Reflector,
  				    LowAFL->getKey("outer"));

  Out=Reflector->getLinkComplement(0)+LowPre->getBoxCut('B');
  LowBFL->addBoundarySurf("inner",Out);  
  LowBFL->addBoundarySurf("outer",Out);  
  LowBFL->createAll(System,0,0,*LowPre);
  LowBFL->addOuterSurf("outer",LowPre->getBoxCut('B'));  

  attachSystem::addToInsertSurfCtrl(System,*LowBFL,*LowPre->getBox('B'));
  attachSystem::addToInsertSurfCtrl(System,*Reflector,
  				    LowBFL->getKey("outer"));
  attachSystem::addToInsertSurfCtrl(System,*LowBFL,
  				    LowAFL->getKey("outer"));
  return;
}

void
makeESS::topFlightLines(Simulation& System)
  /*!
    Build the flight lines of the reflector
    \param System :: Simulation to add to
  */
{
  ELog::RegMethod RegA("makeESS","topFlightLines");
  return;
  std::string Out;

  Out=Reflector->getLinkComplement(0)+TopPre->getBoxCut('A');
  TopAFL->addBoundarySurf("inner",Out);  
  TopAFL->addBoundarySurf("outer",Out);  
  TopAFL->addOuterSurf("outer",TopPre->getBoxCut('A'));  
  TopAFL->createAll(System,0,1,*TopPre);
  attachSystem::addToInsertSurfCtrl(System,*TopAFL,*TopPre->getBox('A'));
  attachSystem::addToInsertSurfCtrl(System,*Reflector,
  				    TopAFL->getKey("outer"));

  Out=Reflector->getLinkComplement(0)+TopPre->getBoxCut('B');
  TopBFL->addBoundarySurf("inner",Out);  
  TopBFL->addBoundarySurf("outer",Out);  
  TopBFL->createAll(System,0,0,*TopPre);
  TopBFL->addOuterSurf("outer",TopPre->getBoxCut('B'));  
  attachSystem::addToInsertSurfCtrl(System,*TopBFL,*TopPre->getBox('B'));
  attachSystem::addToInsertSurfCtrl(System,*Reflector,
  				    TopBFL->getKey("outer"));


  return;
}


void
makeESS::makeTarget(Simulation& System,
		    const std::string& targetType)
  /*!
    Build the different ESS targets
    \param System :: Simulation
    \param targetType :: Type of target
  */
{
  ELog::RegMethod RegA("makeESS","makeTarget");

  const int voidCell(74123);  

  if (targetType=="Wheel")
    Target=boost::shared_ptr<WheelBase>(new Wheel("Wheel"));
  else if (targetType=="SegWheel")
    Target=boost::shared_ptr<WheelBase>(new SegWheel("SegWheel"));
  else
    throw ColErr::InContainerError<std::string>(targetType,"Unknown target type");

  Target->addInsertCell("Shaft",voidCell);
  Target->addInsertCell("Wheel",voidCell);
  Target->createAll(System,World::masterOrigin());

  return;
}

void 
makeESS::createGuides(Simulation& System)
  /*!
    Create all the guidebays and guides
    \param System :: Simulation system to use
   */
{
  ELog::RegMethod RegA("makeESS","createGuides");

  for(size_t i=0;i<4;i++)
    {
      boost::shared_ptr<GuideBay> GB(new GuideBay("GuideBay",i+1));
      GB->addInsertCell("Inner",ShutterBayObj->getMainCell());
      GB->addInsertCell("Outer",ShutterBayObj->getMainCell());
      GB->setCylBoundary(Bulk->getLinkSurf(2),
			 ShutterBayObj->getLinkSurf(2));
      if(i<2)
	GB->createAll(System,*LowMod);  
      //      else
	//	GB->createAll(System,*TopMod);  
      GBArray.push_back(GB);
    }
  return;
}

void
makeESS::buildLowMod(Simulation& System,
		     const std::string& ModType)
  /*!
    Build the lower moderators
    \param System :: Simulation to build
    \param ModType :: Moderator Type
  */
{
  ELog::RegMethod RegA("makeESS","buildLowMod");

  if (ModType=="Cone")
    {
      buildLowConicMod(System);
    }
  else 
    {
      buildLowCylMod(System);
      lowFlightLines(System);
    }
  return;
}

void
makeESS::buildLowCylMod(Simulation& System)
  /*!
    Build the standard moderators
    \param System :: Stardard simulation
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  LowMod=boost::shared_ptr<constructSystem::ModBase>
    (new constructSystem::CylMod("LowMod"));

  OR.addObject(LowMod);

  LowMod->createAll(System,*Reflector);
  attachSystem::addToInsertControl(System,*Reflector,*LowMod);

  LowPre->createAll(System,*LowMod);
  attachSystem::addToInsertControl(System,*Reflector,*LowPre,"Main");
  attachSystem::addToInsertControl(System,*Reflector,*LowPre,"BlockA");
  attachSystem::addToInsertControl(System,*Reflector,*LowPre,"BlockB");


  return;
}


void
makeESS::buildLowConicMod(Simulation& System)
  /*!
    Builds the lower conic moderator
    \param System :: simulation to add
  */
{
  ELog::RegMethod RegA("makeESS","buildLowConicMod");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  LowMod=boost::shared_ptr<constructSystem::ModBase>
    (new ConicModerator("LowConeMod"));
  OR.addObject(LowMod);
    
  LowMod->createAll(System,*Reflector);
  attachSystem::addToInsertControl(System,*Reflector,*LowMod);

  std::string Out=Reflector->getLinkComplement(0);
  LowAFL->addBoundarySurf("inner",Out);  
  LowAFL->addBoundarySurf("outer",Out);  
  LowAFL->createAll(System,1,*LowMod);
  attachSystem::addToInsertSurfCtrl(System,*Reflector,
  				    LowAFL->getKey("outer"));
    
  LowModB=boost::shared_ptr<constructSystem::ModBase>
    (new ConicModerator("LowConeModB"));
  OR.addObject(LowModB);

  LowModB->createAll(System,*Reflector);
  attachSystem::addToInsertControl(System,*Reflector,*LowModB);
  attachSystem::addToInsertSurfCtrl(System,*LowModB,*LowMod);
  Out=Reflector->getLinkComplement(0);
  LowBFL->addBoundarySurf("inner",Out);  
  LowBFL->addBoundarySurf("outer",Out);  
  LowBFL->createAll(System,1,*LowModB);
  attachSystem::addToInsertSurfCtrl(System,*Reflector,
  				    LowBFL->getKey("outer"));
  return;
}

void
makeESS::buildTopCylMod(Simulation& System)
  /*!
    Build the standard moderators
    \param System :: Stardard simulation
  */
{
  ELog::RegMethod RegA("makeESS","buildTopCylMod");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  TopMod=boost::shared_ptr<constructSystem::ModBase>
    (new constructSystem::CylMod("TopMod"));

  OR.addObject(TopMod);

  TopMod->createAll(System,*Reflector);
  attachSystem::addToInsertControl(System,*Reflector,*TopMod);

  TopPre->createAll(System,*TopMod);
  attachSystem::addToInsertControl(System,*Reflector,*TopPre,"Main");
  attachSystem::addToInsertControl(System,*Reflector,*TopPre,"BlockA");
  attachSystem::addToInsertControl(System,*Reflector,*TopPre,"BlockB");
  
  topFlightLines(System);

  return;
}

void 
makeESS::buildLowerPipe(Simulation& System,
			const std::string& pipeType)
  /*!
    Process the lower moderator pipe
    \param System :: Simulation 
    \param pipeType :: pipeType 
  */
{
  ELog::RegMethod RegA("makeESS","processLowPipe");

  LowReturnPipe->setAngleSeg(12);
  if (pipeType=="Top")
    {
      LowSupplyPipe->setOption("Top");
      LowReturnPipe->setOption("Top");
      LowSupplyPipe->createAll(System,*LowMod,0,6,4,*LowPre,2);
      LowReturnPipe->createAll(System,*LowMod,0,5,4,*LowPre,2);
    }
  else
    {
      LowSupplyPipe->createAll(System,*LowMod,0,6,4,*LowPre,2);
      LowReturnPipe->createAll(System,*LowMod,0,3,2,*LowPre,4);
    }
  return;
}


void
makeESS::optionSummary() const
  /*!
    write summary of options
   */
{
  ELog::RegMethod RegA("makeESS","optionSummary");

  ELog::EM<<"Target Type [targetT: "<<ELog::endBasic;
  ELog::EM<<"  -- Wheel       : Simple wheel form"<<ELog::endBasic;
  ELog::EM<<"  -- SegWheel    : Segmented wheel"<<ELog::endBasic;
  return;
}

void 
makeESS::build(Simulation* SimPtr,
	       const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RegA("makeESS","build");

  int voidCell(74123);
  // Add extra materials to the DBdatabase
  ModelSupport::addESSMaterial();

  const std::string lowPipeType=IParam.getValue<std::string>("lowPipe");
  const std::string lowModType=IParam.getValue<std::string>("lowMod");
  const std::string topModType=IParam.getValue<std::string>("topMod");
  const std::string targetType=IParam.getValue<std::string>("targetType");

  makeTarget(*SimPtr,targetType);
  
  Reflector->createAll(*SimPtr,World::masterOrigin());
  attachSystem::addToInsertForced(*SimPtr,*Reflector,Target->getKey("Wheel"));

  Bulk->createAll(*SimPtr,*Reflector,*Reflector);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*Bulk,Target->getKey("Wheel"));
  attachSystem::addToInsertForced(*SimPtr,*Bulk,Target->getKey("Shaft"));

  buildLowMod(*SimPtr,lowModType);

  Bulk->addFlightUnit(*SimPtr,*LowAFL);
  Bulk->addFlightUnit(*SimPtr,*LowBFL);  

  buildTopCylMod(*SimPtr);

  // Bulk->addFlightUnit(*SimPtr,*TopAFL);
  // Bulk->addFlightUnit(*SimPtr,*TopBFL);

  // Full surround object
  ShutterBayObj->addInsertCell(voidCell);
  ShutterBayObj->createAll(*SimPtr,*Bulk,*Bulk);
  attachSystem::addToInsertForced(*SimPtr,*ShutterBayObj,
				  Target->getKey("Wheel"));
  attachSystem::addToInsertForced(*SimPtr,*ShutterBayObj,
				  Target->getKey("Shaft"));

  createGuides(*SimPtr);  
  
  // PROTON BEAMLINE
  
  
  PBeam->createAll(*SimPtr,*Target,1,*ShutterBayObj,-1);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*Reflector,
				    PBeam->getKey("Sector0"));
  
  attachSystem::addToInsertSurfCtrl(*SimPtr,*ShutterBayObj,
				    PBeam->getKey("Full"));
  attachSystem::addToInsertSurfCtrl(*SimPtr,*Bulk,
				    PBeam->getKey("Full"));

  // BMon->createAll(*SimPtr,*Target,1,*PBeam,"Sector");
  // attachSystem::addToInsertForced(*SimPtr,*Reflector,*BMon);

  buildLowerPipe(*SimPtr,lowPipeType);
  
  return;
}


}   // NAMESPACE essSystem

