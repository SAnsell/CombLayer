/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/ImportControl.cxx
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
#include "Qhull.h"
#include "LSwitchCard.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "PhysicsCards.h"
#include "SimProcess.h"
#include "Simulation.h"
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
setWImp(Simulation& System,const std::string& particleType)
  /*!
    control neutron importance for wwn/cell cards
    \param System :: Simulation
    \param particleType :: Particle type
  */
{    
  ELog::RegMethod RegA("ImportControl[F]","setWImp");
  System.getPC().setWImpFlag(particleType);
  return;
}

void
clearWImp(Simulation& System,const std::string& particleType)
  /*!
    control neutron importance for wwg cards
    \param System :: Simulation
    \param particleType :: Particle type
  */
{    
  ELog::RegMethod RegA("ImportControl[F]","clearWImp");
  System.getPC().clearWImpFlag(particleType);
  return;
}

void
removePhysImp(Simulation& System,const std::string& pType)
  /*!
    Removes particle importance if cell based system dominates
    \param System :: Simulation
    \param pType :: Particle type
  */
{
  ELog::RegMethod RegA("ImportControl[F]","removePhysImp");

  System.getPC().removePhysImp("imp",pType);
  return;
}

  
void
zeroImp(Simulation& System,const int initCell,
	const int cellRange)
  /*!
    Zero cell importances in a range
    \param System :: Simulation 
    \param initCell :: inital cell to searcha
    \param cellRange :: range of cells to zero 
						
   */
{
  ELog::RegMethod RegA("ImportControl[F]","zeroImp");

  physicsSystem::PhysicsCards& PC=System.getPC();  
  for(int cellNum=initCell;cellNum<initCell+cellRange;cellNum++)
    {
      MonteCarlo::Qhull* Obj=
	System.findQhull(cellNum);
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
simulationImp(Simulation& System,
	      const mainSystem::inputParam& IParam)
  /*!
    Control importances in individual cells
    \param System :: Simulation
    \param IParam :: input stream
   */
{
  ELog::RegMethod RegA("ImportControl[F]","simulationImport");

  if (!IParam.flag("voidUnMask") && !IParam.flag("mesh"))
    {
      System.findQhull(74123)->setImp(0);
      System.getPC().setCells("imp",74123,0);  // outer void to z	
    }
  
  if (IParam.flag("volCard"))
    {
      System.getPC().clearVolume();
    }
  
  // WEIGHTS:
  if (IParam.flag("imp") )
    {
      const std::string type=IParam.getValue<std::string>("imp",0);
      const std::string bStr=IParam.getValue<std::string>("imp",1);

      const ModelSupport::objectRegister& OR=
	ModelSupport::objectRegister::Instance();

      if (type=="void")
	{
	  int cellNum=OR.getCell("void")+1;	  
	  const int cellRange=OR.getRange("void");	      
	  zeroImp(System,cellNum,cellRange);
	}

      int bNum(0);
      if (type=="beamline" && StrFunc::convert(bStr,bNum))
	{
	  bNum--;
	  if (bNum<0 || bNum>17)
	    throw ColErr::RangeError<int>(bNum+1,1,18,"bNumber");

	  std::list<std::string> BlockName;
	  BlockName.push_back("torpedo");
	  BlockName.push_back("shutter");
	  BlockName.push_back("bulkInsert");

	  for(int i=0;i<18;i++)
	    {
	      if (i!=bNum)
		{
                  const std::string bNum=StrFunc::makeString(i);
                  for(const std::string& bName : BlockName)
		    {
		      // Zero everything in catorgoies:
		      int cellNum=
                        OR.getCell(bName+bNum)+1;
		      const int cellRange=OR.getRange(bName+bNum);
		      zeroImp(System,cellNum,cellRange);
		    }
		}
	    }
	}
    }
  return;
}

  
void
EnergyCellCut(Simulation& System,
              const mainSystem::inputParam& IParam)
  /*!
    Create the energy cell cutting system [ELPT] 
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
ExtField(Simulation& System,
	 const mainSystem::inputParam& IParam)
  /*!
    Control Ext card on the in individual cells
    \param System :: Simulation
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
FCL(Simulation& System,
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
      A.init(System);
      for(size_t index=0;index<nSet;index++)
        A.processUnit(System,IParam,index);
    }
  return;
}

void
IMP(Simulation& System,
    const mainSystem::inputParam& IParam)
  /*!
    Control IMP card on the in individual cells
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
SBias(Simulation& System,
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
DXT(Simulation& System,
    const mainSystem::inputParam& IParam)
  /*!
    Control DXT card(s) on the 
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("ImportControl[F]","DXT");

  // currently only first item / get all
  physicsSystem::DXTConstructor A;
  std::vector<std::string> StrItem;
  const size_t NGrp=IParam.setCnt("wDXT");
  for(size_t grpIndex=0;grpIndex<NGrp;grpIndex++)
    A.processUnit(System,IParam,grpIndex);

  const size_t NGrpD=IParam.setCnt("wDD");
  for(size_t grpIndex=0;grpIndex<NGrpD;grpIndex++)
    A.processDD(System,IParam,grpIndex);

  return;
}

  
void
PWT(Simulation& System,
    const mainSystem::inputParam& IParam)
  /*!
    Control PWT card(s) on the 
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("ImportControl","PWT");

  // currently only first item / get all
  std::vector<std::string> StrItem;
  const size_t NGrp=IParam.setCnt("wPWT");
  for(size_t grpIndex=0;grpIndex<NGrp;grpIndex++)
    {

      physicsSystem::PWTConstructor A;
      
      A.processUnit(System,IParam,grpIndex);
    }
  return;
}



  
}  // NAMESPACE weightSytem
