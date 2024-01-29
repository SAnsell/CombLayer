/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/makeESS.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell/Konstantin Batkov
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
 * along with this program.  If not, see Low<http://www.gnu.org/licenses/>. 
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
#include "OutputLog.h"
#include "Vec3D.h"
#include "support.h"
#include "inputParam.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "LayerComp.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "BasicFlightLine.h"
#include "WedgeFlightLine.h"
#include "AttachSupport.h"
#include "Importance.h"
#include "Object.h"

#include "FocusPoints.h"
#include "beamlineConstructor.h"
#include "WheelBase.h"
#include "Wheel.h"
#include "BilbaoWheel.h"
#include "BeRef.h"
#include "ProtonTube.h"
#include "EssModBase.h"
#include "H2Wing.h"
#include "ButterflyModerator.h"
#include "PancakeModerator.h"
#include "BoxModerator.h"
#include "PreModWing.h"
#include "IradCylinder.h"
#include "BulkModule.h"
#include "TwisterModule.h"
#include "ShutterBay.h"
#include "GuideBay.h"
#include "GuideItem.h"
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
#include "EmptyCyl.h"
#include "makeESS.h"

namespace essSystem
{

makeESS::makeESS() :
  Reflector(new BeRef("BeRef")),
  PBeam(new ProtonTube("ProtonTube")),
  pbip(new PBIP("PBIP")),

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
}


makeESS::~makeESS()
  /*!
    Destructor
  */
{}


void
makeESS::setEngineeringFlag(const mainSystem::inputParam& IParam)
  /*!
    Creates a simple set based on IParam values
    Currently not checking but that should be added
    \param IParam :: Input stream to process
  */
{
  ELog::RegMethod RegA("makeESS","setEngineeringFlag");
  if (IParam.flag("engineering"))
    {
      const std::vector<std::string> items=
	IParam.getAllItems("engineering");
      
      for(const std::string& IV : items)
	engFlags.insert(IV);

      if (items.empty())
	engFlags.insert("All");
    }
  return;  
}

bool
makeESS::hasEngineering(const std::string& Item) const
  /*!
    Test to set if a flag is active 
    \param All is a default for everything
    \return true if flag exists
  */
{
  return (engFlags.find("All")!=engFlags.end() ||
	  engFlags.find(Item)!=engFlags.end());
}
  

  
void
makeESS::makeTarget(Simulation& System,
		    const mainSystem::inputParam& IParam)
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
  const std::string targetType=
    IParam.getValue<std::string>("targetType");

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
  if (IParam.flag("engActive"))
    Target->setEngActive();
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
      GB->setCutSurf("innerCyl",*Bulk,"OuterCyl");
      GB->setCutSurf("outerCyl",*ShutterBayObj,"InnerCyl");
      GB->setCutSurf("wheel",Target->getCC("Wheel").getOuterSurf());

      GB->createAll(System,*ShutterBayObj,0);
      GB->insertComponent(System,"Inner",Target->getCC("Wheel"));      
      GBArray.push_back(GB);
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
    within a moderator (typically). Currently not saved in 
    the class set.
    \param System :: Simulation
    \param IParam :: Name of Irad component + location
   */
{
  ELog::RegMethod RegA("makeESS","buildIradComponent");

  const size_t NSet=IParam.setCnt("iradObject");
  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("iradObject",j);
      // is this possible?
      if (NItems<3)
        throw ColErr::SizeError<size_t>
          (NItems,3,"IradComp["+std::to_string(j)+"]");

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
            System.getObject<attachSystem::FixedComp>(compName);
          if (!FC)
            throw ColErr::InContainerError<std::string>
              (compName,"Component not found");
          const long int linkPt=attachSystem::getLinkNumber(linkName);
          
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


  std::shared_ptr<ButterflyModerator> BM
    (new essSystem::ButterflyModerator("LowFly"));
  
  BM->setRadiusX(Reflector->getRadius());
  
  LowMod=std::shared_ptr<EssModBase>(BM);
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

  std::shared_ptr<ButterflyModerator> BM
    (new essSystem::ButterflyModerator("TopFly"));
  BM->setRadiusX(Reflector->getRadius());
  TopMod=std::shared_ptr<EssModBase>(BM);
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


  std::shared_ptr<PancakeModerator> BM
    (new essSystem::PancakeModerator("LowCake"));
  BM->setRadiusX(Reflector->getRadius());

  LowMod=std::shared_ptr<EssModBase>(BM);
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


  std::shared_ptr<PancakeModerator> BM
    (new essSystem::PancakeModerator("TopCake"));
  BM->setRadiusX(Reflector->getRadius());
  TopMod=std::shared_ptr<EssModBase>(BM);
  
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


  std::shared_ptr<BoxModerator> BM
    (new essSystem::BoxModerator("LowBox"));
  BM->setRadiusX(Reflector->getRadius());
  LowMod=std::shared_ptr<EssModBase>(BM);
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

  std::shared_ptr<BoxModerator> BM
    (new essSystem::BoxModerator("TopBox"));
  BM->setRadiusX(Reflector->getRadius());
  TopMod=std::shared_ptr<EssModBase>(BM);
  
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

  for (size_t i=0; i<nF5; i++)
    {
      std::shared_ptr<F5Collimator> F5(new F5Collimator("F"+std::to_string(i*10+5)));
      F5->addInsertCell(74123);       // !!! 74123=voidCell // SA: how to exclude F5 from any cells?
      F5->createAll(System,World::masterOrigin(),0);
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
          BF->buildAll(System,*BPtr,segNumber,feedName);  
          
          bFeedArray.push_back(BF);
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


  const size_t NSet=IParam.setCnt("bunkerChicane");

  ELog::EM<<"Calling bunker Chicane"<<ELog::endDiag;
  for(size_t j=0;j<NSet;j++)
    {
      const std::string errMess="bunkerChicane "+std::to_string(j);
      const std::string bunkerName=
        IParam.getValueError<std::string>
        ("bunkerChicane",j,0,"BunkerName "+errMess);
      ELog::EM<<"Bunker Name == "<<bunkerName<<ELog::endDiag;
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
        (new Chicane("BunkerChicane"+std::to_string(j)));
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
	    System.getObjectThrow<attachSystem::FixedComp>(segObj,"Chicane Object");

	  const long int linkIndex=FCPtr->getSideIndex(linkName);
          CF->createAll(System,*FCPtr,linkIndex);
	}
      else
	{
          CF->buildAll(System,*BPtr,segNumber);
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
          BF->buildAll(System,*BPtr,12,0);  
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
	ABunkerPillars->build(System,*ABunker);
      if (Item=="BBunker")
	BBunkerPillars->build(System,*BBunker);
    }
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
      const size_t NItems=IParam.itemCnt("beamlines",j);
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
	  
	  if (BLNum.first==1 && BLNum.second<=10)
	    BLfactory.build(System,*ABunker);
	  else if (BLNum.first==1 && BLNum.second>10)
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
  ABunker->createAll(System,*ShutterBayObj,4);

  BBunker->addInsertCell(voidCell);
  BBunker->setCutWall(0,1);
  BBunker->createAll(System,*ShutterBayObj,4);

  ABunker->insertComponent(System,"rightWall",*BBunker);
  ABunker->insertComponent(System,"roofFarEdge",*BBunker);
  ABunker->insertComponent(System,"floor",*BBunker);

  // Other side if needed :
  
  CBunker->addInsertCell(voidCell);
  CBunker->setReversed();
  CBunker->createAll(System,*ShutterBayObj,3);

  DBunker->addInsertCell(voidCell);
  DBunker->setCutWall(0,1);
  DBunker->setReversed();
  DBunker->createAll(System,*ShutterBayObj,3);

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
      TopCurtain->buildAll(System,*ShutterBayObj,6,4);
      /*
      ABHighBay->setCurtainCut
	(TopCurtain->combine("-OuterRadius -OuterZStep"));
      ABHighBay->addInsertCell(voidCell);
      ABHighBay->buildAll(System,*ABunker,*BBunker);

      //      CDHighBay->setCurtainCut
      //	(TopCurtain->combine({"-OuterRadius","-OuterZStep"}));
      CDHighBay->addInsertCell(voidCell);
      CDHighBay->buildAll(System,*CBunker,*DBunker);
      */
      
      // minimize cells as curtain will be in MANY unnecessary roof objects:
      // and we will need to use them later:

      ABHighBay->setCutSurf("frontCut",*ABunker,3);
      ABHighBay->setCutSurf("curtainCut",
			    TopCurtain->combine("-OuterRadius -OuterZStep"));

      ABHighBay->setCutSurf
	("leftWallInner",ABunker->getSurfRules("leftWallInner"));
      ABHighBay->setCutSurf
	("leftWallOuter",ABunker->getSurfRules("leftWallOuter"));
      ABHighBay->setCutSurf
	("rightWallInner",BBunker->getSurfRules("-rightWallInner"));
      ABHighBay->setCutSurf
	("rightWallOuter",BBunker->getSurfRules("-rightWallOuter"));
      ABHighBay->setCutSurf
	("roofOuter",ABunker->getSurfRules("roofOuter"));
      
      ABHighBay->addInsertCell(voidCell);
      ABHighBay->createAll(System,*ABunker,0);

      CDHighBay->setCutSurf("frontCut",*CBunker,3);
      //      CDHighBay->setCurtainCut
      //	(TopCurtain->combine({"-OuterRadius","-OuterZStep"}));
      CDHighBay->setCutSurf
	("leftWallInner",CBunker->getSurfRules("leftWallInner"));
      CDHighBay->setCutSurf
	("leftWallOuter",CBunker->getSurfRules("leftWallOuter"));
      CDHighBay->setCutSurf
	("rightWallInner",DBunker->getSurfRules("-rightWallInner"));
      CDHighBay->setCutSurf
	("rightWallOuter",DBunker->getSurfRules("-rightWallOuter"));
      CDHighBay->setCutSurf
	("roofOuter",CBunker->getSurfRules("roofOuter"));
      
      CDHighBay->addInsertCell(voidCell);
      CDHighBay->createAll(System,*CBunker,0);

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
  //  enum Side {bottom, top};

  const ButterflyModerator* TMod=
    dynamic_cast<const ButterflyModerator*>(TopMod.get());
  if (TMod)
    {
      TopPreWingA = std::shared_ptr<PreModWing>
        (new PreModWing("TopLeftPreWing"));
      
      OR.addObject(TopPreWingA);
      TopPreWingA->setCutSurf("Divider",TMod->getMainRule(-7));
      TopPreWingA->setCutSurf("Inner",TMod->getLeftExclude());
      TopPreWingA->setCutSurf("Mid",TMod->getLeftFarExclude());

      TopPreWingA->setCutSurf("Base",TopPreMod->getSurfRules("Layer2"));
      TopPreWingA->setCutSurf("Top",*TopCapMod,5);
      TopPreWingA->setCutSurf("Outer",TopPreMod->getSurfRule("-OuterRad"));

      TopPreWingA->addInsertCell(TMod->getCells("MainVoid"));
      TopPreWingA->createAll(System,*TMod,0);

      TopPreWingB =
        std::shared_ptr<PreModWing>(new PreModWing("TopRightPreWing"));
      
      OR.addObject(TopPreWingB);
      TopPreWingB->setCutSurf("Divider",TMod->getMainRule(7));
      TopPreWingB->setCutSurf("Inner",TMod->getRightExclude());
      TopPreWingB->setCutSurf("Mid",TMod->getRightFarExclude());

      TopPreWingB->setCutSurf("Base",*TopPreMod,6);
      TopPreWingB->setCutSurf("Top",*TopCapMod,5);
      TopPreWingB->setCutSurf("Outer",TopPreMod->getSurfRule("-OuterRad"));
      
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

  
  const std::string iradLine=IParam.getValue<std::string>("iradLineType");

  //  const size_t nF5=IParam.getValue<size_t>("nF5");
  
  buildFocusPoints(System);
  makeTarget(System,IParam);
  Reflector->globalPopulate(Control);

  // lower moderator
  if (lowModType != "None")
    {
      LowPreMod->setLayout(true,Target->wheelHeight()/2.0,
			   Reflector->getRadius());
      LowPreMod->createAll(System,World::masterOrigin(),0);
    }

  TopPreMod->setLayout(false,Target->wheelHeight()/2.0,
		       Reflector->getRadius());
  TopPreMod->createAll(System,World::masterOrigin(),0);

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
  TopCapMod->setLayout(false,0.0,Reflector->getRadius());
  TopCapMod->createAll(System,*TopMod,6);

  if (lowModType != "None")
    {
      LowCapMod->setLayout(false,0.0,Reflector->getRadius());
      LowCapMod->createAll(System,*LowMod,6);
    }

  buildPreWings(System);

  const double LMAssembly=
    LowPreMod->getHeight()+LMHeight+LowCapMod->getHeight();
  const double TMAssembly=
    TopPreMod->getHeight()+TMHeight+TopCapMod->getHeight();

  Reflector->setVoidThick( Target->wheelHeight(),LMAssembly,TMAssembly);
  Reflector->createAll(System,World::masterOrigin(),0);
  
  Reflector->insertComponent(System,"targetVoid",*Target,1);
  Bulk->setCutSurf("Reflector",Reflector->getExclude());
  Bulk->setCutSurf("TargetBase",*Target,"VoidBase");
  Bulk->setCutSurf("TargetTop",*Target,"VoidTop");
  Bulk->setCutSurf("ReflectorRadius",*Reflector,"OuterRadius");
  Bulk->setCutSurf("TargetRadius",*Target,"front");
  Bulk->createAll(System,*Reflector,0);

  // Build flightlines after bulk
  Reflector->deleteCell(System,"topVoid");
  TopAFL->setCutSurf("Inner",*Reflector,4);
  TopAFL->setCutSurf("Outer",*Bulk,-3);
  TopBFL->setCutSurf("Inner",*Reflector,3);
  TopBFL->setCutSurf("Outer",*Bulk,-3);
  TopAFL->createAll(System,*TopMod,0);
  TopBFL->createAll(System,*TopMod,0);

  if (lowModType != "None")
    {
      Reflector->deleteCell(System,"lowVoid");
      LowAFL->setCutSurf("Inner",*Reflector,4);
      LowAFL->setCutSurf("Outer",*Bulk,-3);
      LowBFL->setCutSurf("Inner",*Reflector,3);
      LowBFL->setCutSurf("Outer",*Bulk,-3);
      LowAFL->createAll(System,*LowMod,0);
      LowBFL->createAll(System,*LowMod,0);
    }
  
  // THESE calls correct the MAIN volume so pipe work MUST be after here:
  // ALL OF THES addto insert surf calls are AWFUL!!!!
  // ----

  // two part (top/base)
  // this can have further specialization

  Bulk->insertComponent(System,"Layer1Base",Target->getCC("Wheel"));
  Bulk->insertComponent(System,"Layer1Top",Target->getCC("Wheel"));
  Bulk->insertComponent(System,"Layer2Base",Target->getCC("Wheel"));
  Bulk->insertComponent(System,"Layer2Top",Target->getCC("Wheel"));
  Bulk->insertComponent(System,"Layer2Base",Target->getCC("Shaft"));
  Bulk->insertComponent(System,"Layer2Top",Target->getCC("Shaft"));

  if (lowModType != "None")
    {
      Bulk->insertComponent(System,"Layer0",0,LowAFL->getCC("outer"));
      Bulk->insertComponent(System,"Layer0",0,LowBFL->getCC("outer"));
      Bulk->insertComponent(System,"Layer1Base",LowAFL->getCC("outer"));
      Bulk->insertComponent(System,"Layer1Base",LowBFL->getCC("outer"));
      Bulk->insertComponent(System,"Layer2Base",LowAFL->getCC("outer"));
      Bulk->insertComponent(System,"Layer2Base",LowBFL->getCC("outer"));
    }

  Bulk->insertComponent(System,"Layer0",1,TopAFL->getCC("outer"));
  Bulk->insertComponent(System,"Layer0",1,TopBFL->getCC("outer"));
  Bulk->insertComponent(System,"Layer1Top",TopAFL->getCC("outer"));
  Bulk->insertComponent(System,"Layer1Top",TopBFL->getCC("outer"));
  Bulk->insertComponent(System,"Layer2Top",TopAFL->getCC("outer"));
  Bulk->insertComponent(System,"Layer2Top",TopBFL->getCC("outer"));
  //  attachSystem::addToInsertForced(System,*Bulk,
  //				  TopAFL->getCC("outer"));
  //  attachSystem::addToInsertForced(System,*Bulk,TopBFL->getCC("outer"));

  buildIradComponent(System,IParam);
  // Full surround object
  ShutterBayObj->addInsertCell(voidCell);
  ShutterBayObj->setCutSurf("Bulk",Bulk->getExclude());
  ShutterBayObj->createAll(System,*Bulk,0);

  ELog::EM<<"Target CC == "<<Target->getCC("Wheel")<<ELog::endDiag;
  
  attachSystem::addToInsertForced(System,*ShutterBayObj,
				  Target->getCC("Wheel"));
  attachSystem::addToInsertForced(System,*ShutterBayObj,
				  Target->getCC("Shaft"));


  createGuides(System);
  makeBunker(System,IParam);

  // THIS CANNOT BE RIGHT--- VERY INEFFICIENT

  TSMainBuildingObj->addInsertCell(voidCell);
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
  // ---------------------------------------------------------------------------------------

  
  // PROTON BEAMLINE

  pbip->setCutSurf("front",*Bulk,3);
  pbip->setCutSurf("back",*Target,1);
  //  pbip->createAll(System,World::masterOrigin(),0);
  //  attachSystem::addToInsertSurfCtrl(System,*Bulk,pbip->getCC("before"));
  //  attachSystem::addToInsertSurfCtrl(System,*Bulk,pbip->getCC("main"));
  //  Reflector->insertComponent(System, "targetVoid", pbip->getCC("after"));

  PBeam->setFront(*Bulk,4);
  PBeam->setBack(*TSMainBuildingObj,-1);
  PBeam->setCutSurf("shieldPlate",*ShutterBayObj,-6);
  PBeam->createAll(System,*Bulk,4);

  attachSystem::addToInsertSurfCtrl(System,*ShutterBayObj,
				    PBeam->getCC("Full"));
  attachSystem::addToInsertSurfCtrl(System,*TSMainBuildingObj,
				    PBeam->getCC("Sector3"));

  // PBeam->setFront(*Reflector,1);
  // PBeam->setBack(*ShutterBayObj,-1);
  // PBeam->createAll(System,*Reflector,1);  
  // attachSystem::addToInsertSurfCtrl(System,*ShutterBayObj,PBeam->getCC("Full"));
  // attachSystem::addToInsertSurfCtrl(System,*Bulk,PBeam->getCC("Full"));

  if (Twister)
    attachSystem::addToInsertSurfCtrl(System,*Twister,pbip->getCC("after"));

  makeBeamLine(System,IParam);
  //  buildF5Collimator(System, IParam);

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


  ELog::EM<<"=Finished beamlines="<<ELog::endDiag;
  return;
}


}   // NAMESPACE essSystem

