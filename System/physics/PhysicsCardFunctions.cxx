/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/PhysicsCardFunctions.cxx
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
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "particleConv.h"
#include "PhysicsCardFunctions.h"

namespace physicsSystem
{

std::string
getMCNPparticleList(const std::set<int>& pSet)
  /*!
    Write the particle list out as :n,p,e etc 
    \param pSet :: List of mcpl numbers
   */
{
  ELog::RegMethod RegA("PhysicsCardFunction[F]","getMCNPparticleList(int)");

  std::set<int> excludeSet;

  return getMCNPparticleList(pSet,excludeSet);
}

std::string
getMCNPparticleList(const std::set<int>& pSet,
		    const std::set<std::string>& excludeNames)
  /*!
    Write the particle list out as :n,p,e etc 
    \param pSet :: List of mcpl numbers
    \param excludeNames :: List of exluded named particles
   */
{
  ELog::RegMethod RegA("PhysicsCardFunction[F]",
		       "getMCNPparticleList(int,string)");

  const particleConv& pConv=particleConv::Instance();
  std::set<int> excludeSet;
  for(const std::string& eP : excludeNames)
    excludeSet.emplace(pConv.mcplITYP(eP));

  return getMCNPparticleList(pSet,excludeSet);
}
  
  
std::string
getMCNPparticleList(const std::set<int>& pSet,
		    const std::set<int>& excludeSet)
  /*!
    Write the particle list out as :n,p,e etc 
    \param pSet :: List of mcpl numbers
    \param excludeSet :: List of exluded  particles
   */
{
  ELog::RegMethod RegA("PhysicsCardFunction",
		       "getMCNPparticleList(int,int)");

  if (pSet.empty()) return " ";
  const particleConv& pConv=particleConv::Instance();
  std::ostringstream cx;
  char separator(':');
  for(const int pIndex : pSet)
    {
      if (excludeSet.find(pIndex)==excludeSet.end())
	{
	  const std::string mcnpPart=
	    pConv.mcplToMCNP(pIndex);
	  cx<<separator<<mcnpPart;
	  separator=',';
	}
    }
  cx<<" ";
  return cx.str();
}
  
} // NAMESPACE physicsSystem
      
   
