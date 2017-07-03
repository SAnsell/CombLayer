/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/makeDelft.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <iterator>
#include <memory>
#include <array>
#include <boost/format.hpp>
#include <boost/multi_array.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Source.h"
#include "KCode.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "World.h"
#include "AttachSupport.h"

#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "BeamTube.h"
#include "SwimingPool.h"
#include "virtualMod.h"
#include "delftH2Moderator.h"
#include "SphereModerator.h"
#include "ConeModerator.h"
#include "FlatModerator.h"
#include "H2Vac.h"
#include "H2Groove.h"
#include "beamSlot.h"
#include "BeamInsert.h"
#include "BeSurround.h"
#include "BeCube.h"
#include "BeFullBlock.h"
#include "SpaceBlock.h"
#include "Rabbit.h"
#include "makeDelft.h"


namespace delftSystem
{

virtualMod*
makeDelft::createColdMod(const std::string& modType) 
  /*!
    Simple constructor for cold moderators
    \param modType :: Moderator type
    \return virtual pointer
  */
{
  ELog::RegMethod RegA("makeDelft","createColdMod");

  if (modType=="Sphere")
    return new SphereModerator("sphereH2");
  if (modType=="SphereLong")
    return new SphereModerator("sphereLong");
  if (modType=="DoubleMoon")
    return new SphereModerator("sphereH2");
  if (modType=="Moon")
    return new delftH2Moderator("delftH2");
  if (modType=="Tunnel" || modType=="Cone")
    return new ConeModerator("cone");
  if (modType=="Kaeri" || modType=="Flat")
    return new FlatModerator("flat");
  if (modType=="Void")
    return 0;
  if (modType=="help")
    {
      ELog::EM<<"Options "<<ELog::endDiag;
      ELog::EM<<"-- Sphere     :: Spherical moderator from FRM-1\n";
      ELog::EM<<"-- SphereLong :: Spherical moderator long extension\n";
      ELog::EM<<"-- DoubleMoon :: Modified twin curve modreator\n";
      ELog::EM<<"-- Moon       :: Single cylindrical cut out moderator\n";
      ELog::EM<<"-- Cone        :: Cone moderator"<<ELog::endDiag;
      ELog::EM<<"-- Flat        :: Simple flat cone"<<ELog::endDiag;
      ELog::EM<<"-- Void        :: No moderator"<<ELog::endDiag;
      return 0;
    }

  throw ColErr::InContainerError<std::string>
    (modType,"Moderator type unknown");
}

makeDelft::makeDelft() :
  GridPlate(new ReactorGrid("delftGrid")),
  Pool(new SwimingPool("delftPool")),
  FlightA(new BeamTube("delftFlightR2")),
  FlightB(new BeamTube("delftFlightR3")),
  FlightC(new BeamTube("delftFlightR1")),
  FlightD(new BeamTube("delftFlightL2")),
  FlightE(new BeamTube("delftFlightL1")),
  FlightF(new BeamTube("delftFlightL3")),
  CSurround(new H2Vac("delftH2Cont")),
  R2Insert(new BeamInsert("R2Insert"))
  /*!
    Constructor
    \param modType :: Cold moderator type
  */
{
  ELog::RegMethod RegA("makeDelft","Constructor");
    
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(GridPlate);
  OR.addObject(Pool);
  OR.addObject(FlightA);
  OR.addObject(FlightB);
  OR.addObject(FlightC);
  OR.addObject(FlightD);
  OR.addObject(FlightE);
  OR.addObject(FlightF);
  OR.addObject(CSurround);
  OR.addObject(R2Insert);
}
  
makeDelft::makeDelft(const makeDelft& A) :
  GridPlate(new ReactorGrid(*A.GridPlate)),
  Pool(new SwimingPool(*A.Pool)),
  FlightA(new BeamTube(*A.FlightA)),  // Centre object get self cut
  FlightB(new BeamTube(*A.FlightB)),
  FlightC(new BeamTube(*A.FlightC)),
  FlightD(new BeamTube(*A.FlightD)),
  FlightE(new BeamTube(*A.FlightE)),
  FlightF(new BeamTube(*A.FlightF)),
  ColdMod((A.ColdMod) ?
	  std::shared_ptr<virtualMod>(A.ColdMod->clone())
	  : A.ColdMod),
  CSurround(new H2Vac(*A.CSurround)),
  R2Insert(new BeamInsert(*A.R2Insert))
  /*!
    Copy constructor
    \param A :: makeDelft to copy
  */
{

  for(const std::shared_ptr<H2Groove>& HG : A.ColdGroove)
    ColdGroove.push_back(std::shared_ptr<H2Groove>(new H2Groove(*HG)));
}

makeDelft&
makeDelft::operator=(const makeDelft& A)
  /*!
    Assignment operator
    \param A :: makeDelft to copy
    \return *this
  */
{
  if (this!=&A)
    {
      *GridPlate= *A.GridPlate;
      *Pool=*A.Pool;
      *FlightA=*A.FlightA;
      *FlightB=*A.FlightB;
      *FlightC=*A.FlightC;
      *FlightD=*A.FlightD;
      *FlightE=*A.FlightE;
      *FlightF=*A.FlightF;
      *ColdMod=*A.ColdMod;
      ColdGroove.clear();
      for(const std::shared_ptr<H2Groove>& HG : A.ColdGroove)
	ColdGroove.push_back(std::shared_ptr<H2Groove>(new H2Groove(*HG)));
      *CSurround=*A.CSurround;
      *R2Insert=*A.R2Insert;
      *R2Be=*A.R2Be;
    }
  return *this;
}

makeDelft::~makeDelft()
  /*!
    Destructor
   */
{}

void 
makeDelft::variableObjects(const FuncDataBase& Control)
  /*!
    Process those object that need to be controlled by variables
    \param Control :: control object
  */
{
  ELog::RegMethod RegA("makeDelft","variableObjects");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const int NGroove=Control.EvalVar<int>("delftH2NGroove");
  for(int i=0;i<NGroove;i++)
    {
      ColdGroove.push_back
	(std::shared_ptr<H2Groove>
	 (new H2Groove("delftH2Groove",i+1)));
      OR.addObject
	(StrFunc::makeString(std::string("delftH2Groove"),i+1),
	 ColdGroove.back());
    }
  return;
}

void
makeDelft::makeBlocks(Simulation& System)
  /*!
    Create the extra blocks within the swiming pool
    \param System :: Simulation system
  */
{  
  ELog::RegMethod RegA("makeDelft","makeBlock");
  typedef std::shared_ptr<SpaceBlock> SPType;

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  SPType GB(new SpaceBlock("Box",1));
  GB->addInsertCell(Pool->getCells("Water"));
  GB->createAll(System,*GridPlate,1);
  
  if (GB->getActiveFlag()<0) return;
  if (GB->getActiveFlag()>0)
    {
      OR.addObject(GB);
      SBox.push_back(GB);
    }

  GB=SPType(new SpaceBlock("Box",2));
  GB->addInsertCell(Pool->getCells("Water"));
  GB->createAll(System,*GridPlate,2);  
  if (GB->getActiveFlag()>0)
    {
      OR.addObject(GB);
      SBox.push_back(GB);
    }

  return;
}

void
makeDelft::makeRabbit(Simulation& System)
  /*!
    Make the rabbits
    \param System :: System to build them in
  */
{
  ELog::RegMethod RegA("makeDelft","makeRabbit");

  typedef std::shared_ptr<Rabbit> RPType;  
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  int index(1);
  int flag(1);
  do
    {
      RPType RB(new Rabbit("Rabbit",index));
	
      flag=RB->createAll(System,*GridPlate);	
      if (flag) 
	{
	  OR.addObject(RB); 
	  RSet.push_back(RB);
	  attachSystem::addToInsertSurfCtrl(System,*Pool,*RB);
	}
      index++;
    } while (flag);  

  
  return;
}

void 
makeDelft::setSource(Simulation* SimPtr,
		     const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input system
  */
{
  // For output stream
  ELog::RegMethod RControl("makeDelft","build");

  
  if (IParam.flag("kcode"))
    {
      const size_t NItems=IParam.itemCnt("kcode",0);
      std::ostringstream cx;
      for(size_t i=0;i<NItems;i++)
	cx<<IParam.getValue<std::string>("kcode",i)<<" ";
      SDef::KCode& KCard=SimPtr->getPC().getKCodeCard();
      KCard.setLine(cx.str());

      if (IParam.flag("ksrcMat"))
	{
	  std::vector<Geometry::Vec3D> FissionVec=
	    GridPlate->fuelCentres();
	  ELog::EM<<"Kmat vector size :"
		  <<FissionVec.size()<<ELog::endDiag;
	  KCard.setKSRC(FissionVec);
	}
    }

  return;
}

void
makeDelft::buildCore(Simulation& System,
		     const mainSystem::inputParam& IParam)
  /*!
    Build the main core
    \param System :: Simualation system
    \param IParam :: Input parameters					       
   */
{
  ELog::RegMethod RegA("makeDelft","buildCore");

  const attachSystem::FixedComp& WC=World::masterOrigin();
  
  if (IParam.flag("fuelXML"))
    GridPlate->loadFuelXML(IParam.getValue<std::string>("fuelXML"));
  
  GridPlate->addInsertCell(Pool->getCells("Water"));
  GridPlate->createAll(System,WC);
  if (IParam.flag("FuelXML"))
    GridPlate->writeFuelXML(IParam.getValue<std::string>("FuelXML"));

}

void
makeDelft::buildFlight(Simulation& System,
		       const std::string& flightConfig)
  /*!
    Build the main core
    \param System :: Simualation system
    \param flightConfig :: fight configurations
   */
{
  ELog::RegMethod RegA("makeDelft","buildFlight");

 
 
  const attachSystem::FixedComp& WC=World::masterOrigin();

  if (flightConfig=="Single")
    {
      FlightA->addInsertCell(74123);
      FlightA->createAll(System,WC,WC.getX());
      return;
    }

  FlightA->addInsertCell(Pool->getCells("Water"));
  FlightA->addInsertCell(74123);
  FlightA->createAll(System,WC,WC.getX());

  FlightB->addInsertCell(Pool->getCells("Water"));
  FlightB->addInsertCell(74123);
  FlightB->createAll(System,WC,WC.getX());

  FlightC->addInsertCell(Pool->getCells("Water"));
  FlightC->addInsertCell(74123);
  FlightC->createAll(System,WC,WC.getX());

  FlightD->addInsertCell(Pool->getCells("Water"));
  FlightD->addInsertCell(74123);
  FlightD->createAll(System,WC,-WC.getX());

  FlightE->addInsertCell(Pool->getCells("Water"));
  FlightE->addInsertCell(74123);
  FlightE->createAll(System,WC,-WC.getX());

  FlightF->addInsertCell(Pool->getCells("Water"));
  FlightF->addInsertCell(74123);
  FlightF->createAll(System,WC,-WC.getX());
  return;
}
  
void
makeDelft::buildModerator(Simulation& System,
			  const std::string& modType,
			  const std::string& refExtra)
  /*!
    Build the main moderator in flightA
    \param System :: Simualation system
    \param modType :: Moderator type
    \param refExtra :: Extra reflector be piece type
   */
{
  ELog::RegMethod RegA("makeDeflt","buildModerator");

  const int voidCell(74123);
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  ColdMod=std::shared_ptr<virtualMod>(createColdMod(modType));
  OR.addObject(ColdMod);
  // First create variable objects [for moderator]:
  variableObjects(System.getDataBase());

  const int vacReq((modType=="Cone" || modType=="Sphere" ||
		    modType=="Flat" || modType=="SphereLong") ? 0 : 1);


  R2Insert->addInsertCell(FlightA->getInnerVoid());
  R2Insert->createAll(System,*FlightA,0);

  if (refExtra=="R2Surround")
    {
      BeSurround* BePtr=new BeSurround("BeRef2");

      BePtr->addInsertCell(Pool->getCells("Water"));
      if (Pool->getCells("Water").empty())
	BePtr->addInsertCell(voidCell);


      BePtr->createAll(System,*FlightA,0,
		       FlightB->getExclude()+FlightC->getExclude());
      R2Be=std::shared_ptr<attachSystem::FixedOffset>(BePtr);
      OR.addObject(R2Be);
    }
  
  if (ColdMod)
    {
      if (!vacReq)
	ColdMod->addInsertCell(FlightA->getInnerVoid());
      ColdMod->createAll(System,*FlightA);
      
      const int MB=ColdMod->getMainBody();
      if (MB)
	{
	  std::vector<std::shared_ptr<H2Groove> >::iterator vc;
	  int gprev(0);
	  for(vc=ColdGroove.begin();vc!=ColdGroove.end();vc++)
	    {
	      (*vc)->addInsertCell(MB);
	      if (gprev) 
		(*vc)->addInsertCell(gprev);
	      (*vc)->createAll(System,*ColdMod,*ColdMod);
	      gprev=(*vc)->getMainCell();
	    }      
	}
      if (vacReq)
	{
	  CSurround->addInsertCell(FlightA->getInnerVoid());
	  CSurround->createAll(System,*ColdMod,*ColdMod);
	}
      ColdMod->postCreateWork(System);
    }

  return;
}
  
void 
makeDelft::build(Simulation& System,
		 const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input paratmers					       
   */
{
  // For output stream
  ELog::RegMethod RControl("makeDelft","build");


  const attachSystem::FixedComp& WC=World::masterOrigin();

  const std::string buildType=IParam.getValue<std::string>("buildType");
  const std::string modType=IParam.getValue<std::string>("modType");
  const std::string refExtra=IParam.getValue<std::string>("refExtra");  

  
  if (buildType!="Single")
    {
      Pool->addInsertCell(74123);
      Pool->createAll(System,WC,0);
      buildFlight(System,buildType);
      
      makeBlocks(System);
      makeRabbit(System);
    }
  
  buildFlight(System,buildType);
  ELog::EM<<"Moderator == "<<modType<<ELog::endDiag;
  buildModerator(System,modType,refExtra);
  
  return;
}


}   // NAMESPACE delftSystem
