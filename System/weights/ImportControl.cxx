/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/ImportControl.cxx
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
#include <memory>
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "MapRange.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "LSwitchCard.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "PhysicsCards.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "inputParam.h"
#include "ZoneUnit.h"
#include "ExtConstructor.h"
#include "PWTConstructor.h"
#include "FCLConstructor.h"
#include "DXTConstructor.h"
#include "ELPTConstructor.h"

namespace WeightSystem
{

void
setWImp(physicsSystem::PhysicsCards& PC,
	const std::string& particleType)
  /*!
    Control neutron importance for wwn/cell cards
    \param PC :: PhysicsCards
    \param particleType :: Particle type
  */
{    
  ELog::RegMethod RegA("ImportControl[F]","setWImp");
  PC.setWImpFlag(particleType);
  return;
}

void
clearWImp(physicsSystem::PhysicsCards& PC,const std::string& particleType)
  /*!
    control neutron importance for wwg cards
    \param PC :: Physics Cards
    \param particleType :: Particle type
  */
{    
  ELog::RegMethod RegA("ImportControl[F]","clearWImp");
  PC.clearWImpFlag(particleType);
  return;
}


  
void
zeroImp(Simulation& System,const int initCell,
	const int cellRange)
  /*!
    Zero cell importances in a range
    \param PC :: Physics Cards
    \param System :: Simulation 
    \param initCell :: inital cell to searcha
    \param cellRange :: range of cells to zero 
						
   */
{
  ELog::RegMethod RegA("ImportControl[F]","zeroImp");

  for(int cellNum=initCell;cellNum<initCell+cellRange;cellNum++)
    {
      MonteCarlo::Object* Obj=System.findObject(cellNum);
      if (Obj)
	{
	  Obj->setImp(0);
	  Obj->setMaterial(0);
	}
    }
  return;
}

void
simulationImp(SimMCNP& System,
	      const mainSystem::inputParam& IParam)
  /*!
    Control importances in individual cells
    \param System :: Simulation
    \param IParam :: input stream
   */
{
  ELog::RegMethod RegA("ImportControl[F]","simulationImp");

  if (!IParam.flag("voidUnMask") && !IParam.flag("mesh"))
    System.findObject(74123)->setImp(0);

  return;
}

  
void
EnergyCellCut(SimMCNP& System,
              const mainSystem::inputParam& IParam)
  /*!
    Create the energy cell cutting system [ELPT] 
    \param PC :: Physics Cards
    \param System :: Simulation 
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("ImportControl[F]","EnergyCellCut");

  // currently only first item / get all
  std::vector<std::string> StrItem;
  const size_t NGrp=IParam.setCnt("wECut");

  for(size_t grpIndex=0;grpIndex<NGrp;grpIndex++)
    {
      physicsSystem::ELPTConstructor A;
      A.processUnit(System,IParam,grpIndex);
    }
  return;
}

void
ExtField(SimMCNP& System,
	 const mainSystem::inputParam& IParam)
  /*!
    Control Ext card on the in individual cells
    \param OGrp :: ObjectGroups
    \param PC :: Physics Cards
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("ImportControl","ExtField");


  // currently only first item / get all

  const size_t NGrp=IParam.setCnt("wExt");

  for(size_t grpIndex=0;grpIndex<NGrp;grpIndex++)
    {
      physicsSystem::ExtConstructor A;
      A.processUnit(System,IParam,grpIndex);
    }
  return;
}

void
FCL(SimMCNP& System,
    const mainSystem::inputParam& IParam)
  /*!
    Control FCL card on the in individual cells
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("ImportControl","FCL");

  // currently only first item / get all
  const size_t nSet=IParam.setCnt("wFCL");
  if (nSet)
    {
      physicsSystem::FCLConstructor A;
      for(size_t index=0;index<nSet;index++)
        A.processUnit(System,IParam,index);
    }
  return;
}


void
SBias(SimMCNP& System,
      const mainSystem::inputParam& IParam)
  /*!
    Control SBIAS card(s) on the 
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("ImportControl","SBias");

  // currently only first item / get all
  std::vector<std::string> StrItem;
  const size_t NGrp=IParam.setCnt("sbias");

  for(size_t grpIndex=0;grpIndex<NGrp;grpIndex++)
    {
      physicsSystem::ExtConstructor A;
      A.processUnit(System,IParam,grpIndex);
    }
  return;
}

void
DXT(SimMCNP& System,
    const mainSystem::inputParam& IParam)
  /*!
    Control DXT card(s) on the 
    \param OGrp :: Object group
    \param PC :: Physics System
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("ImportControl[F]","DXT");
  
  // currently only first item / get all
  physicsSystem::DXTConstructor A;
  
  const size_t NGrp=IParam.setCnt("wDXT");
  for(size_t grpIndex=0;grpIndex<NGrp;grpIndex++)
    A.processUnit(System,IParam,grpIndex);

  const size_t NGrpD=IParam.setCnt("wDD");
  for(size_t grpIndex=0;grpIndex<NGrpD;grpIndex++)
    A.processDD(System,IParam,grpIndex);

  return;
}

  
void
PWT(SimMCNP& System,const mainSystem::inputParam& IParam)
  /*!
    Control PWT card(s) on the 
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("ImportControl","PWT");

  // currently only first item / get all

  const size_t NGrp=IParam.setCnt("wPWT");
  for(size_t grpIndex=0;grpIndex<NGrp;grpIndex++)
    {
      physicsSystem::PWTConstructor A;
      A.processUnit(System,IParam,grpIndex);
    }
  return;
}



  
}  // NAMESPACE weightSytem
