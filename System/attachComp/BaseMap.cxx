/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/BaseMap.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseMap.h"

namespace attachSystem
{

BaseMap::BaseMap()
 /*!
    Constructor 
  */
{}

BaseMap::BaseMap(const BaseMap& A) : 
  Items(A.Items),SplitUnits(A.SplitUnits)
  /*!
    Copy constructor
    \param A :: BaseMap to copy
  */
{}

BaseMap&
BaseMap::operator=(const BaseMap& A)
  /*!
    Assignment operator
    \param A :: BaseMap to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Items=A.Items;
      SplitUnits=A.SplitUnits;
    }
  return *this;
}

void
BaseMap::setItem(const std::string& Key,const int CN)
  /*!
    Insert a cell 
    \param Key :: Keyname
    \param CN :: Item number
  */
{
  setItem(Key,0,CN);
  return; 
}

void
BaseMap::setItem(const std::string& Key,const size_t Index,
		 const int CN)
  /*!
    Insert a cell 
    \param Key :: Keyname
    \param Index :: Index number
    \param CN :: Item number
  */
{
  ELog::RegMethod RegA("BaseMap","setItem(s,i,i)");

  LCTYPE::iterator mc=Items.find(Key);
  if (mc==Items.end())
    {
      if (Index==0)
	Items.insert(LCTYPE::value_type(Key,CN));
      else
	throw ColErr::InContainerError<std::string>
	  (Key,"Key not defined for index > 0");

      return;
    }
  
  if (mc->second>=0)           // +1 case
    {
      if (Index==0)
	mc->second=CN;
      else if (Index==1)
	{
	  SplitUnits.push_back(std::vector<int>({mc->second,CN}));
	  mc->second = -static_cast<int>(SplitUnits.size());
	}
      else
	throw ColErr::IndexError<size_t>
	  (Index,0,"Key["+Key+"] index error");
      return;
    }

  // mc->second :: -ve
  const size_t SI(static_cast<size_t>(-mc->second)-1);
  const size_t SU(SplitUnits[SI].size());

  if (Index<SU)
    SplitUnits[SI][Index]=CN;
  else if (Index==SU)
    SplitUnits[SI].push_back(CN);
  else
    throw ColErr::IndexError<size_t>
      (Index,SU,"Key["+Key+"] index error");

  return; 
}

void
BaseMap::setItems(const std::string& Key,const int CNA,const int CNB)
  /*!
    Insert a set of cells
    \param Key :: Keyname
    \param CNA :: Item number [Start]
    \param CNB :: Item number [End]
  */
{
  ELog::RegMethod RegA("BaseMap","setItems");
  
  int mA(std::min(CNA,CNB));
  int mB(std::max(CNA,CNB));
  std::vector<int> Out;
  for(;mA<=mB;mA++)
    Out.push_back(mA);

  LCTYPE::iterator mc=Items.find(Key);
  if (mc==Items.end())
    {
      if (mA==mB)
	Items.insert(LCTYPE::value_type(Key,mA));
      else
	{
	  SplitUnits.push_back(Out);
	  Items.insert(LCTYPE::value_type
		       (Key,-static_cast<int>(SplitUnits.size())));
	}      
      return;
    }
  
  if (mc->second>=0)           // +1 case
    {
      Out.push_back(mc->second);
      SplitUnits.push_back(Out);
      mc->second = -static_cast<int>(SplitUnits.size());
      return;
    }

  // mc->second :: -ve
  const size_t SI(static_cast<size_t>(-mc->second)-1);
  std::move(Out.begin(),Out.end(),
	    std::back_inserter(SplitUnits[SI]));


  return; 
}

void
BaseMap::addItems(const std::string& Key,
		  const std::vector<int>& inpVec)
  /*!
    Insert a set of cells
    \param Key :: Keyname
    \param inpVec :: Input vector
  */
{
  ELog::RegMethod RegA("BaseMap","setItems");

  if (inpVec.empty()) return;
  
  LCTYPE::iterator mc=Items.find(Key);
  if (mc==Items.end())
    {

      if (inpVec.size()==1)
	Items.insert(LCTYPE::value_type(Key,inpVec.front()));
      else
	{
	  SplitUnits.push_back(inpVec);
	  Items.insert(LCTYPE::value_type
		       (Key,-static_cast<int>(SplitUnits.size())));
	}      
      return;
    }
      
  if (mc->second>=0)           // +1 case [one item]
    {
      SplitUnits.push_back(inpVec);
      SplitUnits.back().push_back(mc->second);
      mc->second = -static_cast<int>(SplitUnits.size());
      return;
    }
  
  // mc->second :: -ve
  const size_t SI(static_cast<size_t>(-mc->second)-1);
  std::move(inpVec.begin(),inpVec.end(),
	    std::back_inserter(SplitUnits[SI]));
  return; 
}

void
BaseMap::addItem(const std::string& Key,const int CN)
  /*!
    Insert a cell 
    \param Key :: Keyname
    \param CN :: Item number
  */
{
  ELog::RegMethod RegA("BaseMap","addItem");
  
  LCTYPE::iterator mc=Items.find(Key);
  if (mc==Items.end())
    setItem(Key,0,CN);
  else if (mc->second>=0)  // +1 case
    setItem(Key,1,CN);
  else
    {
      const size_t index=static_cast<size_t>((-mc->second-1));
      setItem(Key,SplitUnits[index].size(),CN);
    }
  return;
}
  
int
BaseMap::getItem(const std::string& Key) const
  /*!
    Get a cell number
    \param Key :: Keyname
    \return cell number
  */
{
  ELog::RegMethod RegA("BaseMap","getItem");
  return getItem(Key,0);
}

int
BaseMap::getItem(const std::string& Key,const size_t Index) const
  /*!
    Get a cell number
    \param Key :: Keyname
    \param Index :: Index number 
    \return cell number
  */
{
  ELog::RegMethod RegA("BaseMap","getItem(s,index)");

  LCTYPE::const_iterator mc=Items.find(Key);
  if (mc==Items.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");

  if (mc->second>=0)
    {
      if (Index!=0)
	throw ColErr::IndexError<size_t>(Index,0,"Key["+Key+"] index error");
      return mc->second;
    }
  // This can't fail:
  const size_t SI(static_cast<size_t>(-mc->second-1));
  const size_t SU(SplitUnits[SI].size());
  
  if (Index>=SU)
    throw ColErr::IndexError<size_t>(Index,SU,"Key["+Key+"] index error");
  return SplitUnits[SI][Index];
}

std::vector<std::string>
BaseMap::getNames() const
  /*!
    Get a list of the names in the cell
    Mainly debugging use
    \return vector of names
   */
{
  std::vector<std::string> Out;
  for(const LCTYPE::value_type lUnit : Items)
    Out.push_back(lUnit.first);
  return Out;
}
  
std::vector<int>
BaseMap::getItems(const std::string& Key) const
  /*!
    Returns a vector of cells: 
    Note a bit of care is needed over the case on a single value
    \param Key :: keyname to search
    \return vector
   */
{
  ELog::RegMethod RegA("BaseMap","getItems");

  std::vector<int> Out;
  
  LCTYPE::const_iterator mc=Items.find(Key);
  if (mc==Items.end())
    return Out;

  if (mc->second>=0)
    return std::vector<int>({mc->second});
  
  const size_t SU(static_cast<size_t>(-mc->second-1));


  for(const int& CN : SplitUnits[SU])
    if (CN)
      Out.push_back(CN);
  return Out;
}

std::vector<int>
BaseMap::getItems() const
  /*!
    Returns a vector of cells: 
    Note a bit of care is needed over the case on a single value
    \return vector
   */
{
  ELog::RegMethod RegA("BaseMap","getItems()");

  std::vector<int> Out;

  for(const LCTYPE::value_type& mc : Items)
    {
      if (mc.second>=0)
	Out.push_back(mc.second);
      else
	{
	  const size_t SU(static_cast<size_t>(-mc.second-1));
	  for(const int& CN : SplitUnits[SU])
	    if (CN)
	      Out.push_back(CN);
	}
    }

  return Out;
}



  

int
BaseMap::removeItem(const std::string& Key,
		    const size_t Index) 
 /*!
    Remove a cell number a cell
    \param Key :: KeyName for cell
    \param Index :: Item index
    \return cell number removed
  */
{
  ELog::RegMethod RegA("BaseMap","removeItem");

  LCTYPE::iterator mc=Items.find(Key);
  if (mc==Items.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");
  if (mc->second>=0)
    {
      if (Index!=0)
	throw ColErr::IndexError<size_t>
	  (Index,0,"Key["+Key+"] index error");

      const int outCN=mc->second;
      Items.erase(mc);
      return outCN;
    }
  // NOTE HERE WE ZERO and not delete because otherwise a seti
  // of linear calls to this function are junk.
  const size_t SI(static_cast<size_t>(-mc->second-1));
  const size_t SU(SplitUnits[SI].size());
  if (Index>=SU)
    throw ColErr::IndexError<size_t>
      (Index,SU,"Key["+Key+"] index error");

  const int outCN=SplitUnits[SI][SU];
  SplitUnits[SI][SU]=0;
  return outCN;
}

 
}  // NAMESPACE attachSystem
