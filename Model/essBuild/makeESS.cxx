/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/makeESS.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "TaperedFlightLine.h"
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
#include "PreModWing.h"
#include "IradCylinder.h"
#include "SupplyPipe.h"
#include "BulkModule.h"
#include "TwisterModule.h"
#include "ShutterBay.h"
#include "GuideBay.h"
#include "TaperedDiskPreMod.h"
#include "Bunker.h"
#include "RoofPillars.h"
#include "BunkerFeed.h"
#include "Curtain.h"
#include "ConicModerator.h"
#include "makeESSBL.h"
#include "ESSPipes.h"
// F5 collimators:
#include "F5Calc.h"
#include "F5Collimator.h"

#include "makeESS.h"

namespace essSystem
{

makeESS::makeESS() :
  Reflector(new BeRef("BeRef")),
  PBeam(new ProtonTube("ProtonTube")),
  BMon(new BeamMonitor("BeamMonitor")),

  LowPreMod(new TaperedDiskPreMod("LowPreMod")),
  LowCapMod(new TaperedDiskPreMod("LowCapMod")),
  
  LowAFL(new moderatorSystem::TaperedFlightLine("LowAFlight")),
  LowBFL(new moderatorSystem::TaperedFlightLine("LowBFlight")),

  TopPreMod(new TaperedDiskPreMod("TopPreMod")),
  TopCapMod(new TaperedDiskPreMod("TopCapMod")),

  TopAFL(new moderatorSystem::TaperedFlightLine("TopAFlight")),
  TopBFL(new moderatorSystem::TaperedFlightLine("TopBFlight")),
  ModPipes(new ESSPipes()),

  Bulk(new BulkModule("Bulk")),
  ShutterBayObj(new ShutterBay("ShutterBay")),


  ABunker(new Bunker("ABunker")),
  BBunker(new Bunker("BBunker")),
  CBunker(new Bunker("CBunker")),
  DBunker(new Bunker("DBunker")),
  ABunkerPillars(new RoofPillars("ABunkerPillars")),
  BBunkerPillars(new RoofPillars("BBunkerPillars")),
  TopCurtain(new Curtain("Curtain"))
  
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
  Target->createAll(System,World::masterOrigin(),0);

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
	GB->createAll(System,*LowMod,0);  
      else
	GB->createAll(System,*TopMod,0);
      attachSystem::addToInsertForced(System,*GB,Target->getCC("Wheel"));      
      GBArray.push_back(GB);
      attachSystem::addToInsertForced(System,*GB, Target->getCC("Wheel"));
    }
  GBArray[1]->outerMerge(System,*GBArray[2]);
  GBArray[0]->outerMerge(System,*GBArray[3]);
  
  GBArray[0]->createGuideItems(System,LowMod->getComponent("MidWater"),6,5);
  GBArray[1]->createGuideItems(System,LowMod->getComponent("MidWater"),7,8);
  GBArray[2]->createGuideItems(System,TopMod->getComponent("MidWater"),6,5);
  GBArray[3]->createGuideItems(System,TopMod->getComponent("MidWater"),7,8);

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
  TopMod=std::shared_ptr<constructSystem::ModBase>(BM);
  OR.addObject(TopMod);
  
  TopMod->createAll(System,*Reflector,TopPreMod.get(),6);
  return;
}
      

void makeESS::buildF5Collimator(Simulation& System, size_t nF5)
/*!
  Build F5 collimators
  \param System :: Stardard simulation
 */
{
  ELog::RegMethod RegA("makeESS", "buildF5Collimator(System, nF5)");
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();

  for (size_t i=0; i<nF5; i++)
    {
      std::shared_ptr<F5Collimator> F5(new F5Collimator(StrFunc::makeString("F", i*10+5).c_str()));
      
      OR.addObject(F5);
      F5->addInsertCell(74123); // !!! 74123=voidCell // SA: how to exclude F5 from any cells?
      F5->createAll(System,World::masterOrigin());
      
      attachSystem::addToInsertSurfCtrl(System,*ABunker,*F5);
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
  ELog::RegMethod RegA("makeESS", "buildF5Collimator(System, IParam)");
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();

  std::string strtmp, moderator, range;
  const size_t nitems = IParam.itemCnt("f5-collimators",0); // number of parameters in -f5-collimator

  if (!nitems) return;

  std::string midWaterName, lobeName;
  double theta(0.0);
  size_t colIndex(0);
  //  ELog::EM << "Use StrFunc::convert instead of atoi in the loop below. Check its return value." << ELog::endCrit;
  for (size_t i=0; i<nitems; i++)
    {
      strtmp = IParam.getValue<std::string>("f5-collimators", i);
      if ( (strtmp=="TopFly") || (strtmp=="LowFly") )
	{
	  moderator = strtmp;
	  range = IParam.getValue<std::string>("f5-collimators", ++i); // thermal or cold
	  for (size_t j=i+1; j<nitems; j++)
	    {
	      strtmp = IParam.getValue<std::string>("f5-collimators", j);
	      if ((strtmp=="TopFly") || (strtmp=="LowFly"))
		break;
	      // do real work here
	      theta = atoi(strtmp.c_str()); // !!! use StrFunc::convert here !!! 

	      std::shared_ptr<F5Collimator> F5(new F5Collimator(StrFunc::makeString("F", colIndex*10+5).c_str())); colIndex++;
	      OR.addObject(F5);
	      F5->setTheta(theta);
	      F5->setRange(range); // thermal or cold

	      /*	      if (moderator=="TopFly")
		F5->setFocalPoints(TopFocalPoints);
	      else if (moderator=="LowFly")
	      F5->setFocalPoints(LowFocalPoints);*/

	      // get focal points
	      midWaterName = moderator + "MidWater";
	      const attachSystem::FixedComp* midWater = OR.getObject<attachSystem::FixedComp>(midWaterName);
	      if (!midWater)
		throw ColErr::InContainerError<std::string>
		  (midWaterName,"Component not found");

	      lobeName = moderator + "LeftLobe";
	      const attachSystem::FixedComp* lobe = OR.getObject<attachSystem::FixedComp>(lobeName);
	      if (!lobe)
		throw ColErr::InContainerError<std::string>
		  (lobeName,"Component not found");

	      std::vector<Geometry::Vec3D> vecFP;
	      for (size_t ii=0; ii<8; ii++)
		vecFP.push_back(midWater->getLinkPt(ii));
	      vecFP.push_back(lobe->getLinkPt(12)); // zmin
	      vecFP.push_back(lobe->getLinkPt(13)); // zmax
	      F5->setFocalPoints(vecFP);
	      /////
	      
	      F5->addInsertCell(74123); // !!! 74123=voidCell // SA: how to exclude F5 from any cells?
	      F5->createAll(System, World::masterOrigin());

	      ELog::EM << "Implement special care when theta is close to normals" << ELog::endDiag;
	      if (theta<90)
		attachSystem::addToInsertSurfCtrl(System, *ABunker, *F5);
	      else if (theta<180)
		attachSystem::addToInsertSurfCtrl(System, *BBunker, *F5);
	      else if (theta<270)
		attachSystem::addToInsertSurfCtrl(System, *DBunker, *F5);
	      else if (theta<360)
		attachSystem::addToInsertSurfCtrl(System, *CBunker, *F5);
	      F5array.push_back(F5);
	    }
	}
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
makeESS::buildPillars(Simulation& System)
  /*!
    Build the pillars in the bunker
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("makeESS","buildPillars");
  ABunkerPillars->createAll(System,*ABunker);
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
	  
	  // FIND BUNKER HERE:::
	  makeESSBL BLfactory(BL,Btype);
	  std::pair<int,int> BLNum=makeESSBL::getBeamNum(BL);
          ELog::EM<<"BLNum == "<<BLNum.first<<" "<<BLNum.second<<ELog::endDiag;

	  if ((BLNum.first==1 && BLNum.second>10) ||
	      (BLNum.first==4 && BLNum.second<=10) )
	    BLfactory.build(System,*ABunker);

	  else if ((BLNum.first==1 && BLNum.second<=10) ||
	      (BLNum.first==4 && BLNum.second>9) )
	    BLfactory.build(System,*BBunker);

	  else if ((BLNum.first==2 && BLNum.second>10) ||
	      (BLNum.first==3 && BLNum.second<=10) )
	    BLfactory.build(System,*DBunker);

	  else if ((BLNum.first==2 && BLNum.second<=10) ||
	      (BLNum.first==3 && BLNum.second>10) )
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
    \param bunkerType :: different bunker to make
  */
{
  ELog::RegMethod RegA("makeESS","makeBunker");
  const int voidCell(74123);
  const std::string bunkerType=
    IParam.getValue<std::string>("bunkerType");
  
  ABunker->addInsertCell(voidCell);
  ABunker->createAll(System,*LowMod,*GBArray[0],2,true,true);

  BBunker->addInsertCell(voidCell);
  BBunker->setCutWall(0,1);
  BBunker->createAll(System,*LowMod,*GBArray[0],2,true,true);

  ABunker->insertComponent(System,"rightWall",*BBunker);
  ABunker->insertComponent(System,"roofFarEdge",*BBunker);
  ABunker->insertComponent(System,"floor",*BBunker);

  // Other side if needed :
  
  CBunker->addInsertCell(voidCell);
  CBunker->createAll(System,*LowMod,*GBArray[1],2,false,true);

  DBunker->addInsertCell(voidCell);
  DBunker->setCutWall(0,1);
  DBunker->createAll(System,*LowMod,*GBArray[1],2,false,true);

  CBunker->insertComponent(System,"rightWall",*DBunker);
  CBunker->insertComponent(System,"roofFarEdge",*DBunker);
  CBunker->insertComponent(System,"floor",*DBunker);

  if (bunkerType.find("noPillar")==std::string::npos)
    buildPillars(System);

  if (IParam.flag("bunkerFeed"))
    buildBunkerFeedThrough(System,IParam);

  if (bunkerType.find("noCurtain")==std::string::npos)
    {
      TopCurtain->addInsertCell("Top",74123);
      TopCurtain->addInsertCell("Lower",74123);
      TopCurtain->addInsertCell("Mid",74123);
      TopCurtain->addInsertCell("Lower",ABunker->getCells("roof"));
      TopCurtain->addInsertCell("Lower",BBunker->getCells("roof"));
      TopCurtain->addInsertCell("Top",ABunker->getCells("roof"));
      TopCurtain->addInsertCell("Top",BBunker->getCells("roof"));
      
      TopCurtain->addInsertCell("Lower",ABunker->getCells("MainVoid"));
      TopCurtain->addInsertCell("Lower",BBunker->getCells("MainVoid"));
      //  TopCurtain->createAll(System,*GBArray[0],2,1,true);
      TopCurtain->createAll(System,*ShutterBayObj,6,4,*GBArray[0],2,true);
      
      //  TopCurtain->insertComponent(System,"topVoid",*ABunker);
      //  TopCurtain->insertComponent(System,"topVoid",*BBunker);
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
makeESS::buildPreWings(Simulation& System, const std::string& lowModType)
{
  ELog::RegMethod RegA("makeESS","buildPreWings");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  enum Side {bottom, top};
  
  TopPreWing = std::shared_ptr<PreModWing>(new PreModWing("TopPreWing"));
  OR.addObject(TopPreWing);
  TopPreWing->createAll(System, *TopPreMod, 9, false, top, *TopMod);
  attachSystem::addToInsertSurfCtrl(System, *TopPreMod, *TopPreWing);

  TopCapWing = std::shared_ptr<PreModWing>(new PreModWing("TopCapWing"));
  OR.addObject(TopCapWing);
  TopCapWing->createAll(System, *TopCapMod, 10, false, bottom, *TopMod);
  attachSystem::addToInsertSurfCtrl(System, *TopCapMod, *TopCapWing);

  if (lowModType != "None")
    {
      LowPreWing = std::shared_ptr<PreModWing>(new PreModWing("LowPreWing"));
      OR.addObject(LowPreWing);
      LowPreWing->createAll(System, *LowPreMod, 9, true, bottom, *LowMod);
      attachSystem::addToInsertSurfCtrl(System, *LowPreMod, *LowPreWing);

      LowCapWing = std::shared_ptr<PreModWing>(new PreModWing("LowCapWing"));
      OR.addObject(LowCapWing);
      LowCapWing->createAll(System, *LowCapMod, 10, true, top, *LowMod);
      attachSystem::addToInsertSurfCtrl(System, *LowCapMod, *LowCapWing);
    }
}
  
void
makeESS::buildTwister(Simulation& System)
{
  ELog::RegMethod RegA("makeESS","buildTwister");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  Twister = std::shared_ptr<TwisterModule>(new TwisterModule("Twister"));
  OR.addObject(Twister);

  Twister->createAll(System,*Bulk);
  attachSystem::addToInsertForced(System, *Bulk, *Twister);
  attachSystem::addToInsertForced(System, *ShutterBayObj, *Twister);
  attachSystem::addToInsertSurfCtrl(System, *Twister, PBeam->getCC("Sector0"));
  attachSystem::addToInsertSurfCtrl(System, *Twister, PBeam->getCC("Sector1")); ELog::EM << "remove this line after R is set correctly " << ELog::endDiag;
  attachSystem::addToInsertControl(System, *Twister, *Reflector);
  attachSystem::addToInsertForced(System,*Twister,TopAFL->getCC("outer"));
  attachSystem::addToInsertForced(System,*Twister,TopBFL->getCC("outer"));
  attachSystem::addToInsertForced(System,*Twister,LowAFL->getCC("outer"));
  attachSystem::addToInsertForced(System,*Twister,LowBFL->getCC("outer"));
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

  int voidCell(74123);

  const std::string lowPipeType=IParam.getValue<std::string>("lowPipe");
  const std::string topPipeType=IParam.getValue<std::string>("topPipe");

  const std::string lowModType=IParam.getValue<std::string>("lowMod"); // if None then BeRefLowVoidThick must be set to 0.0
  const std::string topModType=IParam.getValue<std::string>("topMod");
  
  const std::string targetType=IParam.getValue<std::string>("targetType");
  const std::string iradLine=IParam.getValue<std::string>("iradLineType");


  //  const size_t nF5 = IParam.getValue<size_t>("nF5");

  if (StrFunc::checkKey("help",lowPipeType,lowModType,targetType) ||
      StrFunc::checkKey("help",iradLine,topModType,""))
    {
      optionSummary(System);
      throw ColErr::ExitAbort("Help system exit");
    }
  
  makeTarget(System,targetType);
  Reflector->globalPopulate(System.getDataBase());

  // lower moderator
  if (lowModType != "None")
    LowPreMod->createAll(System,World::masterOrigin(),0,true,
			 Target->wheelHeight()/2.0,
			 Reflector->getRadius(),false);

  TopPreMod->createAll(System,World::masterOrigin(),0,false,
		       Target->wheelHeight()/2.0,
		       Reflector->getRadius(),true);
  
  if (lowModType != "None")
    buildLowButterfly(System);
  buildTopButterfly(System);
  const double LMHeight=(lowModType == "None") ? 0.0 : attachSystem::calcLinkDistance(*LowMod,5,6);
  const double TMHeight=attachSystem::calcLinkDistance(*TopMod,5,6);
  
  // Cap moderator DOES not span whole unit
  TopCapMod->createAll(System,*TopMod,6,false,
   		       0.0,Reflector->getRadius(),false);

  if (lowModType != "None")
    LowCapMod->createAll(System,*LowMod,6,false,
			 0.0,Reflector->getRadius(),true);

  if (lowModType != "None")
    Reflector->createAll(System,World::masterOrigin(),
			 Target->wheelHeight(),
			 LowPreMod->getHeight()+LMHeight+LowCapMod->getHeight(),
			 TopPreMod->getHeight()+TMHeight+TopCapMod->getHeight());
  else
    Reflector->createAll(System,World::masterOrigin(),
			 Target->wheelHeight(),
			 0.0,
			 TopPreMod->getHeight()+TMHeight+TopCapMod->getHeight());
  
  buildPreWings(System,lowModType);

  Reflector->insertComponent(System,"targetVoid",*Target,1);
  Reflector->deleteCell(System,"lowVoid");
  Reflector->deleteCell(System,"topVoid");
  Bulk->createAll(System,*Reflector,*Reflector);
  // Build flightlines after bulk
  TopAFL->createAll(System,*TopMod,0,*Reflector,4,*Bulk,-3);
  TopBFL->createAll(System,*TopMod,0,*Reflector,3,*Bulk,-3);

  if (lowModType != "None")
    {
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


  if (lowModType != "None")
    {
      createGuides(System);
      makeBunker(System,IParam);
    }

  // PROTON BEAMLINE
  

  PBeam->createAll(System,*Target,1,*ShutterBayObj,-1);
  Reflector->insertComponent(System, "targetVoid", PBeam->getCC("Sector0"));
  
  attachSystem::addToInsertSurfCtrl(System,*ShutterBayObj,
				    PBeam->getCC("Full"));
  attachSystem::addToInsertSurfCtrl(System,*Bulk,
				    PBeam->getCC("Full"));

  const int engActive = System.getDataBase().EvalPair<int>("Bulk", "", "EngineeringActive");
  if (engActive)
    {
      buildTwister(System);
    }

  if (lowModType != "None")
    makeBeamLine(System,IParam);
  buildF5Collimator(System, IParam);

  // WARNING: THESE CALL MUST GO AFTER the main void (74123) has
  // been completed. Otherwize we can't find the pipe in the volume.
  if (lowModType != "None")
    ModPipes->buildLowPipes(System,lowPipeType);
  ModPipes->buildTopPipes(System,topPipeType);

  return;
}


}   // NAMESPACE essSystem

