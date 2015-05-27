/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/makeESS.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <memory>

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
#include "stringCombine.h"
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
#include "CellMap.h"
#include "World.h"
#include "FlightLine.h"
#include "AttachSupport.h"
#include "pipeUnit.h"
#include "PipeLine.h"

#include "beamlineConstructor.h"
#include "WheelBase.h"
#include "Wheel.h"
#include "SegWheel.h"
#include "BeRef.h"
#include "ProtonTube.h"
#include "BeamMonitor.h"
#include "ModBase.h"
#include "ConicInfo.h"
#include "CylMod.h"
#include "H2Wing.h"
#include "ButterflyModerator.h"
#include "BlockAddition.h"
#include "CylPreMod.h"
#include "SupplyPipe.h"
#include "BulkModule.h"
#include "ShutterBay.h"
#include "GuideBay.h"
#include "DiskPreMod.h"

#include "ConicModerator.h"
#include "essDBMaterial.h"
#include "makeESSBL.h"

#include "makeESS.h"

namespace essSystem
{

makeESS::makeESS() :
  Reflector(new BeRef("BeRef")),
  PBeam(new ProtonTube("ProtonTube")),
  BMon(new BeamMonitor("BeamMonitor")),
  LowPreMod(new DiskPreMod("LowPreMod")),
  
  LowAFL(new moderatorSystem::FlightLine("LowAFlight")),
  LowBFL(new moderatorSystem::FlightLine("LowBFlight")),
  LowPre(new CylPreMod("LowPre")),

  LowSupplyPipe(new constructSystem::SupplyPipe("LSupply")),
  LowReturnPipe(new constructSystem::SupplyPipe("LReturn")),

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
  OR.addObject(LowPreMod);
  
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

  //  Out=Reflector->getLinkComplement(0)+LowPre->getBoxCut('A');
  //  LowAFL->addBoundarySurf("inner",Out);  
  //  LowAFL->addBoundarySurf("outer",Out);  
  //  LowAFL->addOuterSurf("outer",LowPre->getBoxCut('A'));
  
  LowAFL->createAll(System,*Reflector,2);   // Note system +1 

  /*
  Out=Reflector->getLinkComplement(0)+LowPre->getBoxCut('B');
  LowBFL->addBoundarySurf("inner",Out);  
  LowBFL->addBoundarySurf("outer",Out);  
  LowBFL->createAll(System,0,0,*LowPre);
  LowBFL->addOuterSurf("outer",LowPre->getBoxCut('B'));  
  */
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

  // Best place to put this to allow simple call
  if (targetType=="help")
    {
      ELog::EM<<"Target Type [Target]:"<<ELog::endBasic;
      ELog::EM<<"  -- Wheel       : Simple wheel form"<<ELog::endBasic;
      ELog::EM<<"  -- SegWheel    : Segmented wheel"<<ELog::endBasic;
      return;
    }


  if (targetType=="Wheel")
    Target=std::shared_ptr<WheelBase>(new Wheel("Wheel"));
  else if (targetType=="SegWheel")
    Target=std::shared_ptr<WheelBase>(new SegWheel("SegWheel"));
  else
    throw ColErr::InContainerError<std::string>(targetType,"Unknown target type");

  Target->addInsertCell("Shaft",voidCell);
  Target->addInsertCell("Wheel",voidCell);
  Target->createAll(System,World::masterOrigin());

  return;
}

void 
makeESS::createGuides(Simulation&)
  /*!
    Create all the guidebays and guides
    \param System :: Simulation system to use
   */
{
  ELog::RegMethod RegA("makeESS","createGuides");

  for(size_t i=0;i<4;i++)
    {
      std::shared_ptr<GuideBay> GB(new GuideBay("GuideBay",i+1));
      GB->addInsertCell("Inner",ShutterBayObj->getMainCell());
      GB->addInsertCell("Outer",ShutterBayObj->getMainCell());
      GB->setCylBoundary(Bulk->getLinkSurf(2),
			 ShutterBayObj->getLinkSurf(2));
      //      if(i<2)
      //	GB->createAll(System,*LowMod);  
      //      else
	//	GB->createAll(System,*TopMod);  
      GBArray.push_back(GB);
    }
  return;
}

void
makeESS::buildLowMod(Simulation& System)		   
  /*!
    Build the lower moderators
    \param System :: Simulation to build
  */
{
  ELog::RegMethod RegA("makeESS","buildLowMod");


  buildLowButterfly(System);
  // else 
  //   {
  //     buildLowCylMod(System);
  //     lowFlightLines(System);
  //     Bulk->addFlightUnit(System,*LowAFL);
  //     Bulk->addFlightUnit(System,*LowBFL);  
  //   }
  return;
}

void
makeESS::buildLowButterfly(Simulation& System)
  /*!
    Build the butterfly moderators
    \param System :: Stardard simulation
  */
{
  ELog::RegMethod RegA("makeESS","buildLowButteflyMod");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<ButterflyModerator> BM
    (new essSystem::ButterflyModerator("LowFly"));
  BM->setRadiusX(Reflector->getRadius());
  LowMod=std::shared_ptr<constructSystem::ModBase>(BM);
  OR.addObject(LowMod);
  
  LowMod->createAll(System,*Reflector,LowPreMod.get(),6);
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

  TopMod=std::shared_ptr<constructSystem::ModBase>
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
  
  if (pipeType=="help")
    {
      ELog::EM<<"Lower Pipe Configuration [lowPipe]"<<ELog::endBasic;
      ELog::EM<<"-- {Any} : Standard TDR edge and centre"<<ELog::endBasic;
      ELog::EM<<"-- Top : Two pipes from the top"<<ELog::endBasic;
      return;
    }

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
      ELog::EM<<"Low supply pipe"<<ELog::endDiag;
      //      LowSupplyPipe->createAll(System,*LowMod,0,6,4,*LowPre,2);
      //      LowReturnPipe->createAll(System,*LowMod,0,3,2,*LowPre,4);
    }
  return;
}


void
makeESS::optionSummary(Simulation& System)
  /*!
    Write summary of options
    \param System :: Dummy call variable 						
   */
{
  ELog::RegMethod RegA("makeESS","optionSummary");
  
  makeTarget(System,"help");
  buildLowerPipe(System,"help");
  
  return;
}

void
makeESS::makeBeamLine(Simulation& System,
		      const mainSystem::inputParam& IParam)
  /*!
    Build a beamline based on LineType
     -- to construct a beamline the name of the guide Item 
     and the beamline typename is required
    \param System :: Simulation 
    \param IParam :: Input paramters
  */
{
  ELog::RegMethod RegA("makeESS","makeBeamLine");

  const size_t NItems=IParam.itemCnt("beamlines",0);

  for(size_t i=0;i+1<NItems;i+=2)
    {
      const std::string BL=IParam.getValue<std::string>("beamlines",i);
      const std::string Btype=IParam.getValue<std::string>("beamlines",i+1);
      ELog::EM<<"Making beamline "<<BL
      	      <<" [" <<Btype<< "] "<<ELog::endDiag;
      makeESSBL BLfactory(BL,Btype);
      BLfactory.build(System,IParam);
    }
  return;
}


void 
makeESS::build(Simulation& System,
	       const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
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
  const std::string iradLine=IParam.getValue<std::string>("iradLineType");

  if (StrFunc::checkKey("help",lowPipeType,lowModType,targetType) ||
      StrFunc::checkKey("help",iradLine,topModType,""))
    {
      optionSummary(System);
      throw ColErr::ExitAbort("Help system exit");
    }

  
  makeTarget(System,targetType);
  Reflector->globalPopulate(System.getDataBase());
    
  LowPreMod->createAll(System,World::masterOrigin(),1,
		       Target->wheelHeight()/2.0,
		       Reflector->getRadius());
  buildLowButterfly(System);
  const double LMHeight=attachSystem::calcLinkDistance(*LowMod,5,6);
  
  Reflector->createAll(System,World::masterOrigin(),
		       Target->wheelHeight(),
		       LowPreMod->getHeight()+LMHeight,
		       -1.0);

  
  Reflector->insertComponent(System,"targetVoid",*Target,1);
  Reflector->deleteCell(System,"lowVoid");
  LowAFL->createAll(System,*LowMod,*Reflector,2);   // Note system +1
  
  Bulk->createAll(System,*Reflector,*Reflector);
  attachSystem::addToInsertSurfCtrl(System,*Bulk,Target->getKey("Wheel"));
  attachSystem::addToInsertForced(System,*Bulk,Target->getKey("Shaft"));


  // Full surround object
  ShutterBayObj->addInsertCell(voidCell);
  ShutterBayObj->createAll(System,*Bulk,*Bulk);
  attachSystem::addToInsertForced(System,*ShutterBayObj,
				  Target->getKey("Wheel"));
  attachSystem::addToInsertForced(System,*ShutterBayObj,
				  Target->getKey("Shaft"));

  createGuides(System);  

  // PROTON BEAMLINE
  

  PBeam->createAll(System,*Reflector,1,*ShutterBayObj,-1);
  // attachSystem::addToInsertSurfCtrl(System,*Reflector,
  // 				    PBeam->getKey("Sector0"));
  
  attachSystem::addToInsertSurfCtrl(System,*ShutterBayObj,
				    PBeam->getKey("Full"));
  attachSystem::addToInsertSurfCtrl(System,*Bulk,
				    PBeam->getKey("Full"));

  // BMon->createAll(System,*Target,1,*PBeam,"Sector");
  // attachSystem::addToInsertForced(System,*Reflector,*BMon);

  //  buildLowerPipe(System,lowPipeType);





  makeBeamLine(System,IParam);



  return;
}


}   // NAMESPACE essSystem

