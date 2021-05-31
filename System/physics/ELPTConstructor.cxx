/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/ELPTConstructor.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "MapRange.h"
#include "Triple.h"
#include "support.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "inputParam.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "NList.h"
#include "NRange.h"
#include "PhysicsCards.h"
#include "ZoneUnit.h"
#include "PhysImp.h"
#include "ELPTConstructor.h" 


namespace physicsSystem
{

ELPTConstructor::ELPTConstructor()
  /// Constructor
{}


void
ELPTConstructor::processUnit(SimMCNP& System,
			     const mainSystem::inputParam& IParam,
			     const size_t Index) 
  /*!
    Add ext component 
    \param System :: Simulation for cell map
    \param IParam :: Main input parameters
    \param Index :: index of the -wECut card
  */
{
  ELog::RegMethod RegA("ELPTConstructor","processUnit");
  double ECutValue(0.0);  /// ENERGY cut value;

  physicsSystem::PhysicsCards& PC=System.getPC();
  const size_t NParam=IParam.itemCnt("wECut",Index);
  if (NParam<1)
    throw ColErr::IndexError<size_t>(NParam,2,"Insufficient items wECut");

  std::string FStr=IParam.getValue<std::string>("wECut",Index,0);  
  if (FStr=="help" || FStr=="Help")
    {
      writeHelp(ELog::EM.Estream());
      ELog::EM<<ELog::endBasic;
      return;
    }
  const std::string particleStr=FStr;
  if (NParam<2)
    throw ColErr::IndexError<size_t>(NParam,3,"Particle/cell/value not given");
  FStr=IParam.getValue<std::string>("wECut",Index,1);  

  // Get all other values:
  std::vector<std::string> StrItem;
  for(size_t j=2;j<NParam;j++)
    StrItem.push_back
      (IParam.getValue<std::string>("wECut",Index,j));

  ZoneUnit<double> ZUnits;
  if (!StrFunc::section(FStr,ECutValue) ||
      !ZUnits.procZone(System,StrItem))
    throw ColErr::InvalidLine
      ("procZone ==> StrItems","-wECut "+IParam.getFull("wECut",Index),0);	
  ZUnits.addData(ECutValue);
  ZUnits.sortZone();

  PhysImp& ELPTimp=PC.getPhysImp("elpt",particleStr,0.0);  
  ELPTimp.updateCells(ZUnits);
  return;
}
  
void
ELPTConstructor::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param OX :: Output stream
  */
{

    OX<<"-wECut particle ECut  \n"
      "ZONE :: \n"
      "   All / object [objectName] / Cells {numbers}(s) / [cellNumber] \n"
      " min-particle energy in cell\n";
  return;
}


}  // NAMESPACE physicsSystem
