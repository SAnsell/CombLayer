/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/cellValueSet.cxx
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
#include "cellValueSet.h"

namespace flukaSystem
{
			
cellValueSet::cellValueSet(const std::string& KN,const std::string& ON) :
  keyName(KN),outName(ON)
  /*!
    Constructor
    \param KN :: Indentifier
    \param ON :: Output id for FLUKA
  */
{}


cellValueSet::cellValueSet(const cellValueSet& A) : 
  keyName(A.keyName),outName(A.outName)
  /*!
    Copy constructor
    \param A :: cellValueSet to copy
  */
{}

cellValueSet&
cellValueSet::operator=(const cellValueSet& A)
  /*!
    Assignment operator
    \param A :: cellValueSet to copy
    \return *this
  */
{
  if (this!=&A)
    {
      dataMap=A.dataMap;
    }
  return *this;
}


cellValueSet::~cellValueSet()
  /*!
    Destructor
  */
{}

  
void
cellValueSet::clearAll()
  /*!
    The big reset
  */
{
  dataMap.erase(dataMap.begin(),dataMap.end());  
  return;
}

bool
cellValueSet::cellSplit(const std::vector<int>& cellN,
			std::vector<std::tuple<int,int,double>>& initCell) const
  /*!
    Process ranges to find for value
    \param dataMap :: Data value to check
    \param initCell :: initialization range
    \return true if output required
   */
{
  typedef std::tuple<int,int,double> TITEM;
  initCell.clear();
  
  if (dataMap.empty() || cellN.empty()) return 0;
  
  size_t prev(0);
  double V;
  for(size_t i=0;i<cellN.size();i++)
    {
      const int CN=cellN[i];
      std::map<int,double>::const_iterator mc=dataMap.find(CN);
      if (mc==dataMap.end())
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
  
  return (initCell.empty()) ? 0 : 1;
}


void
cellValueSet::setValue(const int cN,const double V)
  /*!
    Set a value in the map
    \param cN :: Cell number   
    \param V :: value for cell
  */
{
  dataMap.emplace(cN,V);
  return;
}

void
cellValueSet::writeFLUKA(std::ostream& OX,
			 const std::vector<int>& cellN,
			 const std::string& ControlStr) const 
/*!
    Process is to write keyName ControlStr units 
    \param OX :: Output stream
    \param cellN :: vector of cells
    \param ControlStr units [%0/%1/%2] for cell range/Value
  */
{
  ELog::RegMethod RegA("cellValueSet","writeFLUKA");
  
  typedef std::tuple<int,int,double> TITEM;

  std::ostringstream cx;
  std::vector<TITEM> Bgroup;

  
  if (cellSplit(cellN,Bgroup))
    {
      const std::vector<std::string> Units=StrFunc::StrParts(ControlStr);
      std::vector<std::string> SArray(3);
      for(const TITEM& tc : Bgroup)
	{
	  SArray[0]=std::to_string(std::get<0>(tc));
	  SArray[1]=std::to_string(std::get<1>(tc));
	  SArray[2]=std::to_string(std::get<2>(tc));
	  cx.str("");
	  cx<<outName<<" ";
	  for(const std::string& UC : Units)
	    {
	      if (UC[0]=='%' && UC.size()==2)
		{
		  const size_t SA=static_cast<size_t>(UC[1]-'0');
		  cx<<SArray[SA];
		}
	      else
		cx<<UC;
	    }
	  StrFunc::writeFLUKA(cx.str(),OX);
	}
    }
  return;
}

  
} // NAMESPACE flukaSystem
      
   
