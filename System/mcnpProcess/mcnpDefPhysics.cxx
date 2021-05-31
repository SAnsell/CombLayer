/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   mcnpProcess/mcnpDefPhysics.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "PhysCard.h"
#include "PStandard.h"
#include "PSimple.h"
#include "ModeCard.h"
#include "LSwitchCard.h"
#include "PhysicsCards.h"

namespace mcnpSystem
{


void
setPhysicsModel(SimMCNP& System,
		const std::string& PModel)
  /*!
    Set the physics model based on the input parameter set
    \param System :: Simulation for physics system
    \param PModel :: Physics model to choose 
  */
{
  ELog::RegMethod RegA("DefPhysics[F]","setPhysicsModel");

  physicsSystem::LSwitchCard& lea=System.getPC().getLEA();
// Goran

  ELog::EM<<"Physics Model == "<<PModel<<ELog::endBasic;

  if (PModel=="CEM03")
    {
      lea.setValues("lca","2 1 1 0023 1 1 0 1 1 0");  // CEM
      lea.setValues("lea","1 4 1 0 1 0 0 1");
    }
  else if (PModel=="IA")
    {
      lea.setValues("lca","2 1 0 0023 1 1 2 1 2 0");  // INCL4 - ABLA
      lea.setValues("lea","1 4 1 0 1 0 2 1");
    }
  else if (PModel=="BD")
    {
      lea.setValues("lca","2 1 1 0023 1 1 0 1 0 0");  // Bertini -
                                                      // DrAnnesner
      lea.setValues("lea","1 4 1 0 1 0 0 1");
    }
  else if (PModel=="BA")
    {
      lea.setValues("lca","2 1 1 0023 1 1 2 1 0 0");  // Bertini - ABLA
      lea.setValues("lea","1 4 1 0 1 0 2 1");
    }
  else
    {
      ELog::EM<<"physModel :\n"
	"CEM03 :: CEM03 model \n"
	"IA :: INCL4 - ABLA model \n"
	"BD :: Bertini - Dresner model \n"
	"BA :: Bertini - ABLA model"<<ELog::endBasic;
      throw ColErr::ExitAbort("No model");
    }

  return;
}

void 
setNeutronPhysics(physicsSystem::PhysicsCards& PC,
		  const FuncDataBase&,
		  const double maxEnergy)
  /*!
    Set the neutron Physics for MCNP run
    \param PC :: Physcis cards
    \param Control :: Database of variables
    \param maxEnergy :: max energy cut
  */
{
  ELog::RegMethod RegA("DefPhysics","setNeutronPhysics");

  
  const std::string EMax=std::to_string(maxEnergy);

  PC.setMode("n");
  PC.setPrintNum("10 20 50 110 120");
  
  physicsSystem::PStandard* NCut=
    PC.addPhysCard<physicsSystem::PStandard>("cut","n");
  NCut->setValues(4,1.0e+8,0.0,0.4,-0.1);
  // Process physics
  physicsSystem::PStandard* pn=
    PC.addPhysCard<physicsSystem::PStandard>("phys","n");
  pn->setValues(EMax+" 0.0 j j j");
  
  return; 
}


void 
setReactorPhysics(physicsSystem::PhysicsCards& PC,
		  const FuncDataBase& Control,
		  const mainSystem::inputParam& IParam)
  /*!
    Set the neutron Physics for MCNP run on a reactor
    \param PC :: Physcis cards
    \param Control :: Databae of variables
  */
{
  ELog::RegMethod RegA("DefPhysics","setReactorPhysics");
  
  std::string PList("");
  const double maxEnergy=Control.EvalDefVar<double>("sdefEnergy",20.0);
  
  const double elcEnergy=IParam.getValue<double>("electron");
  const double phtEnergy=IParam.getValue<double>("photon");
  const double phtModel=IParam.getValue<double>("photonModel");


  const std::string elcAdd((elcEnergy>0 ? " e" : ""));
  ELog::EM<<"ECL == "<<elcAdd<<ELog::endDiag;
  PC.setMode("n p "+PList+elcAdd);
  PC.setPrintNum("10 110");
  
  physicsSystem::PStandard* NCut=
    PC.addPhysCard<physicsSystem::PStandard>("cut","n");
  NCut->setValues(4,1.0e+8,0.0,0.4,-0.1);
  
  physicsSystem::PStandard* allCut=
     PC.addPhysCard<physicsSystem::PStandard>("cut",PList);
  allCut->setValues(2,1e+8,0.0);
  physicsSystem::PStandard* photonCut=
     PC.addPhysCard<physicsSystem::PStandard>("cut","p");
  photonCut->setValues(2,1e+8,phtEnergy);

  if (elcEnergy>=0.0)
    {
      physicsSystem::PStandard* elcCut=
	PC.addPhysCard<physicsSystem::PStandard>("cut","e");
      elcCut->setValues(2,1e+8,elcEnergy);
    }


  const std::string EMax=std::to_string(maxEnergy);
  const std::string PHMax=std::to_string(phtEnergy);
  
  physicsSystem::PStandard* pn=
    PC.addPhysCard<physicsSystem::PStandard>("phys","n");
  pn->setValues(EMax+" 0.0 j j j");
  
  physicsSystem::PStandard* pp=
	PC.addPhysCard<physicsSystem::PStandard>("phys","p");
  if (elcEnergy>=0.0)
    pp->setValues(PHMax+" j j 1");
  else
    pp->setValues(PHMax);

  physicsSystem::PStandard* pa=
    PC.addPhysCard<physicsSystem::PStandard>("phys","/ d t s a "+elcAdd);
  pa->setValues(EMax);
  
  physicsSystem::PStandard* ph=
    PC.addPhysCard<physicsSystem::PStandard>("phys","h");
  ph->setValues(EMax);

  if (elcEnergy>0.0)
    {
      physicsSystem::PStandard* pe=
	PC.addPhysCard<physicsSystem::PStandard>("phys","e");
      pe->setValues(std::to_string(elcEnergy));
    }

  return; 
}

void
setGenericPhysics(SimMCNP& System,
		  const std::string& PModel)
  /*!
    Set general default imp/mode
    \param System :: Simulation to use
    \param PModel :: Model
   */
{
  ELog::RegMethod RegA("DefPhysics[F]","setGenericPhysics");
  
  physicsSystem::PhysicsCards& PC=System.getPC();

  PC.setMode("n");
  setPhysicsModel(System,PModel);
  //  const std::vector<int> cellVec=System.getCellVector();

  System.setImp(1,0);

  return;
}

void 
setDefaultPhysics(SimMCNP& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Set the default Physics
    \param System :: Simulation
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("DefPhysics[F]","setDefaultPhysics");

  // LCA ielas ipreq iexisa ichoic jcoul nexite npidk noact icem ilaq 
  // LEA ipht icc nobalc nobale ifbrk ilvden ievap nofis

  const FuncDataBase& Control=System.getDataBase();
  physicsSystem::PhysicsCards& PC=System.getPC();
  
  const std::string PModel=IParam.getValue<std::string>("physModel");
  const double maxEnergy=IParam.getDefValue<double>
    (3000.0,"maxEnergy");

  setGenericPhysics(System,PModel);
  
  PC.setNPS(static_cast<size_t>(IParam.getValue<double>("nps")));
  PC.setRND(IParam.getValue<long int>("random"));	
  PC.setVoidCard(IParam.flag("void"));
  // Default:   10 20 40 50 110 120"
  PC.setPrintNum(IParam.getValue<std::string>("printTable"));

  // If Reactor stuff set and void
  if (IParam.hasKey("kcode") && IParam.itemCnt("kcode"))
    {
      setReactorPhysics(PC,Control,IParam);
      return;
    }

  if (IParam.hasKey("neutronOnly"))
    {
      setNeutronPhysics(PC,Control,maxEnergy);
      return;
    }
  
  std::string PList=
    IParam.getDefValue<std::string>("h / d t s a z / * k ?","mode",0);
  if (PList=="empty" || PList=="Empty")
    {
      ELog::EM<<"WARNING:: plist empty"<<ELog::endWarn; 
      PC.addPhysCard<physicsSystem::PSimple>
	(std::string("mphys"),std::string(""));
      PList=" ";
    }
  
  const double cutUp=IParam.getValue<double>("cutWeight",0);  // [1keV
  const double cutMin=IParam.getValue<double>("cutWeight",1);  // [1keV def]
  const double cutTime=IParam.getDefValue<double>(1e8,"cutTime",0); 
  
  const double elcEnergy=IParam.getValue<double>("electron");
  const double phtEnergy=IParam.getValue<double>("photon");  // [1keV def]
  const double phtModel=IParam.getValue<double>("photonModel");
  const std::string elcAdd((elcEnergy>0 ? " e" : ""));

  if (std::abs(cutUp)<=std::abs(cutMin))
    throw ColErr::NumericalAbort
      ("CutUp<=cutMin: "+std::to_string(cutUp)+
       "<="+std::to_string(cutMin));
  
  PC.setMode("n p "+PList+elcAdd);
  //  PC.setCellNumbers(cellImp);
  //  PC.setCells("imp",1,0);            // Set a zero cell
  
  physicsSystem::PStandard* NCut=
    PC.addPhysCard<physicsSystem::PStandard>("cut","n");
  NCut->setValues(4,cutTime,0.0,0.4,-0.1);
  
  physicsSystem::PStandard* allCut=
     PC.addPhysCard<physicsSystem::PStandard>("cut",PList);
  allCut->setValues(4,cutTime,0.1,cutUp,cutMin);
  physicsSystem::PStandard* photonCut=
     PC.addPhysCard<physicsSystem::PStandard>("cut","p");
  photonCut->setValues(4,cutTime,phtEnergy,cutUp,cutMin);

  if (elcEnergy>=0.0)
    {
      ELog::EM<<"ELC == "<<elcEnergy<<ELog::endDiag;
      physicsSystem::PStandard* elcCut=
	PC.addPhysCard<physicsSystem::PStandard>("cut","e");
      elcCut->setValues(2,1e+8,elcEnergy);
    }
  
  const std::string EMax=std::to_string(maxEnergy);
  const std::string PHMax=std::to_string(phtModel);
  // Process physics
  physicsSystem::PStandard* pn=
	PC.addPhysCard<physicsSystem::PStandard>("phys","n");
  pn->setValues(EMax+" 0.0 j j j");

  physicsSystem::PStandard* pp=
    PC.addPhysCard<physicsSystem::PStandard>("phys","p");
  if (elcEnergy>=0.0)
    pp->setValues(PHMax+" j j -1");  // analog photonuclear
  else
    pp->setValues(PHMax);

  
  if (!PList.empty())
    {
      const std::string::size_type hpos=PList.find("h");
      if (PList.find("h")!=std::string::npos)
        {
          physicsSystem::PStandard* ph=
            PC.addPhysCard<physicsSystem::PStandard>("phys","h");
          ph->setValues(EMax);
          PList.erase(hpos,1);
        }
      physicsSystem::PStandard* pa=
        PC.addPhysCard<physicsSystem::PStandard>("phys",PList);
      pa->setValues(EMax);

      physicsSystem::PStandard* pe=
	PC.addPhysCard<physicsSystem::PStandard>("phys","e");
      pe->setValues(1,maxEnergy);
    }
  return; 
}



} // NAMESPACE ModelSupport
