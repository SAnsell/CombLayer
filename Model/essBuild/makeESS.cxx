/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/makeESS.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Konstantin Batkov
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
#include <array>
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
#include "support.h"
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
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "FrontBackCut.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "BasicFlightLine.h"
#include "FlightLine.h"
#include "WedgeFlightLine.h"
#include "AttachSupport.h"
#include "LinkSupport.h"
#include "pipeUnit.h"
#include "PipeLine.h"

#include "FocusPoints.h"
#include "beamlineConstructor.h"
#include "WheelBase.h"
#include "Wheel.h"
#include "BilbaoWheel.h"
#include "BeRef.h"
#include "TelescopicPipe.h"
#include "BeamMonitor.h"
#include "EssModBase.h"
#include "ConicInfo.h"
#include "H2Wing.h"
#include "ButterflyModerator.h"
#include "PancakeModerator.h"
#include "BoxModerator.h"
#include "BlockAddition.h"
#include "CylPreMod.h"
#include "PreModWing.h"
#include "IradCylinder.h"
#include "BulkModule.h"
#include "TwisterModule.h"
#include "ShutterBay.h"
#include "GuideBay.h"
#include "DiskPreMod.h"
#include "DiskLayerMod.h"
#include "PBIP.h"
#include "Bunker.h"
#include "pillarInfo.h"
#include "RoofPillars.h"
#include "BunkerFeed.h"
#include "BunkerQuake.h"
#include "Curtain.h"
#include "HighBay.h"
#include "makeESSBL.h"
#include "ESSPipes.h"
#include "F5Calc.h"
#include "F5Collimator.h"
#include "TSMainBuilding.h"
#include "Chicane.h"
#include "makeESS.h"

namespace essSystem
{

makeESS::makeESS() :
  Reflector(new BeRef("BeRef")),
  PBeam(new TelescopicPipe("ProtonTube")),
  pbip(new PBIP("PBIP")),
  BMon(new BeamMonitor("BeamMonitor")),

  topFocus(new FocusPoints("TopFocus")),
  lowFocus(new FocusPoints("LowFocus")),
  LowPreMod(new DiskLayerMod("LowPreMod")),
  LowCapMod(new DiskLayerMod("LowCapMod")),
  
  LowAFL(new essSystem::WedgeFlightLine("LowAFlight")),
  LowBFL(new essSystem::WedgeFlightLine("LowBFlight")),

  TopPreMod(new DiskLayerMod("TopPreMod")),
  TopCapMod(new DiskLayerMod("TopCapMod")),

  TopAFL(new essSystem::WedgeFlightLine("TopAFlight")),
  TopBFL(new essSystem::WedgeFlightLine("TopBFlight")),
  ModPipes(new ESSPipes()),

  Bulk(new BulkModule("Bulk")),
  ShutterBayObj(new ShutterBay("ShutterBay")),

  ABunker(new Bunker("ABunker")),
  BBunker(new Bunker("BBunker")),
  CBunker(new Bunker("CBunker")),
  DBunker(new Bunker("DBunker")),
  ABunkerPillars(new RoofPillars("ABunkerPillars")),
  BBunkerPillars(new RoofPillars("BBunkerPillars")),
  TopCurtain(new Curtain("Curtain")),
  
  ABHighBay(new HighBay("ABHighBay")),
  CDHighBay(new HighBay("CDHighBay")),

  TSMainBuildingObj(new TSMainBuilding("TSMainBuilding"))

 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(Reflector);
  OR.addObject(PBeam);
  OR.addObject(pbip);
  OR.addObject(BMon);
  OR.addObject(topFocus);
  OR.addObject(lowFocus);
  
  OR.addObject(LowPreMod);
  OR.addObject(LowCapMod);
  
  OR.addObject(LowAFL);
  OR.addObject(LowBFL);

  OR.addObject(TopPreMod);
  OR.addObject(TopCapMod);


  OR.addObject(TopAFL);
  OR.addObject(TopBFL);

  OR.addObject(Bulk);
  OR.addObject(TSMainBuildingObj);

  OR.addObject(ShutterBayObj);
  OR.addObject(ABunker);
  OR.addObject(BBunker);
  OR.addObject(CBunker);
  OR.addObject(DBunker);
  OR.addObject(ABunkerPillars);
  OR.addObject(BBunkerPillars);
  OR.addObject(TopCurtain);
  OR.addObject(ABHighBay);
  OR.addObject(CDHighBay);
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

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
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
  Target->createAll(System,World::masterOrigin(),0);

  OR.addObject(Target);
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

  for(size_t i=0;i<2;i++)
    {
      std::shared_ptr<GuideBay> GB(new GuideBay("GuideBay",i+1));
      OR.addObject(GB);
      GB->addInsertCell("Inner",ShutterBayObj->getCell("MainCell"));
      GB->addInsertCell("Outer",ShutterBayObj->getCell("MainCell"));
      GB->setCylBoundary(Bulk->getLinkSurf(3),
			 ShutterBayObj->getLinkSurf(7));

      GB->createAll(System,*ShutterBayObj,0);  
      attachSystem::addToInsertForced(System,*GB,Target->getCC("Wheel"));      
      GBArray.push_back(GB);
      attachSystem::addToInsertForced(System,*GB,Target->getCC("Wheel"));
    }
  
  GBArray[0]->createGuideItems(System,"Top",Target->getKeyName());
  GBArray[0]->createGuideItems(System,"Low",Target->getKeyName());
  GBArray[1]->createGuideItems(System,"Top",Target->getKeyName());
  GBArray[1]->createGuideItems(System,"Low",Target->getKeyName());

  return;
}

void
makeESS::buildIradComponent(Simulation& System,
                            const mainSystem::inputParam& IParam)
  /*!
    Build the Iradiation component. It is an object
    within a moderator (typically). Currently not saves in 
    the class set.
    \param System :: Simulation
    \param IParam :: Name of Irad component + location
   */
{
  ELog::RegMethod RegA("makeESS","buildIradComponent");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const size_t NSet=IParam.setCnt("iradObject");
  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("iradObject",j);
      // is this possible?
      if (NItems<3)
        throw ColErr::SizeError<size_t>
          (NItems,3,"IradComp["+StrFunc::makeString(j)+"]");

      const std::string objectName=
        IParam.getValue<std::string>("iradObj",j,0);
      const std::string compName=
        IParam.getValue<std::string>("iradObj",j,1);
      const std::string linkName=
        IParam.getValue<std::string>("iradObj",j,2);

      // First do we have an object name
      if (objectName.substr(0,7)=="IradCyl")
        {
          std::shared_ptr<IradCylinder>
            IRadComp(new IradCylinder(objectName));
          const attachSystem::FixedComp* FC=
            OR.getObject<attachSystem::FixedComp>(compName);
          if (!FC)
            throw ColErr::InContainerError<std::string>
              (compName,"Component not found");
          const long int linkPt=attachSystem::getLinkNumber(linkName);
          
          OR.addObject(IRadComp);
          IRadComp->createAll(System,*FC,linkPt);
          attachSystem::addToInsertLineCtrl(System,*FC,*IRadComp);
        }
    }
  return;
}


void
makeESS::buildFocusPoints(Simulation& System)
  /*!
    Construct the focus points [moderators change -- 
    focus points don't]
    \param System :: Simulation system
   */
{
  ELog::RegMethod RegA("makeESS","buildFocusPoints");

  topFocus->createAll(System,World::masterOrigin(),0);
  lowFocus->createAll(System,World::masterOrigin(),0);
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
  
  LowMod=std::shared_ptr<EssModBase>(BM);
  OR.addObject(LowMod);
  LowMod->createAll(System,*LowPreMod,6,*Reflector,0);
  
  return;
}

void
makeESS::buildTopButterfly(Simulation& System)
  /*!
    Build the top butterfly moderator
    \param System :: Stardard simulation
  */
{
  ELog::RegMethod RegA("makeESS","buildTopButteflyMod");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<ButterflyModerator> BM
    (new essSystem::ButterflyModerator("TopFly"));
  BM->setRadiusX(Reflector->getRadius());

  TopMod=std::shared_ptr<EssModBase>(BM);
  OR.addObject(TopMod);

  TopMod->createAll(System,*TopPreMod,6,*Reflector,0);
  return;
}
      
void
makeESS::buildLowPancake(Simulation& System)
  /*!
    Build the lower pancake moderator
    \param System :: Stardard simulation
  */
{
  ELog::RegMethod RegA("makeESS","buildLowPancake");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<PancakeModerator> BM
    (new essSystem::PancakeModerator("LowCake"));
  BM->setRadiusX(Reflector->getRadius());

  LowMod=std::shared_ptr<EssModBase>(BM);
  OR.addObject(LowMod);
  LowMod->createAll(System,*LowPreMod,6,*Reflector,0);
  return;
}

  
void
makeESS::buildTopPancake(Simulation& System)
  /*!
    Build the top pancake moderator
    \param System :: Stardard simulation
  */
{
  ELog::RegMethod RegA("makeESS","buildTopPancake");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<PancakeModerator> BM
    (new essSystem::PancakeModerator("TopCake"));
  BM->setRadiusX(Reflector->getRadius());
  TopMod=std::shared_ptr<EssModBase>(BM);
  OR.addObject(TopMod);
  
  TopMod->createAll(System,*TopPreMod,6,*Reflector,0);
  return;
}

void
makeESS::buildLowBox(Simulation& System)
  /*!
    Build the lower box moderator
    \param System :: Stardard simulation
  */
{
  ELog::RegMethod RegA("makeESS","buildLowBox");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<BoxModerator> BM
    (new essSystem::BoxModerator("LowBox"));
  BM->setRadiusX(Reflector->getRadius());
  LowMod=std::shared_ptr<EssModBase>(BM);
  OR.addObject(LowMod);
  LowMod->createAll(System,*LowPreMod,6,*Reflector,0);
  return;
}

  
void
makeESS::buildTopBox(Simulation& System)
  /*!
    Build the top box moderator
    \param System :: Stardard simulation
  */
{
  ELog::RegMethod RegA("makeESS","buildTopBox");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<BoxModerator> BM
    (new essSystem::BoxModerator("TopBox"));
  BM->setRadiusX(Reflector->getRadius());
  TopMod=std::shared_ptr<EssModBase>(BM);
  OR.addObject(TopMod);
  
  TopMod->createAll(System,*TopPreMod,6,*Reflector,0);
  return;
}

void
makeESS::buildF5Collimator(Simulation& System,const size_t nF5)
 /*!
   Build F5 collimators
   \param System :: Stardard simulation
   \param nF5 :: number of collimators to build
 */
{
  ELog::RegMethod RegA("makeESS", "buildF5Collimator");
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();

  for (size_t i=0; i<nF5; i++)
    {
      std::shared_ptr<F5Collimator>
        F5(new F5Collimator(StrFunc::makeString("F", i*10+5).c_str()));
      OR.addObject(F5);
      F5->addInsertCell(74123); // !!! 74123=voidCell // SA: how to exclude F5 from any cells?
      F5->createAll(System,World::masterOrigin());
      attachSystem::addToInsertSurfCtrl(System,*ABunker,*F5);
      F5array.push_back(F5);
    }

  return;
}


void
makeESS::buildBunkerFeedThrough(Simulation& System,
                                const mainSystem::inputParam& IParam)
  /*!
    Build the underground feedThroughs
    \param System :: Simulation
    \param IParam :: Input data
  */
{
  ELog::RegMethod RegA("makeESS","buildBunkerFeedThrough");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const size_t NSet=IParam.setCnt("bunkerFeed");

  ELog::EM<<"Calling bunker Feed"<<ELog::endDiag;
  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("bunkerFeed",j);
      if (NItems>=3)
        {
          const std::string bunkerName=
            IParam.getValue<std::string>("bunkerFeed",j,0);
          const size_t segNumber=
            IParam.getValue<size_t>("bunkerFeed",j,1);
          const std::string feedName=
            IParam.getValue<std::string>("bunkerFeed",j,2);

          // bunkerA/etc should be a map
          std::shared_ptr<Bunker> BPtr;
          if (bunkerName=="BunkerA" || bunkerName=="ABunker")
            BPtr=ABunker;
          else if (bunkerName=="BunkerB" || bunkerName=="BBunker")
            BPtr=BBunker;
          else if (bunkerName=="BunkerC" || bunkerName=="CBunker")
            BPtr=CBunker;
          else if (bunkerName=="BunkerD" || bunkerName=="DBunker")
            BPtr=DBunker;
          else
            throw ColErr::InContainerError<std::string>
              (bunkerName,"bunkerName not know");
          
          std::shared_ptr<BunkerFeed> BF
            (new BunkerFeed("BunkerFeed",j));
          OR.addObject(BF);
          BF->createAll(System,*BPtr,segNumber,feedName);  
          
          bFeedArray.push_back(BF);
          //  attachSystem::addToInsertForced(System,*GB, Target->getCC("Wheel"));
          
        }
    }
  return;
}

void
makeESS::buildBunkerChicane(Simulation& System,
                            const mainSystem::inputParam& IParam)
  /*!
    Build the chicane
    \param System :: Simulation
    \param IParam :: Input data
   */
{
  ELog::RegMethod RegA("makeESS","buildBunkerChicane");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const size_t NSet=IParam.setCnt("bunkerChicane");

  ELog::EM<<"Calling bunker Chicane"<<ELog::endDiag;
  for(size_t j=0;j<NSet;j++)
    {
      const std::string errMess="bunkerChicane "+StrFunc::makeString(j);
      const std::string bunkerName=
        IParam.getValueError<std::string>
        ("bunkerChicane",j,0,"BunkerName "+errMess);

      // bunkerA/etc should be a map
      std::shared_ptr<Bunker> BPtr;
      if (bunkerName=="BunkerA" || bunkerName=="ABunker")
        BPtr=ABunker;
      else if (bunkerName=="BunkerB" || bunkerName=="BBunker")
        BPtr=BBunker;
      else if (bunkerName=="BunkerC" || bunkerName=="CBunker")
        BPtr=CBunker;
      else if (bunkerName=="BunkerD" || bunkerName=="DBunker")
        BPtr=DBunker;
      else
        throw ColErr::InContainerError<std::string>
          (bunkerName,"bunkerName not know");

      std::shared_ptr<Chicane> CF
        (new Chicane("BunkerChicane"+StrFunc::makeString(j)));
      OR.addObject(CF);
      CF->addInsertCell(74123);

      // Positioned relative to segment:
      size_t segNumber(0);
      const std::string segObj=
        IParam.getValueError<std::string>
        ("bunkerChicane",j,1,"SegmentNumber "+errMess);
      
      if (!StrFunc::convert(segObj,segNumber))
	{ 
	  const std::string linkName=
	    IParam.getValueError<std::string>
	    ("bunkerChicane",j,2,"SegmentNumber "+errMess);
	  const attachSystem::FixedComp* FCPtr=
	    OR.getObjectThrow<attachSystem::FixedComp>(segObj,"Chicane Object");

	  const long int linkIndex=attachSystem::getLinkIndex(linkName);
          CF->createAll(System,*FCPtr,linkIndex);
	}
      else
	{
          CF->createAll(System,*BPtr,segNumber);
	}

      attachSystem::addToInsertLineCtrl(System,*BPtr,"MainVoid",*CF,*CF);
      attachSystem::addToInsertLineCtrl(System,*BPtr,"roof",*CF,*CF);
      attachSystem::addToInsertLineCtrl(System,*BPtr,"frontWall",*CF,*CF);
    }
  return;
}

void
makeESS::buildBunkerQuake(Simulation& System,
			  const mainSystem::inputParam& IParam)
  /*!
    Build the bunker earthquake dilitation join
    \param System :: Simulation
    \param IParam :: Input data
  */
{
  ELog::RegMethod RegA("makeESS","buildBunkerQuake");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const size_t NSet=IParam.setCnt("bunkerQuake");

  ELog::EM<<"Calling bunker Quake"<<ELog::endDiag;
  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("bunkerQuake",j);
      if (NItems>=1)
        {
          const std::string bunkerName=
            IParam.getValue<std::string>("bunkerQuake",j,0);

          // bunkerA/etc should be a map
          std::shared_ptr<Bunker> BPtr;
          if (bunkerName=="BunkerA" || bunkerName=="ABunker")
            BPtr=ABunker;
          else if (bunkerName=="BunkerB" || bunkerName=="BBunker")
            BPtr=BBunker;
          else if (bunkerName=="BunkerC" || bunkerName=="CBunker")
            BPtr=CBunker;
          else if (bunkerName=="BunkerD" || bunkerName=="DBunker")
            BPtr=DBunker;
          else
            throw ColErr::InContainerError<std::string>
              (bunkerName,"bunkerName not know");
          
          std::shared_ptr<BunkerQuake> BF(new BunkerQuake(BPtr->getKeyName()));
          OR.addObject(BF);
          BF->createAll(System,*BPtr,12,0);  
        }
    }

  return;
}
  
void
makeESS::buildPillars(Simulation& System,
		      const mainSystem::inputParam& IParam)
  /*!
    Build the pillars in the bunker
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("makeESS","buildPillars");

  const std::vector<std::string> BP=
    IParam.getAllItems("bunkerPillars");

  for(const std::string& Item : BP)
    {
      if (Item=="ABunker")
	ABunkerPillars->createAll(System,*ABunker);
      if (Item=="BBunker")
	BBunkerPillars->createAll(System,*BBunker);
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
  FuncDataBase& Control=System.getDataBase();

  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("beamlines",0);
      size_t index=1;
      while(index<NItems)  // min of two items
	{
	  const std::string BL=
	    IParam.getValue<std::string>("beamlines",j,index-1);
	  const std::string Btype=
	    IParam.getValue<std::string>("beamlines",j,index);
	  index+=2;

	  Control.addVariable(BL+"Active",1);
	  int fillFlag(0);
	  if (IParam.checkItem<int>("beamlines",j,index+1,fillFlag))
	    {
	      // setting is CONTROLED as value from variable taken
	      // otherwise
	      Control.addVariable(BL+"Filled",fillFlag);
	      index++;
	    }

	  if (IParam.checkItem<int>("beamlines",j,index+1,fillFlag))
	    {
	      // setting is CONTROLED as value from variable taken
	      // otherwise
	      Control.addVariable(BL+"Filled",fillFlag);
	      index++;
	    }
	  
	  // FIND BUNKER HERE:::
	  makeESSBL BLfactory(BL,Btype);
	  std::pair<int,int> BLNum=makeESSBL::getBeamNum(BL);
          ELog::EM<<"BLNum == "<<BLNum.first<<" "<<BLNum.second<<ELog::endDiag;
	  
	  if (BLNum.first==1 && BLNum.second<=11)
	    BLfactory.build(System,*ABunker);
	  else if (BLNum.first==1 && BLNum.second>11)
	    BLfactory.build(System,*BBunker);
	  else if (BLNum.first==2 && BLNum.second<=11)
	    BLfactory.build(System,*DBunker);
	  else if (BLNum.first==2 && BLNum.second>11)
	    BLfactory.build(System,*CBunker);
	}
    }

  return;
}

void
makeESS::makeBunker(Simulation& System,
                    const mainSystem::inputParam& IParam)
  /*!
    Make the bunker system
    \param System :: Simulation 
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("makeESS","makeBunker");
  const int voidCell(74123);
  const std::string bunkerType=
    IParam.getValue<std::string>("bunkerType");
  
  ABunker->addInsertCell(voidCell);
  ABunker->setRotationCentre(ShutterBayObj->getCentre());
  ABunker->createAll(System,*ShutterBayObj,4,false);

  BBunker->addInsertCell(voidCell);
  BBunker->setCutWall(0,1);
  BBunker->createAll(System,*ShutterBayObj,4,false);

  ABunker->insertComponent(System,"rightWall",*BBunker);
  ABunker->insertComponent(System,"roofFarEdge",*BBunker);
  ABunker->insertComponent(System,"floor",*BBunker);

  // Other side if needed :
  
  CBunker->addInsertCell(voidCell);
  CBunker->createAll(System,*ShutterBayObj,3,true);

  DBunker->addInsertCell(voidCell);
  DBunker->setCutWall(0,1);
  DBunker->createAll(System,*ShutterBayObj,3,true);

  CBunker->insertComponent(System,"rightWall",*DBunker);
  CBunker->insertComponent(System,"roofFarEdge",*DBunker);
  CBunker->insertComponent(System,"floor",*DBunker);

  if (bunkerType.find("noCurtain")==std::string::npos)
    {

      TopCurtain->addInsertCell("Top",voidCell);
      TopCurtain->addInsertCell("Lower",voidCell);
      TopCurtain->addInsertCell("Mid",voidCell);

      // THIS IS HORRIFFICALLY INEFFICENT :: FIX
      TopCurtain->addInsertCell("RoofCut",ABunker->getCells("roof"));
      TopCurtain->addInsertCell("RoofCut",BBunker->getCells("roof"));
      TopCurtain->createAll(System,*ShutterBayObj,6,4);

      ABHighBay->setCurtainCut
	(TopCurtain->combine({"-OuterRadius","-OuterZStep"}));
      ABHighBay->addInsertCell(voidCell);
      ABHighBay->createAll(System,*ABunker,*BBunker);

      //      CDHighBay->setCurtainCut
      //	(TopCurtain->combine({"-OuterRadius","-OuterZStep"}));
      CDHighBay->addInsertCell(voidCell);
      CDHighBay->createAll(System,*CBunker,*DBunker);
    }
  if (bunkerType.find("help")!=std::string::npos)
    {
      ELog::EM<<"bunkerType :: \n"
              <<"  noCurtain -- Disallow the curtain\n"
              <<"  noPillar -- Disallow the pillars\n"
              <<ELog::endBasic;
    }
        
  return;
}

  
void
makeESS::buildPreWings(Simulation& System)
  /*!
    Build pre wings :: These are little layers of pre-moderator that
    drop into the flight-line space 
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("makeESS","buildPreWings");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  enum Side {bottom, top};

  const ButterflyModerator* TMod=
    dynamic_cast<const ButterflyModerator*>(TopMod.get());
  if (TMod)
    {
      TopPreWingA = std::shared_ptr<PreModWing>
        (new PreModWing("TopLeftPreWing"));
      
      OR.addObject(TopPreWingA);
      TopPreWingA->setDivider(TMod->getMainRule(-7));
      TopPreWingA->setInnerExclude(TMod->getLeftExclude());
      TopPreWingA->setMidExclude(TMod->getLeftFarExclude());

      TopPreWingA->setBaseCut(TopPreMod->getSurfRules("Layer2"));
      TopPreWingA->setTopCut(TopCapMod->getFullRule(5));
      TopPreWingA->setOuter(TopPreMod->getSurfRule("-OuterRad"));
      TopPreWingA->addInsertCell(TMod->getCells("MainVoid"));
      TopPreWingA->createAll(System,*TMod,0);

      TopPreWingB =
        std::shared_ptr<PreModWing>(new PreModWing("TopRightPreWing"));
      
      OR.addObject(TopPreWingB);
      TopPreWingB->setDivider(TMod->getMainRule(7));
      TopPreWingB->setInnerExclude(TMod->getRightExclude());
      TopPreWingB->setMidExclude(TMod->getRightFarExclude());
      TopPreWingB->setBaseCut(TopPreMod->getFullRule(6));
      TopPreWingB->setTopCut(TopCapMod->getFullRule(5));
      TopPreWingB->setOuter(TopPreMod->getSurfRule("-OuterRad"));
      
      TopPreWingB->addInsertCell(TMod->getCells("MainVoid"));
      TopPreWingB->createAll(System,*TMod,0);
    }

  return;
}

void
makeESS::buildTwister(Simulation& System)
  /*!
    Adds a twister to the main system
    \param System :: Simulation 
   */
{
  ELog::RegMethod RegA("makeESS","buildTwister");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  Twister = std::shared_ptr<TwisterModule>(new TwisterModule("Twister"));
  OR.addObject(Twister);

  Twister->createAll(System,*Bulk,0);

  attachSystem::addToInsertForced(System,*Bulk,Twister->getCC("Shaft"));
  attachSystem::addToInsertForced(System,*Bulk,Twister->getCC("PlugFrame"));
  attachSystem::addToInsertForced(System,*Bulk,Twister->getCC("ShaftBearing"));
  
  attachSystem::addToInsertForced(System,*ShutterBayObj,Twister->getCC("Shaft"));
  attachSystem::addToInsertSurfCtrl(System,*Twister,PBeam->getCC("Sector0"));
  attachSystem::addToInsertSurfCtrl(System,*Twister, PBeam->getCC("Sector1"));
  attachSystem::addToInsertControl(System, *Twister, *Reflector);

  // split Twister by components
  // for (const ContainedComp & CC : Twister->getCC()) ...
  // use LineControl for intersections with flight lines
  
  ELog::EM<<"CALLING addInsertForce [INEFFICIENT] "<<ELog::endWarn;
  attachSystem::addToInsertForced(System,*Twister,TopAFL->getCC("outer"));
  attachSystem::addToInsertForced(System,*Twister,TopBFL->getCC("outer"));
  attachSystem::addToInsertForced(System,*Twister,LowAFL->getCC("outer"));
  attachSystem::addToInsertForced(System,*Twister,LowBFL->getCC("outer"));

  attachSystem::addToInsertForced(System,*Twister, Target->getCC("Wheel"));
  attachSystem::addToInsertForced(System,*Twister, Target->getCC("Wheel"));

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

  const FuncDataBase& Control=System.getDataBase();
  int voidCell(74123);

  const std::string lowPipeType=IParam.getValue<std::string>("lowPipe");
  const std::string topPipeType=IParam.getValue<std::string>("topPipe");

  const std::string lowModType=IParam.getValue<std::string>("lowMod");
  const std::string topModType=IParam.getValue<std::string>("topMod");

  
  const std::string targetType=IParam.getValue<std::string>("targetType");
  const std::string iradLine=IParam.getValue<std::string>("iradLineType");

  const size_t nF5=IParam.getValue<size_t>("nF5");
  const int engActive=IParam.flag("eng");
  
  if (StrFunc::checkKey("help",lowPipeType,lowModType,targetType) ||
      StrFunc::checkKey("help",iradLine,topModType,""))
    {
      optionSummary(System);
      throw ColErr::ExitAbort("Help system exit");
    }
  
  buildFocusPoints(System);
  makeTarget(System,targetType);
  Reflector->globalPopulate(Control);

  // lower moderator
  if (lowModType != "None")
    LowPreMod->createAll(System,World::masterOrigin(),0,true,
			 Target->wheelHeight()/2.0,
			 Reflector->getRadius());

  TopPreMod->createAll(System,World::masterOrigin(),0,false,
		       Target->wheelHeight()/2.0,
		       Reflector->getRadius());


  if (lowModType == "Butterfly")
    buildLowButterfly(System);
  else if (lowModType == "Pancake")
    buildLowPancake(System);
  else if (lowModType == "Box")
    buildLowBox(System);
  else if (lowModType != "None")
    throw ColErr::InContainerError<std::string>(lowModType,"Low Mod Type");

  if (topModType == "Butterfly")
    buildTopButterfly(System);
  else if (topModType == "Pancake")
    buildTopPancake(System);
  else if (topModType == "Box")
    buildTopBox(System);
  else 
    throw ColErr::InContainerError<std::string>(topModType,"Top Mod Type");

  const double LMHeight=(lowModType == "None")
    ? 0.0 : LowMod->getLinkDistance(5,6);
  const double TMHeight=TopMod->getLinkDistance(5,6);

  
  // Cap moderator DOES not span whole unit
  TopCapMod->createAll(System,*TopMod,6,false,
   		       0.0,Reflector->getRadius());

  if (lowModType != "None")
    LowCapMod->createAll(System,*LowMod,6,false,
			 0.0,Reflector->getRadius());

  buildPreWings(System);

  const double LMAssembly=
    LowPreMod->getHeight()+LMHeight+LowCapMod->getHeight();
  const double TMAssembly=
    TopPreMod->getHeight()+TMHeight+TopCapMod->getHeight();

  Reflector->createAll(System,World::masterOrigin(),0,
		       Target->wheelHeight(),LMAssembly,TMAssembly);
  
  Reflector->insertComponent(System,"targetVoid",*Target,1);
  Bulk->createAll(System,*Reflector,*Reflector);

  // Build flightlines after bulk
  Reflector->deleteCell(System,"topVoid");
  TopAFL->createAll(System,*TopMod,0,*Reflector,4,*Bulk,-3);
  TopBFL->createAll(System,*TopMod,0,*Reflector,3,*Bulk,-3);

  if (lowModType != "None")
    {
      Reflector->deleteCell(System,"lowVoid");
      LowAFL->createAll(System,*LowMod,0,*Reflector,4,*Bulk,-3);
      LowBFL->createAll(System,*LowMod,0,*Reflector,3,*Bulk,-3);
    }

  
  // THESE calls correct the MAIN volume so pipe work MUST be after here:
  attachSystem::addToInsertSurfCtrl(System,*Bulk,Target->getCC("Wheel"));
  attachSystem::addToInsertForced(System,*Bulk,Target->getCC("Shaft"));
  if (lowModType != "None")
    {
      attachSystem::addToInsertForced(System,*Bulk,LowAFL->getCC("outer"));
      attachSystem::addToInsertForced(System,*Bulk,LowBFL->getCC("outer"));
    }
  attachSystem::addToInsertForced(System,*Bulk,TopAFL->getCC("outer"));
  attachSystem::addToInsertForced(System,*Bulk,TopBFL->getCC("outer"));

  buildIradComponent(System,IParam);
  // Full surround object
  ShutterBayObj->addInsertCell(voidCell);
  ShutterBayObj->createAll(System,*Bulk,*Bulk);
  attachSystem::addToInsertForced(System,*ShutterBayObj,
				  Target->getCC("Wheel"));
  attachSystem::addToInsertForced(System,*ShutterBayObj,
				  Target->getCC("Shaft"));


  createGuides(System);
  makeBunker(System,IParam);

  // THIS CANNOT BE RIGHT--- VERY INEFFICIENT
  /*
  TSMainBuildingObj->addInsertCell(74123);
  TSMainBuildingObj->createAll(System,World::masterOrigin(),0);
  attachSystem::addToInsertLineCtrl(System, *TSMainBuildingObj, *ShutterBayObj);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *ABunker);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *BBunker);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *CBunker);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *DBunker);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, TopCurtain->getCC("Top"));
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, TopCurtain->getCC("Mid"));
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, TopCurtain->getCC("Lower"));
  attachSystem::addToInsertForced(System, *TSMainBuildingObj,   Target->getCC("Shaft"));

  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *ABHighBay);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *CDHighBay);
  */
  
  // PROTON BEAMLINE

  //  pbip->createAll(System,World::masterOrigin(),0,*Bulk,3,*Target,1);
  //  attachSystem::addToInsertSurfCtrl(System,*Bulk,pbip->getCC("before"));
  //  attachSystem::addToInsertSurfCtrl(System,*Bulk,pbip->getCC("main"));
  //  Reflector->insertComponent(System, "targetVoid", pbip->getCC("after"));

  PBeam->setFront(*Reflector,1);
  PBeam->setBack(*ShutterBayObj,-1);
  PBeam->createAll(System,*Reflector,1);  
  attachSystem::addToInsertSurfCtrl(System,*ShutterBayObj,PBeam->getCC("Full"));
  attachSystem::addToInsertSurfCtrl(System,*Bulk,PBeam->getCC("Full"));

  if (engActive)
    buildTwister(System);
  else
    {
      // if no -eng flag then Twister is not built -> must insert into Bulk
      //   attachSystem::addToInsertSurfCtrl(System,*Bulk,pbip->getCC("after"));
    }

  // WARNING: THESE CALL MUST GO AFTER the main void (74123) has
  // been completed. Otherwize we can't find the pipe in the volume.

  
  ModPipes->buildTopPipes(System,topPipeType);
  buildPillars(System,IParam);
  if (IParam.flag("bunkerFeed"))
    buildBunkerFeedThrough(System,IParam);
  if (IParam.flag("bunkerQuake"))
    buildBunkerQuake(System,IParam);
  if (IParam.flag("bunkerChicane"))
    buildBunkerChicane(System,IParam);

  if (lowModType != "None")
    ModPipes->buildLowPipes(System,lowPipeType);

  makeBeamLine(System,IParam);

  buildF5Collimator(System, nF5);
  ELog::EM<<"=Finished beamlines="<<ELog::endDiag;
  return;
}


}   // NAMESPACE essSystem

