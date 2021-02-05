/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/FCLConstructor.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list>
#include <set>
#include <map> 
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "support.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "MapRange.h"
#include "ZoneUnit.h"
#include "inputParam.h"

#include "FCLConstructor.h"

namespace physicsSystem
{



  
void
FCLConstructor::processUnit(SimMCNP& System,
                            const mainSystem::inputParam& IParam,
                            const size_t Index)
  /*!
    Set individual FCL based on Iparam
    \param System :: simulation					
    \param IParam :: input stream
    \param index :: wFCL set card
  */
{
  ELog::RegMethod RegA("FCLConstructor","processUnit");

  physicsSystem::PhysicsCards& PC=System.getPC();
  const size_t NParam=IParam.itemCnt("wFCL",Index);
  if (NParam<1)
    throw ColErr::IndexError<size_t>(NParam,2,"Insufficient items wFCL");

  std::string FStr=IParam.getValue<std::string>("wFCL",Index,0);  
  if (FStr=="help" || FStr=="Help")
    {
      ELog::EM<<"-wFCL particle value [ cells ]"<<ELog::endBasic;
      return;
    }
  const std::string particle=FStr;
  if (NParam<2)
    throw ColErr::IndexError<size_t>(NParam,3,"Particle/value/cell not given");

  const double value=IParam.getValueError<double>
    ("wFCL",Index,0,"No fractional for wFCL");
  if (std::abs<double>(value)>1.0+Geometry::zeroTol)
    throw ColErr::RangeError<double>(value,-1.0,1.0,"Value for FCL");

  // Get all other values:
  std::vector<std::string> StrItem;
  for(size_t j=2;j<NParam;j++)
    StrItem.push_back
      (IParam.getValue<std::string>("wFCL",Index,j));

  double fclValue;
  ZoneUnit<double> ZUnits;
  
  if (!StrFunc::section(FStr,fclValue) ||
      !ZUnits.procZone(System,StrItem))
    throw ColErr::InvalidLine
      ("procZone ==> StrItems","-wFLC "+IParam.getFull("wFCL",Index),0);	

  ZUnits.addData(fclValue);
  ZUnits.sortZone();

  PhysImp& FCLimp=PC.getPhysImp("fcl",particle,0.0);
  FCLimp.updateCells(ZUnits);
  return;
}


  
} // Namespace physicsSystem
