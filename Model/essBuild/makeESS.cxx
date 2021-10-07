/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/makeESS.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell/Konstantin Batkov
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

#include "BnId.h"
#include "AcompTools.h"
#include "Acomp.h"
#include "Algebra.h"
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "Object.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "FrontBackCut.h"
#include "LayerComp.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "BasicFlightLine.h"
#include "WedgeFlightLine.h"
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
#include "EmptyCyl.h"
#include "BeRef.h"
#include "TelescopicPipe.h"
#include "ProtonTube.h"
#include "PBIP.h"
#include "ModBase.h"
#include "EssModBase.h"
#include "ConicInfo.h"
#include "CylMod.h"
#include "H2Wing.h"
#include "ButterflyModerator.h"
#include "PancakeModerator.h"
#include "BoxModerator.h"
#include "BoxD2Moderator.h"
#include "BlockAddition.h"
#include "CylPreMod.h"
#include "PreModWing.h"
#include "IradCylinder.h"
//#include "BulkModule.h"
#include "LayeredBulkModule.h"
#include "TwisterModule.h"
#include "ShutterBay.h"
#include "GuideBay.h"
#include "FixedOffsetGroup.h"
#include "GuideItem.h"
#include "GuideLine.h"
#include "DiskLayerMod.h"
#include "Bunker.h"
#include "pillarInfo.h"
#include "RoofPillars.h"
#include "BunkerFeed.h"
#include "BunkerQuake.h"
#include "Curtain.h"
#include "HighBay.h"
#include "ConicModerator.h"
#include "makeESSBL.h"
#include "ESSPipes.h"
#include "F5Calc.h"
#include "F5Collimator.h"
#include "TSMainBuilding.h"
#include "MatMesh.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Chicane.h"
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


  
  Bulk(new LayeredBulkModule("Bulk")),
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
  
  // const int voidCell(74123);
  // Target->addInsertCell("Shaft",voidCell);
  // Target->addInsertCell("Wheel",voidCell);
  Target->createAll(System,World::masterOrigin(),0);

  OR.addObject(Target);
  return;
}

void
makeESS::makeTargetClearance(Simulation& System, const int engActive)
{
  /*!
    Build clearance above and below the taret wheel
    \param System :: Simulation
    \param engActive :: Engineering active flag
   */


  const FuncDataBase& Control=System.getDataBase();
  const int engTwisterActive=std::max(engActive, Control.EvalDefVar<int>
				      ("TwisterEngineeringActive",0));

  
  TargetTopClearance = std::shared_ptr<EmptyCyl>(new EmptyCyl("TargetTopClearance"));
  TargetLowClearance = std::shared_ptr<EmptyCyl>(new EmptyCyl("TargetLowClearance"));


  
  if (engTwisterActive)
    {
      buildTwister(System);
      TargetTopClearance->createAll(System,*Target,6,3,13,*Twister,-16,
				    *GBArray[0],4,
				    *GBArray[1],3);
      TargetLowClearance->createAll(System,*Target,5,3,13,*Twister,-16,
				    *GBArray[0],4,
				    *GBArray[1],3);

    }
  else
    {
      TargetTopClearance->createAll(System,*Target,6,3,13,*Bulk,-9,
				    *GBArray[0],4,
				    *GBArray[1],3);
      TargetLowClearance->createAll(System,*Target,5,3,13,*Bulk,-9,
				    *GBArray[0],4,
				    *GBArray[1],3);
    }

  for (const std::shared_ptr<GuideBay> GB : GBArray)
    {
      attachSystem::addToInsertSurfCtrl(System,*GB,*TargetTopClearance);
      attachSystem::addToInsertSurfCtrl(System,*GB,*TargetLowClearance);

      std::vector<std::shared_ptr<GuideItem> > GUnit = GB->getGuideItems();
      for (const std::shared_ptr<GuideItem> GA : GUnit)
	{
	  if (GA->isActive())
	    {
	      ELog::EM << GA->getKeyName() << ELog::endDiag;
	      attachSystem::addToInsertSurfCtrl(System,*GA,*TargetTopClearance);
	    }
	}
    }


  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(TargetTopClearance);
  OR.addObject(TargetLowClearance);
  //Will be replaced by insertions to other 
  attachSystem::addToInsertSurfCtrl(System,*TopAFL,*TargetTopClearance);
  attachSystem::addToInsertSurfCtrl(System,*TopBFL,*TargetTopClearance);

  attachSystem::addToInsertSurfCtrl(System,*LowAFL,*TargetLowClearance);
  attachSystem::addToInsertSurfCtrl(System,*LowBFL,*TargetLowClearance);

  attachSystem::addToInsertSurfCtrl(System,*LowBFL,Target->getCC("Wheel"));
  attachSystem::addToInsertSurfCtrl(System,*LowAFL,Target->getCC("Wheel"));
  attachSystem::addToInsertSurfCtrl(System,*TopBFL,Target->getCC("Wheel"));
  attachSystem::addToInsertSurfCtrl(System,*TopAFL,Target->getCC("Wheel"));

    
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
      attachSystem::addToInsertSurfCtrl(System,*GB,Target->getCC("Wheel"));
      GBArray.push_back(GB);

      GB->createGuideItems(System,"Top",Target->getKeyName());
      GB->createGuideItems(System,"Low",Target->getKeyName());
    }

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
	    System.getObject<attachSystem::FixedComp>(compName);
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
  ELog::RegMethod RegA("makeESS","buildTopButtefly");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<ButterflyModerator> BM
    (new essSystem::ButterflyModerator("TopFly"));
  BM->setRadiusX(Reflector->getRadius());
  TopMod=std::shared_ptr<EssModBase>(BM);
  OR.addObject(TopMod);

  BM->createAll(System,*TopPreMod.get(),6,*Reflector,0);
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
makeESS::buildLowD2Box(Simulation& System)
  /*!
    Build the lower box moderator
    \param System :: Stardard simulation
  */
{
  ELog::RegMethod RegA("makeESS","buildLowBox");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<BoxD2Moderator> BM
    (new essSystem::BoxD2Moderator("LowD2Box"));

  
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
  ELog::RegMethod RegA("makeESS", "buildF5Collimator(System, nF5)");
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

  const FuncDataBase& Control=System.getDataBase();
  const double phiAB(1.0-Control.EvalVar<double>("ABunkerRightPhase")-Control.EvalVar<double>("ABunkerLeftPhase"));
  const double phiCD(359.0+Control.EvalVar<double>("CBunkerRightPhase")+Control.EvalVar<double>("CBunkerLeftPhase"));
  std::vector<Geometry::Vec3D> vecFP;
  size_t colIndex(0);
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
	      theta = atof(strtmp.c_str()); // !!! use StrFunc::convert here !!!

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
	      const attachSystem::FixedComp* midWater = System.getObject<attachSystem::FixedComp>(midWaterName);
	      if (!midWater)
		throw ColErr::InContainerError<std::string>
		  (midWaterName,"Component not found");

	      lobeName = moderator + "LeftLobe";
	      const attachSystem::FixedComp* lobe = System.getObject<attachSystem::FixedComp>(lobeName);
	      if (!lobe)
		throw ColErr::InContainerError<std::string>
		  (lobeName,"Component not found");

	      for (long ii=0; ii<8; ii++) // we need 4,5,6,7
		vecFP.push_back(midWater->getLinkPt(ii+1));
	      vecFP.push_back(lobe->getLinkPt(13)); // zmin
	      vecFP.push_back(lobe->getLinkPt(14)); // zmax
	      F5->setFocalPoints(vecFP);
	      /////

	      F5->createAll(System, World::masterOrigin());

	      if (theta<phiAB)
		{
		  attachSystem::addToInsertSurfCtrl(System, *ABunker, *F5);
		  if (std::abs(theta-phiAB)<1.0) // Special care when theta is close to normals
		    attachSystem::addToInsertSurfCtrl(System, *BBunker, *F5);
		}
	      else if (theta<180.0)
		{
		  attachSystem::addToInsertSurfCtrl(System, *BBunker, *F5);
		  if (std::abs(theta-phiAB)<1.0) // Special care when theta is close to normals
		    attachSystem::addToInsertSurfCtrl(System, *ABunker, *F5);
		}
	      else if (theta<phiCD)
		{
		  attachSystem::addToInsertSurfCtrl(System, *DBunker, *F5);
		  if (std::abs(theta-phiCD)<1.0) // Special care when theta is close to normals
		    attachSystem::addToInsertSurfCtrl(System, *CBunker, *F5);
		}
	      else if (theta<=360.0)
		{
		  attachSystem::addToInsertSurfCtrl(System, *CBunker, *F5);
		  if (std::abs(theta-phiCD)<1.0) // Special care when theta is close to normals
		    attachSystem::addToInsertSurfCtrl(System, *DBunker, *F5);
		}

	      F5array.push_back(F5);
	    }
	} else if ((strtmp=="TopCake") || (strtmp=="TopBox"))
	{
	  moderator = strtmp;
	  range = IParam.getValue<std::string>("f5-collimators", ++i); // thermal or cold
	  for (size_t j=i+1; j<nitems; j++)
	    {
	      strtmp = IParam.getValue<std::string>("f5-collimators", j);
	      theta = std::stod(strtmp);
	      std::shared_ptr<F5Collimator> F5(new F5Collimator("F" + std::to_string(colIndex*10+5))); colIndex++;
	      OR.addObject(F5);
	      F5->setTheta(theta);
	      F5->setRange(range); // thermal or cold

	      // set up the focal points
	      const std::string midH2Name = moderator+"MidH2";
	      const attachSystem::FixedComp *midH2 = System.getObject<attachSystem::FixedComp>(midH2Name);
	      if (!midH2)
		throw ColErr::InContainerError<std::string>
		  (midH2Name,"Component not found");

	      // just fill vecFP with large enough coordinates, so
	      // F5Collimator.cxx could work:
	      for (size_t i=0; i<10; i++)
		vecFP.push_back(midH2->getLinkPt(0));
	      
	      // last two must be zmin/zmax:
	      vecFP.push_back(midH2->getLinkPt(7+1));
	      vecFP.push_back(midH2->getLinkPt(8+1));
	      F5->setFocalPoints(vecFP);

	      F5->addInsertCell(74123); // !!! 74123=voidCell // SA: how to exclude F5 from any cells?
	      F5->createAll(System, World::masterOrigin());
	      attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *F5);
	      F5array.push_back(F5);
	    }
	}
    } // for

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
	    System.getObjectThrow<attachSystem::FixedComp>(segObj,"Chicane Object");

	  const long int linkIndex=FCPtr->getSideIndex(linkName);
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
makeESS::buildPillars(Simulation& System)
  /*!
    Build the pillars in the bunker
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("makeESS","buildPillars");
  //  ABunkerPillars->createAll(System,*ABunker);
  BBunkerPillars->createAll(System,*BBunker);
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
	  // Here do insertions of the flight paths for NNBAR beamline
	  if((Btype=="NNBAR")&&(BLNum.first==1)&&(BLNum.second==11))
	   buildNNBARFlights(System);
	  
	  
	  if (BLNum.first==1 && BLNum.second<=9) // <=11)
	    BLfactory.build(System,*ABunker);
	  else if (BLNum.first==1 && BLNum.second>9)  //>11)
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
makeESS::buildTwisterFlight(Simulation& System)
  /*!
    Flight paths in twister for NNBAR beamline
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("makeESS","buildTwisterFlight");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  TwisterFlightNNBAR =
    std::shared_ptr<beamlineSystem::GuideLine>
    (new beamlineSystem::GuideLine("TwisterFlightNNBAR"));
  OR.addObject(TwisterFlightNNBAR);

  TwisterFlightOther =
    std::shared_ptr<beamlineSystem::GuideLine>
    (new beamlineSystem::GuideLine("TwisterFlightOther"));
  OR.addObject(TwisterFlightOther);

  
  TwisterFlightNNBAR->
    setFront(LowMod->getComponent("MidD2").getLinkSurf(2));

  //First split twister cells
  // Low part
  std::vector<int> iCellTLow=Twister->getCells("PlugVolumeLow");
  std::vector<int> iCellTNNBAR;
  std::vector<int> iCellTOther;

     for (size_t i = 0; i<iCellTLow.size(); i++){
    try{
      iCellTNNBAR.push_back(
       System.splitObject(iCellTLow[i],
			  LowMod->getComponent("MidD2").getLinkSurf(2)));
    }catch(...)
      {};

    try{
      iCellTOther.push_back(
       System.splitObject(iCellTLow[i],
			  LowMod->getComponent("MidD2").getLinkSurf(1)));
    }catch(...)
      {};
      TwisterFlightOther->addInsertCell(iCellTOther[i]);
    }
     //Also top part, to avoid problems with the moderator
     for (size_t i=0; i<iCellTNNBAR.size(); i++)
         for (size_t i=0; i<iCellTOther.size(); i++)
       Twister->addCells("PlugVolumeLowOther",iCellTOther);
   Twister->addCells("PlugVolumeLowNNBAR",iCellTNNBAR);
   
    
  TwisterFlightNNBAR->addInsertCell(LowMod->getCell("MainVolume"));

  TwisterFlightNNBAR->createAll(System,
				*LowMod,2,*LowMod,2);

  addSurfaceToHRule(System,
		    TwisterFlightNNBAR->getCells(),
		    -Reflector->getLinkSurf(2));
  
  TwisterFlightOther->
    setFront(LowMod->getComponent("MidD2").getLinkSurf(1));

  TwisterFlightOther->addInsertCell(Twister->
				    getCells("PlugFrameMinusLow"));
  TwisterFlightOther->addInsertCell(Twister->
				    getCells("PlugFrameOuterLow"));

  TwisterFlightOther->addInsertCell(LowMod->getCell("MainVolume"));

  TwisterFlightOther->createAll(System,
				*LowMod,1,*LowMod,1);

  addSurfaceToHRule(System,
		    TwisterFlightOther->getCells(),
		    Twister->getLinkString(16));

  
  return;
  				
}



void
makeESS::buildNNBARFlights(Simulation& System)
//			   const attachSystem::FixedComp& Bulk,
//			   const attachSystem::FixedComp& Reflector)

{
  ELog::RegMethod RegA("makeESS","buildNNBARFlights");
  ModelSupport::objectRegister& OR=
         ModelSupport::objectRegister::Instance();

     LowNNBARFL = std::shared_ptr<WedgeFlightLine>
       (new WedgeFlightLine("LowNNBARFlight"));
 
     TopNNBARFL = std::shared_ptr<WedgeFlightLine>
       (new WedgeFlightLine("TopNNBARFlight"));

     LowTwisterNNBARFL = std::shared_ptr<WedgeFlightLine>
       (new WedgeFlightLine("LowTwisterNNBARFlight"));
 
     TopTwisterNNBARFL = std::shared_ptr<WedgeFlightLine>
       (new WedgeFlightLine("TopTwisterNNBARFlight"));
     // Low NNBAR flight originates not from low moderator center
     // Arranging its position according to ESS-1210581.2 w.r.t. origin


    
  OR.addObject(TopNNBARFL);
  OR.addObject(LowNNBARFL);
  OR.addObject(LowTwisterNNBARFL);
  OR.addObject(TopTwisterNNBARFL);
   
      
      //To create top flight with respect to top moderator link 0:
      //TopNNBARFL->createAll(System,*TopMod,0,*Reflector,4,*Bulk,-3);
      //Creating with respect to master Origin
     TopNNBARFL->
      	createAll(System,World::masterOrigin(),0,
		  *Reflector, 4,
		  *Bulk,-3);

      attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("WingATop"),
					TopNNBARFL->getCC("outer"));
      attachSystem::addToInsertSurfCtrl(System,*TopAFL,
					TopNNBARFL->getCC("outer"));
      attachSystem::addToInsertSurfCtrl(System,
				      *TopNNBARFL,
				      *TargetTopClearance);
      attachSystem::addToInsertSurfCtrl(System,*LowNNBARFL,
				      *TargetLowClearance);
      attachSystem::addToInsertSurfCtrl(System,
				      *TopNNBARFL,
					Target->getCC("Wheel"));
    //Limiting flight line to bulk only

     attachSystem::addToInsertSurfCtrl(System,
				 TopNNBARFL->getCells(),
				      Twister->getCC("PlugFrame"));

     TopTwisterNNBARFL->createAll(System,World::masterOrigin(),0,
		  *Reflector, 4,*Bulk,-3);
     
         attachSystem::addToInsertForced(System,
     		     Twister->getCells("PlugVolumeTopA"),
     				     TopTwisterNNBARFL->getCC("outer"));
         attachSystem::addToInsertForced(System,
     		     Twister->getCells("PlugFramePlusTop"),
     				     TopTwisterNNBARFL->getCC("outer"));
         attachSystem::addToInsertForced(System,
     		     Twister->getCells("PlugFrameOuterTop"),
     				     TopTwisterNNBARFL->getCC("outer"));
     addSurfaceToHRule(System,TopTwisterNNBARFL->getCells(),
		       Twister->getLinkString(16));           


   
  LowNNBARFL->
       createAll(System,World::masterOrigin(),0,
		 //LowMod->getComponent("MidD2"),2,
		 *Reflector, 4, *Bulk,-3);
     //Do this insertion just in case LowAFlight is not large enough:
     attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("WingALow"),
					LowNNBARFL->getCC("outer"));
     attachSystem::addToInsertSurfCtrl(System,*LowAFL,
				LowNNBARFL->getCC("outer"));

    attachSystem::addToInsertSurfCtrl(System,
				 LowNNBARFL->getCells(),
					Twister->getCC("PlugFrame"));

  LowTwisterNNBARFL->
         createAll(System,World::masterOrigin(),0,
		 *Reflector, 4, *Bulk,-3);

    attachSystem::addToInsertForced(System,
				 Twister->getCells("PlugVolumeLowNNBAR"),
				  LowTwisterNNBARFL->getCC("outer"));  

    attachSystem::addToInsertForced(System,
				 Twister->getCells("PlugFramePlusLow"),
				  LowTwisterNNBARFL->getCC("outer"));  
    attachSystem::addToInsertForced(System,
				 Twister->getCells("PlugFrameOuterLow"),
				  LowTwisterNNBARFL->getCC("outer"));  
         
    
    addSurfaceToHRule(System, LowTwisterNNBARFL->getCells(),
		      Twister->getLinkString(16));

    addSurfaceToHRule(System, LowNNBARFL->getCells(),
		      "("+std::to_string(Target->getLinkSurf(3))
      + " : "+std::to_string(TargetLowClearance->getLinkSurf(4))+")");
      
     
      std::vector<int>   iCells = Bulk->getCells("WingAMid");
  for (size_t i =0; i< iCells.size(); i++){
    iCells[i]=System.splitObject(iCells[i],TopNNBARFL->getLinkSurf(-3));
    iCells[i]=System.splitObject(iCells[i],TopNNBARFL->getLinkSurf(-4));
  }
    addSurfaceToHRule(System,iCells,
		      std::to_string(LowNNBARFL->getLinkSurf(6))+" "+
		      std::to_string(TopNNBARFL->getLinkSurf(5)));

    addSurfaceToHRule(System,TopNNBARFL->getCells(),
		      LowNNBARFL->getLinkSurf(6));
    

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

  TopPreWingA = std::shared_ptr<PreModWing>
    (new PreModWing("TopLeftPreWing"));
    
  OR.addObject(TopPreWingA);
  TopPreWingA->setDivider(TopMod->getMainRule(-7));
  TopPreWingA->setInnerExclude(TopMod->getLeftExclude());
  TopPreWingA->setMidExclude(TopMod->getLeftFarExclude());

  TopPreWingA->setBaseCut(TopPreMod->getSurfRules("Layer2"));
  TopPreWingA->setTopCut(TopCapMod->getFullRule(5));
  TopPreWingA->setOuter(TopPreMod->getSurfRule("-OuterRad"));
  TopPreWingA->addInsertCell(TopMod->getCells("MainVoid"));
  TopPreWingA->createAll(System,*TopMod,0);

  TopPreWingB =
    std::shared_ptr<PreModWing>(new PreModWing("TopRightPreWing"));
      
  OR.addObject(TopPreWingB);
  TopPreWingB->setDivider(TopMod->getMainRule(7));
  TopPreWingB->setInnerExclude(TopMod->getRightExclude());
  TopPreWingB->setMidExclude(TopMod->getRightFarExclude());
  TopPreWingB->setBaseCut(TopPreMod->getFullRule(6));
  TopPreWingB->setTopCut(TopCapMod->getFullRule(5));
  TopPreWingB->setOuter(TopPreMod->getSurfRule("-OuterRad"));
      
  TopPreWingB->addInsertCell(TopMod->getCells("MainVoid"));
  TopPreWingB->createAll(System,*TopMod,0);

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

   //     attachSystem::addToInsertSurfCtrl(System,*Bulk,
   //				    Twister->getCC("PlugFrame"));
  System.removeCell(Bulk->getCell("Inner"));
   
   
  ELog::EM<<"CALLING addInsertForce [INEFFICIENT] "<<ELog::endWarn;
  attachSystem::addToInsertForced(System,*ShutterBayObj,
				  Twister->getCC("Shaft"));


  attachSystem::addToInsertControl(System, *Twister, *Reflector);
  Twister->addCell("PlugVolumeAbove",
	    System.splitObject(Twister->getCell("PlugVolume"),
			       Reflector->getLinkSurf(6)));

  Twister->addCell("PlugVolumeBelow",
  	    System.splitObject(Twister->getCell("PlugVolume"),
  			       Reflector->getLinkSurf(5)));
  
  // addSurfaceToHRule(System,Twister->getCells("PlugVolume"),
  //		     Reflector->getLinkString(4));
 // Twister->addCell("PlugVolumeTB",newcell);
 
  // split Twister by components
  std::vector<int> iCellT=Twister->getCells("PlugVolume");
  std::vector<int> iCellTLow;
  std::vector<int> iCellTTop;
       
  for (size_t i = 0; i<iCellT.size(); i++){
    try{
	iCellTTop.push_back(System.splitObject(iCellT[i],
					  Target->getLinkSurf(6)));
        iCellTLow.push_back(System.splitObject(iCellT[i],
				    Target->getLinkSurf(5)));
    }catch(...) {ELog::EM<<
  "Attempt to split a cell in Twister by non-suitable surface [WARNING] "
   <<ELog::endWarn;};  }
   Twister->addCells("PlugVolumeLow",iCellTLow);
   Twister->addCells("PlugVolumeTop",iCellTTop);


  iCellT=Twister->getCells("PlugFrameMinus");       
  for (size_t i = 0; i<iCellT.size(); i++){
    try{
      Twister->addCell("PlugFrameMinusTop",System.splitObject(iCellT[i],
			      Target->getLinkSurf(6)));
      Twister->addCell("PlugFrameMinusLow",System.splitObject(iCellT[i],
				    Target->getLinkSurf(5)));
    }catch(...){ELog::EM<<
  "Attempt to split a cell in Twister by non-suitable surface [WARNING] "
   <<ELog::endWarn;};}

    iCellT=Twister->getCells("PlugFramePlus");       
  for (size_t i = 0; i<iCellT.size(); i++){
    try{
      Twister->addCell("PlugFramePlusTop",System.splitObject(iCellT[i],
			      Target->getLinkSurf(6)));
      Twister->addCell("PlugFramePlusLow",System.splitObject(iCellT[i],
			      Target->getLinkSurf(5)));
    }catch(...){ELog::EM<<
  "Attempt to split a cell in Twister by non-suitable surface [WARNING] "
   <<ELog::endWarn; };}

    iCellT=Twister->getCells("PlugFrameOuter");       
  for (size_t i = 0; i<iCellT.size(); i++){
    try{
      Twister->addCell("PlugFrameOuterTop",System.splitObject(iCellT[i],
			      Target->getLinkSurf(6)));
      Twister->addCell("PlugFrameOuterLow",System.splitObject(iCellT[i],
			      Target->getLinkSurf(5)));
    }catch(...){ELog::EM<<
  "Attempt to split a cell in Twister by non-suitable surface [WARNING] "
   <<ELog::endWarn; };}


  attachSystem::addToInsertSurfCtrl(System,
                                    Twister->getCells("PlugVolume"),
				    Target->getCC("Wheel"));

  attachSystem::addToInsertSurfCtrl(System,
                                    Twister->getCells("PlugFrameOuter"),
				    Target->getCC("Wheel"));

  attachSystem::addToInsertSurfCtrl(System,
                                    Twister->getCells("PlugFramePlus"),
				    Target->getCC("Wheel"));

  attachSystem::addToInsertSurfCtrl(System,
                                    Twister->getCells("PlugFrameMinus"),
				    Target->getCC("Wheel"));

  // for (const ContainedComp & CC : Twister->getCC()) ...
  // use LineControl for intersections with flight lines

  //Due to further insertions in TopAFL/BFL their outer surface acquires
  //complicated shapes. In the twister volume will subtract 
  //a much simpler expression.

  std::string SN5AT = std::to_string( TopAFL->getLinkSurf(5)) +
    " " + std::to_string( TopAFL->getLinkSurf(11));
  std::string SN6AT = std::to_string( TopAFL->getLinkSurf(6)) +
    " " + std::to_string( TopAFL->getLinkSurf(12));

  std::string SN5BT = std::to_string( TopBFL->getLinkSurf(5))+
    " " + std::to_string( TopBFL->getLinkSurf(11));
  std::string SN6BT = std::to_string( TopBFL->getLinkSurf(6))+
    " " + std::to_string( TopBFL->getLinkSurf(12));

  int iSN3A = (TopAFL->getLinkSurf(3));
  int iSN4B = (TopBFL->getLinkSurf(4));
  int iSN4A = (TopAFL->getLinkSurf(4));
  int iSN3B = (TopBFL->getLinkSurf(3));


  std::string SN3A = std::to_string(TopAFL->getLinkSurf(3));
  std::string SN4B = std::to_string(TopBFL->getLinkSurf(4));
  std::string SN4A = std::to_string(TopAFL->getLinkSurf(4));
  std::string SN3B = std::to_string(TopBFL->getLinkSurf(3));

 {
  // code from Simulation::splitObject
  int CA = Twister->getCell("PlugVolumeTop");
  //  Bulk->setCell("Wheel", CA); 
  MonteCarlo::Object* CPtr = System.findObject(CA);
  if (!CPtr)
    throw ColErr::InContainerError<int>(CA,"Cell not found");
  CPtr->populate();
  
  // headrules +/- surface
  HeadRule CHead=CPtr->getHeadRule();
  HeadRule DHead(CHead);
  HeadRule EHead(CHead);
  HeadRule FHead(CHead);
  
  CHead.addIntersection(SN3A);
  CHead.addIntersection(SN4B);

  Twister->addCell("PlugVolumeTopBack",CA);
  CPtr->procHeadRule(CHead);
  CPtr->populate();
  CPtr->createSurfaceList();
  const std::string rulestring3A4A=
    std::to_string(-iSN3A)+" "+std::to_string(-iSN4A);
  DHead.addIntersection(rulestring3A4A);
  const std::string rulestring4A3B=
    std::to_string(iSN4A)+" "+std::to_string(iSN3B);
  EHead.addIntersection(rulestring4A3B);
  const std::string rulestring3B4B=
    std::to_string(-iSN4B)+" "+std::to_string(-iSN3B);
  FHead.addIntersection(rulestring3B4B);

  const int CB=System.getNextCell(CA);
  Twister->makeCell("PlugVolumeTopA",System,CB,CPtr->getMat(),
		 CPtr->getTemp(),DHead.display()+" "+SN5AT);
  const int CBB=System.getNextCell(CB);
  Twister->makeCell("PlugVolumeTopA",System,CBB,CPtr->getMat(),
		 CPtr->getTemp(),DHead.display()+" "+SN6AT);
 
  MonteCarlo::Object* DPtr = System.findObject(CB);  
  DPtr->populate();
  DPtr->createSurfaceList();  
 
  MonteCarlo::Object* DDPtr = System.findObject(CBB);  
  DDPtr->populate();
  DDPtr->createSurfaceList();  
  
  const int CE=System.getNextCell(CBB);

  Twister->makeCell("PlugVolumeTopBeam",System,CE,CPtr->getMat(),
		 CPtr->getTemp(),EHead.display());  
  MonteCarlo::Object* EPtr = System.findObject(CE);  
  EPtr->populate();
  EPtr->createSurfaceList();

  const int CF=System.getNextCell(CE);
  Twister->makeCell("PlugVolumeTopB",System,CF,CPtr->getMat(),
		    CPtr->getTemp(),FHead.display()+" "+SN5BT);
  MonteCarlo::Object* FPtr = System.findObject(CF);  
  FPtr->populate();
  FPtr->createSurfaceList();

  const int CFF=System.getNextCell(CF);
  Twister->makeCell("PlugVolumeTopB",System,CFF,CPtr->getMat(),
		    CPtr->getTemp(),FHead.display()+" "+SN6BT);
  MonteCarlo::Object* FFPtr = System.findObject(CFF);  
  FFPtr->populate();
  FFPtr->createSurfaceList();

 }

 addSurfaceToHRule(System,Twister->getCells("PlugFramePlusTop"),
		   "( "+SN3A+" : "+SN4A+" : "+SN5AT+" : "+SN6AT+" )");

 addSurfaceToHRule(System,Twister->getCells("PlugFrameMinusTop"),
 "( "+SN3B+" : "+SN4B+" : "+SN5BT+" : "+SN6BT+" )");

 addSurfaceToHRule(System,Twister->getCells("PlugFrameOuterTop"),
 "("+SN3A+" : "+SN4A+" : "+SN5AT+" : "+SN6AT+" )"+
 "("+SN3B+" : "+SN4B+" : "+SN5BT+" : "+SN6BT+" )");


 attachSystem::addToInsertForced(System,
 		Twister->getCells("Shaft"),TopBFL->getCC("outer"));


attachSystem::addToInsertSurfCtrl(System,*LowAFL,
				  Twister->getCC("PlugFrame"));
attachSystem::addToInsertSurfCtrl(System,LowBFL->getCells(),
				Twister->getCC("PlugFrame"));


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

  const std::string lowModType=IParam.getValue<std::string>("lowMod"); // if None then BeRefLowVoidThick must be set to 0.0
  const std::string topModType=IParam.getValue<std::string>("topMod");

  const std::string targetType=IParam.getValue<std::string>("targetType");
  const std::string iradLine=IParam.getValue<std::string>("iradLineType");

  const int matmesh=IParam.getValue<int>("matmesh"); // generate material mesh

  const int engActive=Control.EvalPair<int>
    ("BulkEngineeringActive","EngineeringActive");

  const int engTwisterActive=engActive>0? engActive: Control.EvalDefVar<int>
    ("TwisterEngineeringActive",0);

  
  // OR better set it to the flag value?
  //  const int engActive=IParam.flag("eng");
  
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


  ELog::EM << "Constructing " << topModType << " for top moderator"
	   << ELog::endDiag;
  if (topModType == "Butterfly")
    buildTopButterfly(System);
  else if (topModType == "Pancake")
    buildTopPancake(System);
  else if (topModType == "Box")
    buildTopBox(System);

  ELog::EM << "Constructing " << lowModType << " for low moderator"
	   << ELog::endDiag;
  if (lowModType == "Butterfly")
    buildLowButterfly(System);
  else if (lowModType == "Pancake")
    buildLowPancake(System);
  else if (lowModType == "Box")
    buildLowBox(System);
  else if (lowModType == "D2Box")
    buildLowD2Box(System);
  else if (lowModType != "None")
    throw ColErr::InContainerError<std::string>(lowModType,"Low Mod Type");

  const double LMHeight=(lowModType == "None") ? 0.0 : LowMod->getLinkDistance(5,6);

  const double TMHeight=TopMod->getLinkDistance(5,6);


  // Cap moderator DOES not span whole unit
  TopCapMod->createAll(System,*TopMod,6,false,
   		       0.0,Reflector->getRadius());

  if (lowModType != "None")
    LowCapMod->createAll(System,*LowMod,6,false,
			 0.0,Reflector->getRadius());

  buildPreWings(System);

  if (lowModType != "None"){
    Reflector->createAll(System,World::masterOrigin(),
	 Target->wheelHeight(),
	 LowPreMod->getHeight()+LMHeight+LowCapMod->getHeight(),
	 TopPreMod->getHeight()+TMHeight+TopCapMod->getHeight());
    }  else
    Reflector->createAll(System,World::masterOrigin(),
	 Target->wheelHeight(),
	 0.0,
	 TopPreMod->getHeight()+TMHeight+TopCapMod->getHeight());


  Reflector->insertComponent(System,"targetVoid",*Target,1);
  Reflector->deleteCell(System,"lowVoid");
  Reflector->deleteCell(System,"topVoid");
  Bulk->createAll(System,*Reflector,*Reflector);

  // Build flightlines after bulk
  if((engActive)||(engTwisterActive))
    {TopAFL->createAll(System,*TopMod,0,*Reflector,4,*Bulk,-3);
    TopBFL->createAll(System,*TopMod,0,*Reflector,3,*Bulk,-3);
  }else{
  TopAFL->createAll(System,*TopMod,0,*Bulk,-9,*Bulk,-3);
  TopBFL->createAll(System,*TopMod,0,*Bulk,-9,*Bulk,-3);
  };
  
  if (lowModType != "None")
    {
      if((engActive)||(engTwisterActive)) {
      LowAFL->createAll(System,*LowMod,0,*Reflector,4,*Bulk,-3);
      LowBFL->createAll(System,*LowMod,0,*Reflector,3,*Bulk,-3);
      }else{
      LowAFL->createAll(System,*LowMod,0,*Bulk,-9,*Bulk,-3);
      LowBFL->createAll(System,*LowMod,0,*Bulk,-9,*Bulk,-3);
      }
    }
  else
    {
      // If LowMod is not built then still create Low[AB],
      // but with respect to TopMod. In this case their ZStep and XYAngle
      // are adjusted in moderatorVariables.cxx in order to place them
      // in the original position with LowMod.
      if((engActive)||(engTwisterActive)){
   LowAFL->createAll(System,World::masterOrigin(),0,*Reflector,4,*Bulk,-3);
   LowBFL->createAll(System,World::masterOrigin(),0,*Reflector,3,*Bulk,-3);
      }else{
// Moved inner connection to the surface of void inner part of bulk
// rather then reflector
   LowAFL->createAll(System,World::masterOrigin(),0,*Bulk,-9,*Bulk,-3);
   LowBFL->createAll(System,World::masterOrigin(),0,*Bulk,-9,*Bulk,-3);
      }
    }

  // THESE calls correct the MAIN volume so pipe work MUST be after here:





  buildIradComponent(System,IParam);
  // Full surround object
  ShutterBayObj->addInsertCell(voidCell);
  ShutterBayObj->createAll(System,*Bulk,*Bulk);
    ELog::EM << "Forced insertions to ShutterBay" << ELog::endDiag;
  attachSystem::addToInsertForced(System,*ShutterBayObj,
				  Target->getCC("Wheel"));
  //  attachSystem::addToInsertForced(System,*ShutterBayObj,
  // 				  Target->getCC("Shaft"));
  attachSystem::addToInsertForced(System,*ShutterBayObj,
  				  Target->getCC("ShaftConnection"));
 
  createGuides(System);
  // build target clearance and Twister:

  makeTargetClearance(System,engActive);

  if((engTwisterActive)&&(lowModType=="D2Box")) {
    buildTwisterFlight(System);
  }
	
  // Now doing insertions into bulk in a way to avoid
  // complicated cell description
  
 //Piece of code for splitting bulk in layers:
 //Taking cells from bulk except for the central part
 std::vector<int> iBulkCells = Bulk->getCells("Outer");
 size_t iBSize = iBulkCells.size();

 // Side surfaces defining flight paths
 ELog::EM << "Assuming identical sectors for low and top flight paths!"
	  << ELog::endDiag;
 //splitting all the available bulk thickness
 int SN3A = TopAFL->getLinkSurf(3);
 int SN4B = TopBFL->getLinkSurf(4);
 int SN4A = TopAFL->getLinkSurf(4);
 int SN3B = TopBFL->getLinkSurf(3);

 for (size_t i=0; i< iBSize; i++) {
   
   // code from Simulation::splitObject
   int CA = iBulkCells[i];

   //  Bulk->setCell("Wheel", CA); 

  MonteCarlo::Object* CPtr = System.findObject(CA);
  if (!CPtr)
    throw ColErr::InContainerError<int>(CA,"Cell not found");
  CPtr->populate();

  // headrules +/- surface
  HeadRule CHead=CPtr->getHeadRule();
  HeadRule DHead(CHead);
  HeadRule EHead(CHead);
  HeadRule FHead(CHead);

  CHead.addIntersection(SN3A);
  CHead.addIntersection(SN4B);

  Bulk->addCell("Wheel",CA);
  CPtr->procHeadRule(CHead);
  CPtr->populate();
  CPtr->createSurfaceList();


  const std::string rulestring3A4A=
    std::to_string(-SN3A)+" "+std::to_string(-SN4A);
  DHead.addIntersection(rulestring3A4A);

  const std::string rulestring4A3B=
    std::to_string(SN4A)+" "+std::to_string(SN3B);
  EHead.addIntersection(rulestring4A3B);

  const std::string rulestring3B4B=
  std::to_string(-SN4B)+" "+std::to_string(-SN3B);
  FHead.addIntersection(rulestring3B4B);


  const int CB=System.getNextCell(CA);
  Bulk->makeCell("WingA",System,CB,CPtr->getMat(),
		 CPtr->getTemp(),DHead.display());

  //  Bulk->addCell("WingA",CB);
  
  MonteCarlo::Object* DPtr = System.findObject(CB);  

  DPtr->populate();
  DPtr->createSurfaceList();

  
  const int CE=System.getNextCell(CB);

  // Pipes for proton beam
  Bulk->makeCell("PBeam",System,CE,CPtr->getMat(),
		 CPtr->getTemp(),EHead.display());
  
  MonteCarlo::Object* EPtr = System.findObject(CE);  

  EPtr->populate();
  EPtr->createSurfaceList();
  // Bulk->addCell("PBeam",CE);


  const int CF=System.getNextCell(CE);
  //  Bulk->setCell("WingB", CF);
  //  System.addCell(CF,CPtr->getMat(),CPtr->getTemp(),FHead);
  Bulk->makeCell("WingB",System,CF,CPtr->getMat(),
		 CPtr->getTemp(),FHead.display());

  MonteCarlo::Object* FPtr = System.findObject(CF);  

  FPtr->populate();
  FPtr->createSurfaceList();
  //Bulk->addCell("WingB",CE);
  
 }
 ELog::EM << "Split bunker into Wings Front and Back" << ELog::endDiag;

 //Inserting Twister Plug Frame if relevant into PBeam and Wings

 if((engTwisterActive)||(engActive)){
 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("WingA"),
  				   Twister->getCC("PlugFrame"));
 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("WingB"),
  				   Twister->getCC("PlugFrame"));
 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("WingB"),
  				   Twister->getCC("Shaft"));
 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("WingB"),
  				   Twister->getCC("ShaftBearing"));
 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("Inner"),
  				   Twister->getCC("PlugFrame"));
 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("PBeam"),
  				   Twister->getCC("PlugFrame"));
 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("Top"),
  				   Twister->getCC("PlugFrame"));
 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("Lower"),
  				   Twister->getCC("PlugFrame"));
 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("Wheel"),
  				   Twister->getCC("PlugFrame"));
 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("PBeam"),
				   Twister->getCC("Shaft"));
 attachSystem::addToInsertForced(System,Bulk->getCells("Top"),
				   Twister->getCC("Shaft"));
 attachSystem::addToInsertForced(System,Bulk->getCells("Lower"),
				     Twister->getCC("ShaftBearing"));
 attachSystem::addToInsertForced(System,Bulk->getCells("PBeam"),
				     Twister->getCC("ShaftBearing"));

    } 

 // Further cuttings in the wings
 // Cutting out Low fligth path sector A
 // Leaving parts of wing below and above flight path 
 std::vector<int> iWingACells = Bulk->getCells("WingA");
 size_t iWASize = iWingACells.size();
 int SN5AL = LowAFL->getLinkSurf(5);
 int SN11AL = LowAFL->getLinkSurf(11);
 int SN6AL = LowAFL->getLinkSurf(6);
 int SN12AL = LowAFL->getLinkSurf(12);
 // if ((lowModType == "Butterfly"))
  if ((lowModType != "None"))
    {
      SN5AL = LowAFL->getLinkSurf(6);
      SN11AL = LowAFL->getLinkSurf(12);
      SN6AL = LowAFL->getLinkSurf(5);
      SN12AL = LowAFL->getLinkSurf(11);
    } 
 int SN5AT = TopAFL->getLinkSurf(5);
 int SN11AT = TopAFL->getLinkSurf(11);
 int SN6AT = TopAFL->getLinkSurf(6);
 int SN12AT = TopAFL->getLinkSurf(12);


 for (size_t i=0; i< iWASize; i++) {
   // code from Simulation::splitObject
   int CA = iWingACells[i];

   // Bulk->setCell("WingALow", CA); 

  MonteCarlo::Object* CPtr = System.findObject(CA);
  if (!CPtr)
    throw ColErr::InContainerError<int>(CA,"Cell not found");
  CPtr->populate();

  // headrules +/- surface
  HeadRule CHead=CPtr->getHeadRule();
  HeadRule DHead(CHead);
  HeadRule EHead(CHead);

  const std::string rulestring5A11AL
    = std::to_string(SN5AL) + " "+std::to_string(SN11AL);
  CHead.addIntersection(rulestring5A11AL);
  Bulk->addCell("WingALow",CA); // rename cell CA
  CPtr->procHeadRule(CHead);
  CPtr->populate();
  CPtr->createSurfaceList();
  
  
  const std::string rulestring6A12AL
    = std::to_string(SN6AL) + " "+std::to_string(SN12AL);
  const std::string rulestring5A11AT
    = std::to_string(SN5AT) + " "+std::to_string(SN11AT);
  DHead.addIntersection(rulestring6A12AL+" "+rulestring5A11AT);
  const int CB=System.getNextCell(CA);

  Bulk->makeCell("WingAMid",System,CB,CPtr->getMat(),
		 CPtr->getTemp(),DHead.display());

  
  MonteCarlo::Object* DPtr = System.findObject(CB);  

  DPtr->populate();
  DPtr->createSurfaceList();

  
  
  const std::string rulestring6A12AT
    = std::to_string(SN6AT) + " "+std::to_string(SN12AT);
  EHead.addIntersection(rulestring6A12AT);

   
  const int CE=System.getNextCell(CB);
  // Wings above top flight
  
  Bulk->makeCell("WingATop",System,CE,CPtr->getMat(),
		 CPtr->getTemp(),EHead.display());
  
  MonteCarlo::Object* EPtr = System.findObject(CE);  

  EPtr->populate();
  EPtr->createSurfaceList();
    
 }
 // ELog::EM << "Split Wing A" << ELog::endDiag;




 // Cutting out Low fligth path sector B
 // Better write a function, but yet ... 
 // Leaving parts of wing below and above flight path 
 std::vector<int> iWingBCells = Bulk->getCells("WingB");
 size_t iWBSize = iWingBCells.size();
 int SN5BL = LowBFL->getLinkSurf(5);
 int SN11BL = LowBFL->getLinkSurf(11);
 int SN6BL = LowBFL->getLinkSurf(6);
 int SN12BL = LowBFL->getLinkSurf(12);
 //if ((lowModType == "Butterfly")){
 if ((lowModType != "None")){
 SN5BL = LowBFL->getLinkSurf(6);
 SN11BL = LowBFL->getLinkSurf(12);
 SN6BL = LowBFL->getLinkSurf(5);
 SN12BL = LowBFL->getLinkSurf(11);
 }
 int SN5BT = TopBFL->getLinkSurf(5);
 int SN11BT = TopBFL->getLinkSurf(11);
 int SN6BT = TopBFL->getLinkSurf(6);
 int SN12BT = TopBFL->getLinkSurf(12);


 for (size_t i=0; i< iWBSize; i++) {
   // code from Simulation::splitObject
   int CA = iWingBCells[i];

   // Bulk->setCell("WingALow", CA); 

  MonteCarlo::Object* CPtr = System.findObject(CA);
  if (!CPtr)
    throw ColErr::InContainerError<int>(CA,"Cell not found");
  CPtr->populate();

  // headrules +/- surface
  HeadRule CHead=CPtr->getHeadRule();
  HeadRule DHead(CHead);
  HeadRule EHead(CHead);
  const std::string rulestring5B11BL
    = std::to_string(SN5BL) + " "+std::to_string(SN11BL);
  CHead.addIntersection(rulestring5B11BL);
  Bulk->addCell("WingBLow",CA) ;
  CPtr->procHeadRule(CHead);

  CPtr->populate();
  CPtr->createSurfaceList();

  
  
  const std::string rulestring6B12BL
    = std::to_string(SN6BL) + " "+std::to_string(SN12BL);
  const std::string rulestring5B11BT
    = std::to_string(SN5BT) + " "+std::to_string(SN11BT);
  DHead.addIntersection(rulestring6B12BL+" "+rulestring5B11BT);
  const int CB=System.getNextCell(CA);
  Bulk->makeCell("WingBMid",System,CB,CPtr->getMat(),
		 CPtr->getTemp(),DHead.display());
  
  MonteCarlo::Object* DPtr = System.findObject(CB);  

  DPtr->populate();
  DPtr->createSurfaceList();

 
  const std::string rulestring6B12BT
    = std::to_string(SN6BT) + " "+std::to_string(SN12BT);
  EHead.addIntersection(rulestring6B12BT);

   
  const int CE=System.getNextCell(CB);
  // Wings above top flight
  Bulk->makeCell("WingBTop",System,CE,CPtr->getMat(),
		 CPtr->getTemp(),EHead.display());
  
  MonteCarlo::Object* EPtr = System.findObject(CE);  
  EPtr->populate();
  EPtr->createSurfaceList();
    
 }

 ELog::EM << "Cut wheel and target clearance cylinders cut from Bulk wings" << ELog::endDiag;

 ELog::EM << "NB! Need to arrange a function out of that code" << ELog::endDiag;


 addSurfaceToHRule(System,
		   Bulk->getCells("WingAMid"),Target->getLinkSurf(3)); 
 addSurfaceToHRule(System,
		   Bulk->getCells("WingBMid"),Target->getLinkSurf(3)); 
 
 

 attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("Inner"),
				  Target->getCC("Wheel"));
 attachSystem::addToInsertForced(System,Bulk->getCells("Wheel"),
				  Target->getCC("Wheel"));

  ELog::EM << "Insertions of Wheel to bulk -- Forced " << ELog::endDiag;

  attachSystem::addToInsertForced(System,Bulk->getCells("Wheel"),
				  Target->getCC("Shaft"));

  attachSystem::addToInsertForced(System,Bulk->getCells("Wheel"),
				  Target->getCC("ShaftConnection"));

  attachSystem::addToInsertForced(System,Bulk->getCells("Top"),
				  Target->getCC("ShaftConnection"));
  ELog::EM << "Insertions of Wheel Shaft to Bulk" << ELog::endDiag;


    attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("Wheel"),
				    *TargetLowClearance);
    attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("Wheel"),
				    *TargetTopClearance);

			    
    /////   Done with insertions into bulk



    

  makeBunker(System,IParam);


  
  TSMainBuildingObj->addInsertCell(voidCell);
  TSMainBuildingObj->createAll(System,World::masterOrigin());
  attachSystem::addToInsertLineCtrl(System, *TSMainBuildingObj, *ShutterBayObj);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *ABunker);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *BBunker);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *CBunker);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *DBunker);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, TopCurtain->getCC("Top"));
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, TopCurtain->getCC("Mid"));
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, TopCurtain->getCC("Lower"));
  attachSystem::addToInsertForced(System, *TSMainBuildingObj,   Target->getCC("ShaftConnection"));

  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *ABHighBay);
  attachSystem::addToInsertSurfCtrl(System, *TSMainBuildingObj, *CDHighBay);

  // PROTON BEAMLINE

  pbip->createAll(System,World::masterOrigin(),0,*Bulk,-4,*Target,1);
   
  attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("PBeam"),
				    pbip->getCC("before"));
  attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("PBeam"),
				    pbip->getCC("main"));
  attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("PBeam"),
				    pbip->getCC("after"));

  attachSystem::addToInsertSurfCtrl(System,Bulk->getCells("Inner"),
				    pbip->getCC("after"));
 
  
  Reflector->insertComponent(System, "targetVoid", pbip->getCC("after"));
  /*
  PBeam->setFront(*Bulk,4);
  PBeam->setBack(*TSMainBuildingObj,-1);
  PBeam->createAll(System,*Bulk,4,*ShutterBayObj,-6);

  attachSystem::addToInsertSurfCtrl(System,*ShutterBayObj,
				    PBeam->getCC("Full"));
  // attachSystem::addToInsertSurfCtrl(System,*Bulk,
  // 				    PBeam->getCC("Full"));
  attachSystem::addToInsertSurfCtrl(System,*TSMainBuildingObj,
  				    PBeam->getCC("Sector3"));
  */
  if (Twister){
    attachSystem::addToInsertSurfCtrl(System,
	     Twister->getCells("Shaft"),pbip->getCC("after"));
    attachSystem::addToInsertSurfCtrl(System,
	     Twister->getCells("PlugVolume"),pbip->getCC("after"));
    attachSystem::addToInsertSurfCtrl(System,
    	     Twister->getCells("PlugFramePlus"),pbip->getCC("after"));
  }
  makeBeamLine(System,IParam);
  buildF5Collimator(System, IParam);

  // WARNING: THESE CALL MUST GO AFTER the main void (74123) has
  // been completed. Otherwize we can't find the pipe in the volume.

  

  //if (lowModType != "None")
    // ModPipes->buildLowPipes(System,lowPipeType);
    //   ModPipes->buildTopPipes(System,topPipeType);

   // Add feedthoughs/chicanes

  if (IParam.flag("bunkerFeed"))
    buildBunkerFeedThrough(System,IParam);
  if (IParam.flag("bunkerQuake"))
    buildBunkerQuake(System,IParam);
  if (IParam.flag("bunkerChicane"))
    buildBunkerChicane(System,IParam);


  if (IParam.flag("rotate")) // rotate to the Alan's coordinate system
    // SA says I need to use master rotation here - see his t2 model
    {
      masterRotate& MR = masterRotate::Instance();
      MR.addRotation(Geometry::Vec3D(1,0,0), Geometry::Vec3D(0,0,0), -90.0);
      MR.addRotation(Geometry::Vec3D(0,1,0), Geometry::Vec3D(0,0,0), 180.0);
    }

  if (matmesh) // generate material mesh
    {
      MatMesh m(System);
      m.Dump(matmesh);
    }

  return;
}

void makeESS::addSurfaceToHRule(Simulation& System,
				std::vector<int> iCells, int SN)
  {
    ELog::RegMethod RegA("makeESS","addSurfaceToHRule");

  size_t iBSize = iCells.size();
  for (size_t i=0; i< iBSize; i++) {   
   // code from Simulation::splitObject
   int CA = iCells[i];
  MonteCarlo::Object* CPtr = System.findObject(CA);
  if (!CPtr)
    throw ColErr::InContainerError<int>(CA,"Cell not found");
  CPtr->populate();
  HeadRule CHead=CPtr->getHeadRule();
  CHead.addIntersection(SN);

  CPtr->procHeadRule(CHead);
  CPtr->populate();
  CPtr->createSurfaceList();
   }
  return;
  }


void makeESS::addSurfaceToHRule(Simulation& System,
				std::vector<int> iCells, std::string SN)
  {
    ELog::RegMethod RegA("makeESS","addSurfaceToHRule");

  size_t iBSize = iCells.size();
  for (size_t i=0; i< iBSize; i++) {   
   // code from Simulation::splitObject
   int CA = iCells[i];
  MonteCarlo::Object* CPtr = System.findObject(CA);
  if (!CPtr)
    throw ColErr::InContainerError<int>(CA,"Cell not found");
  CPtr->populate();
  HeadRule CHead=CPtr->getHeadRule();
  CHead.addIntersection(SN);

  CPtr->procHeadRule(CHead);
  CPtr->populate();
  CPtr->createSurfaceList();
   }
  return;
  }


}   // NAMESPACE essSystem

