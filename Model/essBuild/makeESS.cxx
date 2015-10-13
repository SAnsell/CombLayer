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
#include "BasicFlightLine.h"
#include "FlightLine.h"
#include "AttachSupport.h"
#include "pipeUnit.h"
#include "PipeLine.h"

#include "beamlineConstructor.h"
#include "WheelBase.h"
#include "Wheel.h"
#include "BilbaoWheel.h"
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
#include "Bunker.h"

#include "ConicModerator.h"
#include "essDBMaterial.h"
#include "makeESSBL.h"
// F5 collimators:
#include "F5Calc.h"
#include "F5Collimator.h"
// BEAMLINES:
#include "ODIN.h"
#include "LOKI.h"
#include "NMX.h"

#include "makeESS.h"

namespace essSystem
{

makeESS::makeESS() :
  Reflector(new BeRef("BeRef")),
  PBeam(new ProtonTube("ProtonTube")),
  BMon(new BeamMonitor("BeamMonitor")),

  LowPreMod(new DiskPreMod("LowPreMod")),
  LowCapMod(new DiskPreMod("LowCapMod")),
  
  LowAFL(new moderatorSystem::BasicFlightLine("LowAFlight")),
  LowBFL(new moderatorSystem::BasicFlightLine("LowBFlight")),

  LowSupplyPipe(new constructSystem::SupplyPipe("LSupply")),
  LowReturnPipe(new constructSystem::SupplyPipe("LReturn")),

  TopPreMod(new DiskPreMod("TopPreMod")),
  TopCapMod(new DiskPreMod("TopCapMod")),

  TopAFL(new moderatorSystem::BasicFlightLine("TopAFlight")),
  TopBFL(new moderatorSystem::BasicFlightLine("TopBFlight")),

  Bulk(new BulkModule("Bulk")),
  BulkLowAFL(new moderatorSystem::FlightLine("BulkLAFlight")),
  ShutterBayObj(new ShutterBay("ShutterBay")),

  ABunker(new Bunker("ABunker"))
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
  OR.addObject(LowCapMod);
  
  OR.addObject(LowAFL);
  OR.addObject(LowBFL);
  OR.addObject(TopPreMod);
  OR.addObject(TopCapMod);

  OR.addObject(TopAFL);
  OR.addObject(TopBFL);

  OR.addObject(Bulk);
  OR.addObject(BulkLowAFL);

  OR.addObject(ShutterBayObj);
  OR.addObject(ABunker);
}


makeESS::~makeESS()
  /*!
    Destructor
  */
{}

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
      ELog::EM<<"  -- Wheel     : Simple wheel form"<<ELog::endBasic;
      ELog::EM<<"  -- Bilbao    : Bilbao wheel"<<ELog::endBasic;
      return;
    }


  if (targetType=="Wheel")
    Target=std::shared_ptr<WheelBase>(new Wheel("Wheel"));
  else if (targetType=="Bilbao")
    Target=std::shared_ptr<WheelBase>(new BilbaoWheel("BilbaoWheel"));
  else
    throw ColErr::InContainerError<std::string>
      (targetType,"Unknown target type");

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
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for(size_t i=0;i<4;i++)
    {
      std::shared_ptr<GuideBay> GB(new GuideBay("GuideBay",i+1));
      OR.addObject(GB);
      GB->addInsertCell("Inner",ShutterBayObj->getMainCell());
      GB->addInsertCell("Outer",ShutterBayObj->getMainCell());
      GB->setCylBoundary(Bulk->getLinkSurf(2),
			 ShutterBayObj->getLinkSurf(2));
      
      if (i<2)
	GB->createAll(System,*LowMod);  
      else
	GB->createAll(System,*TopMod);
      
      GBArray.push_back(GB);
      attachSystem::addToInsertForced(System,*GB, Target->getCC("Wheel"));
    }
  GBArray[1]->outerMerge(System,*GBArray[2]);
  GBArray[0]->outerMerge(System,*GBArray[3]);
  for(size_t i=0;i<4;i++)
    GBArray[i]->createGuideItems(System);



  return;
}

void
makeESS::buildLowButterfly(Simulation& System)
  /*!
    Build the lower butterfly moderator
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
  LowFocalPoints = BM->getFocalPoints();
  return;
}

void
makeESS::buildTopButterfly(Simulation& System)
  /*!
    Build the upper butterfly moderator
    \param System :: Stardard simulation
  */
{
  ELog::RegMethod RegA("makeESS","buildTopButteflyMod");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<ButterflyModerator> BM
    (new essSystem::ButterflyModerator("TopFly"));

  BM->setRadiusX(Reflector->getRadius());
  TopMod=std::shared_ptr<constructSystem::ModBase>(BM);
  OR.addObject(TopMod);
  
  TopMod->createAll(System,*Reflector,TopPreMod.get(),6);

  TopFocalPoints = BM->getFocalPoints();

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

void makeESS::buildF5Collimator(Simulation& System, size_t nF5)
/*!
  Build F5 collimators
  \param System :: Stardard simulation
 */
{
  ELog::RegMethod RegA("makeESS", "buildF5Collimator");
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();

  for (size_t i=0; i<nF5; i++) {
    std::shared_ptr<F5Collimator> F5(new F5Collimator(StrFunc::makeString("F", i*10+5).c_str()));
    OR.addObject(F5);
    F5->setFocalPoints(TopFocalPoints);
    F5->addInsertCell(74123); // !!! 74123=voidCell // SA: how to exclude F5 from any cells?
    F5->createAll(System, World::masterOrigin());

    attachSystem::addToInsertSurfCtrl(System, *ABunker, *F5);
    F5array.push_back(F5);
  }

  return;
}

void makeESS::buildF5Collimator(Simulation& System, const mainSystem::inputParam& IParam)
/*!
  Build F5 collimators
  \param System :: Stardard simulation
  \param IParam :: command line parameters. Example: --f5-collimators {top,low} {cold,thermal} theta1 theta2 theta3 ...
 */
{
  ELog::RegMethod RegA("makeESS", "buildF5Collimator");
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();

  const std::string moderator(IParam.getValue<std::string>("f5-collimators", 0));
  const std::string range(IParam.getValue<std::string>("f5-collimators", 1));

  const size_t nColl = IParam.itemCnt("f5-collimators",0)-2; // number of collimators to build (-2 due to Mod and Range)
  ELog::EM << moderator << " " << range << " " << nColl << ELog::endDiag;
  //  return;
  double theta(0.0);
  for (size_t i=0; i<nColl; i++)
    {
      theta = IParam.getValue<double>("f5-collimators", i+2);
      std::cout << theta << std::endl;
      std::shared_ptr<F5Collimator> F5(new F5Collimator(StrFunc::makeString("F", i*10+5).c_str()));
      OR.addObject(F5);
      F5->setTheta(theta);
      F5->setRange(range);
      F5->setFocalPoints(TopFocalPoints);
      F5->addInsertCell(74123); // !!! 74123=voidCell // SA: how to exclude F5 from any cells?
      F5->createAll(System, World::masterOrigin());
      
      attachSystem::addToInsertSurfCtrl(System, *ABunker, *F5);
      F5array.push_back(F5);
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

  const size_t NSet=IParam.setCnt("beamlines");

  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("beamlines",0);
      for(size_t i=1;i<NItems;i+=2)
	{
	  const std::string BL=IParam.getValue<std::string>("beamlines",j,i-1);
	  const std::string Btype=IParam.getValue<std::string>("beamlines",j,i);
	  // FIND BUNKER HERE:::
	  
	  makeESSBL BLfactory(BL,Btype);
	  BLfactory.build(System,*ABunker);
	    
	}
    }
  return;
}

void
makeESS::makeBunker(Simulation& System,
		    const std::string& bunkerType)
  /*!
    Make the bunker system
    \param System :: Simulation 
    \param bunkerType :: different bunker to make
  */
{
  ELog::RegMethod RegA("makeESS","makeBunker");

  ELog::EM<<"Bunker == "<<bunkerType<<ELog::endDiag;
  
  ABunker->addInsertCell(74123);
  ABunker->createAll(System,*LowMod,*GBArray[0],2,true);

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
  const std::string topPipeType=IParam.getValue<std::string>("topPipe");
  
  const std::string targetType=IParam.getValue<std::string>("targetType");
  const std::string iradLine=IParam.getValue<std::string>("iradLineType");
  const std::string bunker=IParam.getValue<std::string>("bunkerType");

  const size_t nF5 = IParam.getValue<int>("nF5");

  if (StrFunc::checkKey("help",lowPipeType,lowModType,targetType) ||
      StrFunc::checkKey("help",iradLine,topModType,""))
    {
      optionSummary(System);
      throw ColErr::ExitAbort("Help system exit");
    }
  
  makeTarget(System,targetType);
  Reflector->globalPopulate(System.getDataBase());

  // lower moderator
  LowPreMod->createAll(System,World::masterOrigin(),0,true,
		       Target->wheelHeight()/2.0,
		       Reflector->getRadius());

  TopPreMod->createAll(System,World::masterOrigin(),0,false,
		       Target->wheelHeight()/2.0,
		       Reflector->getRadius());

  buildLowButterfly(System);
  buildTopButterfly(System);
  const double LMHeight=attachSystem::calcLinkDistance(*LowMod,5,6);
  const double TMHeight=attachSystem::calcLinkDistance(*TopMod,5,6);
  // Cap moderator DOES not span whole unit
  LowCapMod->createAll(System,*LowMod,6,false,
   		       0.0,Reflector->getRadius());
  TopCapMod->createAll(System,*TopMod,6,false,
   		       0.0,Reflector->getRadius());
  Reflector->createAll(System,World::masterOrigin(),
		       Target->wheelHeight(),
		       LowPreMod->getHeight()+LMHeight+LowCapMod->getHeight(),
		       TopPreMod->getHeight()+TMHeight+TopCapMod->getHeight());

  Reflector->insertComponent(System,"targetVoid",*Target,1);
  Reflector->deleteCell(System,"lowVoid");
  Reflector->deleteCell(System,"topVoid");
  Bulk->createAll(System,*Reflector,*Reflector);

  // Build flightlines after bulk
  TopAFL->createAll(System,*TopMod,0,*Reflector,4,*Bulk,-3);
  TopBFL->createAll(System,*TopMod,0,*Reflector,3,*Bulk,-3);

  LowAFL->createAll(System,*LowMod,0,*Reflector,4,*Bulk,-3);
  LowBFL->createAll(System,*LowMod,0,*Reflector,3,*Bulk,-3);   

  attachSystem::addToInsertSurfCtrl(System,*Bulk,Target->getCC("Wheel"));
  attachSystem::addToInsertForced(System,*Bulk,Target->getCC("Shaft"));
  attachSystem::addToInsertForced(System,*Bulk,LowAFL->getCC("outer"));
  attachSystem::addToInsertForced(System,*Bulk,LowBFL->getCC("outer"));
  attachSystem::addToInsertForced(System,*Bulk,TopAFL->getCC("outer"));
  attachSystem::addToInsertForced(System,*Bulk,TopBFL->getCC("outer"));


  // Full surround object
  ShutterBayObj->addInsertCell(voidCell);
  ShutterBayObj->createAll(System,*Bulk,*Bulk);
  attachSystem::addToInsertForced(System,*ShutterBayObj,
				  Target->getCC("Wheel"));
  attachSystem::addToInsertForced(System,*ShutterBayObj,
				  Target->getCC("Shaft"));


  createGuides(System);
  makeBunker(System,bunker);

  // PROTON BEAMLINE
  

  PBeam->createAll(System,*Reflector,1,*ShutterBayObj,-1);
  // attachSystem::addToInsertSurfCtrl(System,*Reflector,
  // 				    PBeam->getCC("Sector0"));
  
  attachSystem::addToInsertSurfCtrl(System,*ShutterBayObj,
				    PBeam->getCC("Full"));
  attachSystem::addToInsertSurfCtrl(System,*Bulk,
				    PBeam->getCC("Full"));

  makeBeamLine(System,IParam);

  //  buildF5Collimator(System, nF5);
  buildF5Collimator(System, IParam);
  return;
}


}   // NAMESPACE essSystem

