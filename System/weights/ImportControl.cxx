/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/ImportControl.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "MapRange.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "Object.h"
#include "LSwitchCard.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "PhysicsCards.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "ZoneUnit.h"
#include "ExtConstructor.h"
#include "PWTConstructor.h"
#include "FCLConstructor.h"
#include "IMPConstructor.h"
#include "DXTConstructor.h"
#include "ELPTConstructor.h"
#include "ImportControl.h"

namespace WeightSystem
{

void
setWImp(physicsSystem::PhysicsCards& PC,const std::string& particleType)
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
removePhysImp(physicsSystem::PhysicsCards& PC,const std::string& pType)
  /*!
    Removes particle importance if cell based system dominates
    \param PC :: Physics Cards
    \param pType :: Particle type
  */
{
  ELog::RegMethod RegA("ImportControl[F]","removePhysImp");

  PC.removePhysImp("imp",pType);
  return;
}

  
void
zeroImp(physicsSystem::PhysicsCards& PC,
	Simulation& System,const int initCell,
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
	  PC.setCells("imp",cellNum,0); 
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
    \param PC :: PhysicsCards
    \param System :: Simulation
    \param IParam :: input stream
   */
{
  ELog::RegMethod RegA("ImportControl[F]","simulationImport");

  physicsSystem::PhysicsCards& PC=System.getPC();
  if (!IParam.flag("voidUnMask") && !IParam.flag("mesh"))
    {      
      System.findObject(74123)->setImp(0);
      PC.setCells("imp",74123,0);  // outer void to z	
    }
  
  if (IParam.flag("volCard"))
    {
      PC.clearVolume();
    }  
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
ExtField(const objectGroups& OGrp,
	 physicsSystem::PhysicsCards& PC,
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
      A.processUnit(OGrp,PC,IParam,grpIndex);
    }
  return;
}

void
FCL(const objectGroups& OGrp,
    physicsSystem::PhysicsCards& PC,
    const mainSystem::inputParam& IParam)
  /*!
    Control FCL card on the in individual cells
    \param OGrp :: Object group
    \param PC :: Physics Cards
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
        A.processUnit(OGrp,PC,IParam,index);
    }
  return;
}

void
IMP(SimMCNP& System,
    const mainSystem::inputParam& IParam)
  /*!
    Control IMP card on the in individual cells
    \param OGrp :: Object group
    \param PC :: physics System
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("ImportControl","IMP");

  // currently only first item / get all

  const size_t nSet=IParam.setCnt("wIMP");
  if (nSet)
    {
      physicsSystem::IMPConstructor A;
      for(size_t index=0;index<nSet;index++)
        A.processUnit(System,IParam,index);
    }
  return;
}
  
void
SBias(const objectGroups& OGrp,
      physicsSystem::PhysicsCards& PC,
      const mainSystem::inputParam& IParam)
  /*!
    Control SBIAS card(s) on the 
    \param OGrp :: Object Groups
    \param PC :: Physics System
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
      A.processUnit(OGrp,PC,IParam,grpIndex);
    }
  return;
}

void
DXT(const objectGroups& OGrp,
    physicsSystem::PhysicsCards& PC,
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
    A.processUnit(OGrp,PC,IParam,grpIndex);

  const size_t NGrpD=IParam.setCnt("wDD");
  for(size_t grpIndex=0;grpIndex<NGrpD;grpIndex++)
    A.processDD(PC,IParam,grpIndex);

  return;
}

  
void
PWT(const objectGroups& OGrp,
    physicsSystem::PhysicsCards& PC,
    const mainSystem::inputParam& IParam)
  /*!
    Control PWT card(s) on the 
    \param OGrp :: Object group
    \param PC :: Physics System
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("ImportControl","PWT");

  // currently only first item / get all

  const size_t NGrp=IParam.setCnt("wPWT");
  for(size_t grpIndex=0;grpIndex<NGrp;grpIndex++)
    {

      physicsSystem::PWTConstructor A;
      A.processUnit(OGrp,PC,IParam,grpIndex);
    }
  return;
}



  
}  // NAMESPACE weightSytem
