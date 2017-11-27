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
#include "SourceBase.h"
#include "sourceDataBase.h"
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
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "World.h"
#include "AttachSupport.h"

#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "BeamTube.h"
#include "BeamTubeJoiner.h"
#include "SwimingPool.h"
#include "virtualMod.h"
#include "delftH2Moderator.h"
#include "SphereModerator.h"
#include "ConeModerator.h"
#include "FlatModerator.h"
#include "H2Vac.h"
#include "PressureVessel.h"
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

makeDelft::makeDelft() :
  GridPlate(new ReactorGrid("delftGrid")),
  Pool(new SwimingPool("delftPool")),
  FlightA(new BeamTubeJoiner("delftFlightR2")),
  FlightB(new BeamTube("delftFlightR3")),
  FlightC(new BeamTube("delftFlightR1")),
  FlightD(new BeamTube("delftFlightL2")),
  FlightE(new BeamTube("delftFlightL1")),
  FlightF(new BeamTube("delftFlightL3")),
  ColdMod(new FlatModerator("flat")),
  ColdVac(new PressureVessel("delftVac")),
  ColdPress(new PressureVessel("delftPress")),
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
  OR.addObject(ColdMod);
  OR.addObject(ColdVac);
  OR.addObject(ColdPress);
  OR.addObject(R2Insert);
}
  
makeDelft::makeDelft(const makeDelft& A) :
  GridPlate(new ReactorGrid(*A.GridPlate)),
  Pool(new SwimingPool(*A.Pool)),
  FlightA(new BeamTubeJoiner(*A.FlightA)),  // Centre object get self cut
  FlightB(new BeamTube(*A.FlightB)),
  FlightC(new BeamTube(*A.FlightC)),
  FlightD(new BeamTube(*A.FlightD)),
  FlightE(new BeamTube(*A.FlightE)),
  FlightF(new BeamTube(*A.FlightF)),
  ColdMod(new FlatModerator(*A.ColdMod)),
  ColdVac(new PressureVessel(*A.ColdVac)),
  ColdPress(new PressureVessel(*A.ColdPress)),
  R2Insert(new BeamInsert(*A.R2Insert))
  /*!
    Copy constructor
    \param A :: makeDelft to copy
  */
{}

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
      *ColdVac=*A.ColdVac;
      *ColdPress=*A.ColdPress;
      *ColdMod=*A.ColdMod;

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
makeDelft::setSource(Simulation& System,
		     const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input system
  */
{
  // For output stream
  ELog::RegMethod RControl("makeDelft","build");
  SDef::sourceDataBase& SDB=SDef::sourceDataBase::Instance();
  
  if (IParam.flag("kcode"))
    {
      const size_t NItems=IParam.itemCnt("kcode",0);
      std::ostringstream cx;
      for(size_t i=0;i<NItems;i++)
	cx<<IParam.getValue<std::string>("kcode",i)<<" ";
      
      SDef::KCode KCard;
      KCard.setLine(cx.str());

      if (IParam.flag("ksrcMat"))
	{
	  const size_t fissileZaid=IParam.getDefValue<size_t>(0,"ksrcMat",0,0);
	  std::vector<int> fuelCells=
	    GridPlate->getFuelCells(System,fissileZaid);

	  std::vector<Geometry::Vec3D> FissionVec;
	  for(const int FC : fuelCells)
	    {
	      MonteCarlo::Qhull* OPtr=
		System.findQhull(FC);
	      if (!OPtr)
		throw ColErr::InContainerError<int>(FC,"Cell Not found");
	      if (OPtr->calcVertex())
		{
		  const Geometry::Vec3D& CPt(OPtr->getCofM());
		  if (OPtr->isValid(CPt))
		    FissionVec.push_back(CPt);
		}		
	    }
	  ELog::EM<<"Fission size == "<<FissionVec.size()<<ELog::endDiag;
	  KCard.setKSRC(FissionVec);
	  SDB.registerSource("kcode",KCard);
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
  GridPlate->createAll(System,WC,0);
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
    return;


  FlightB->addInsertCell(Pool->getCells("Water"));
  FlightB->addInsertCell(74123);
  FlightB->createAll(System,WC,0);

  FlightC->addInsertCell(Pool->getCells("Water"));
  FlightC->addInsertCell(74123);
  FlightC->createAll(System,WC,0);

  FlightD->addInsertCell(Pool->getCells("Water"));
  FlightD->addInsertCell(74123);
  FlightD->createAll(System,WC,0);

  FlightE->addInsertCell(Pool->getCells("Water"));
  FlightE->addInsertCell(74123);
  FlightE->createAll(System,WC,0);

  FlightF->addInsertCell(Pool->getCells("Water"));
  FlightF->addInsertCell(74123);
  FlightF->createAll(System,WC,0);
  return;
}
  
void
makeDelft::buildModerator(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex,
			  const std::string& modType,
			  const std::string& refExtra)
  /*!
    Build the main moderator in flightA
    \param System :: Simualation system
    \param FC :: Fixed point of side of reactro
    \param sideIndex :: link point 
    \param modType :: Name of moderator type
    \param refExtra :: Extra reflector be piece type
   */
{
  ELog::RegMethod RegA("makeDeflt","buildModerator");

  ELog::EM<<"Mod type == "<<modType<<ELog::endDiag;
  const int voidCell(74123);
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  ColdPress->addInsertCell(Pool->getCells("Water"));
  ColdPress->addInsertCell(74123);
  ColdPress->createAll(System,FC,sideIndex);

  ColdVac->addInsertCell(ColdPress->getCells("Void"));
  ColdVac->createAll(System,*ColdPress,0);
  if (modType!="Empty" && modType!="empty")
    {
      ColdMod->addInsertCell(ColdVac->getCells("Void"));
      ColdMod->createAll(System,*ColdVac,0);
    }

  // Joins onto the pressure vessel
  FlightA->addInsertCell(Pool->getCells("Water"));
  FlightA->addInsertCell(74123);
  FlightA->createAll(System,*ColdPress,2);

  R2Insert->addInsertCell(FlightA->getCells("Void"));
  R2Insert->createAll(System,*FlightA,0);
    

  // Built relative to main plate
  if (refExtra=="R2Surround")
    {
      BeSurround* BePtr=new BeSurround("R2Ref");
      BePtr->addInsertCell(Pool->getCells("Water"));
      if (Pool->getCells("Water").empty())
	BePtr->addInsertCell(voidCell);
      HeadRule CPCut(ColdPress->getFullRule(1));
      CPCut.addUnion(ColdPress->getFullRule(3));
      BePtr->createAll(System,FC,sideIndex,
		       FlightB->getExclude()+FlightC->getExclude()+
		       CPCut.display());
      R2Be=std::shared_ptr<attachSystem::FixedOffset>(BePtr);
      OR.addObject(R2Be);
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

  const attachSystem::FixedComp* FCPtr=&WC;
  long int sideIndex(0);
  if (buildType!="Single")
    {
      Pool->addInsertCell(74123);
      Pool->createAll(System,WC,0);
      
      makeBlocks(System);
      buildCore(System,IParam);
      makeRabbit(System);
      FCPtr=GridPlate.get();  
      sideIndex=2;
    }
  
  buildFlight(System,buildType);

  buildModerator(System,*FCPtr,sideIndex,modType,refExtra);
  
  return;
}


}   // NAMESPACE delftSystem
