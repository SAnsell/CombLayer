/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/flukaPhysics.cxx
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
#include <tuple>


#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "MapRange.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

#include "particleConv.h"
#include "flukaPhysics.h"

namespace flukaSystem
{
			
flukaPhysics::flukaPhysics() 
  /*!
    Constructor
  */
{}


flukaPhysics::~flukaPhysics()
  /*!
    Destructor
  */
{}

  
void
flukaPhysics::clearAll()
  /*!
    The big reset
  */
{
  cellN.clear();
  allImp.erase(allImp.begin(),allImp.end());
  hadronImp.erase(hadronImp.begin(),hadronImp.end());
  electronImp.erase(electronImp.begin(),electronImp.end());
  lowImp.erase(lowImp.begin(),lowImp.end());
  
  return;
}

bool
flukaPhysics::cellSplit(const std::map<int,double>& DataMap,
			std::vector<std::tuple<int,int,double>>& initCell) const
  /*!
    Process ranges to find for value
    \param DataMap :: Data value to check
    \param initCell :: initialization range
   */
{
  typedef std::tuple<int,int,double> TITEM;
  if (DataMap.empty()) return 0;
  
  initCell.clear();
  size_t prev(0);
  double V;
  for(size_t i=0;i<cellN.size();i++)
    {
      const int CN=cellN[i];
      std::map<int,double>::const_iterator mc=DataMap.find(CN);
      if (mc==DataMap.end())
	{
	  if (prev)
	    {
	      initCell.push_back(TITEM(cellN[prev-1],cellN[i-1],V));
	      prev=0;
	    }
	}
      else
	{
	  if (prev && std::abs(V-mc->second)>Geometry::zeroTol)
	    {
	      initCell.push_back(TITEM(cellN[prev-1],cellN[i-1],V));
	      prev=i+1;
	      V=mc->second;
	    }
	  else if (!prev)
	    {
	      prev=i+1;
	      V=mc->second;
	    }
	}
    }
  if (prev)
    initCell.push_back(TITEM(cellN[prev-1],cellN.back(),V));      
	  
  return 1;
}

void
flukaPhysics::setCellNumbers(const std::vector<int>& cellInfo)
  /*!
    Process the list of the valid cells 
    over each importance group.
    \param cellInfo :: list of cells
  */

{
  ELog::RegMethod RegA("flukaPhysics","setCellNumbers");

  cellN=cellInfo;
  return;
}
  
void
flukaPhysics::setImp(const std::string& keyName,
		     const int cellN,
		     const double value)
  /*!
    Set the importance list
    \param keyName :: all/hadron/electron/low
    \param cellN :: Cell number
    \param value :: Value to use
  */
{
  if (keyName=="all")
    allImp.emplace(cellN,value);
  else if (keyName=="hadron")
    hadronImp.emplace(cellN,value);
  else if (keyName=="electron")
    electronImp.emplace(cellN,value);
  else if (keyName=="low")
    lowImp.emplace(cellN,value);
  return;
}



  
} // NAMESPACE flukaSystem
      
   
