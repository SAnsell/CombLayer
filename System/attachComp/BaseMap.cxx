/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/BaseMap.cxx
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
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"

namespace attachSystem
{

BaseMap::BaseMap()
 /*!
    Constructor 
  */
{}

BaseMap::BaseMap(const BaseMap& A) : 
  Items(A.Items)
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
    }
  return *this;
}

std::string
BaseMap::getFCKeyName() const
  /*!
    Get the FixedComp keyname if this is a FixedComp
    \return FC name / "Not-FC"
   */
{
  const attachSystem::FixedComp* FCPtr=
    dynamic_cast<const attachSystem::FixedComp*>(this);
  return (FCPtr) ? FCPtr->getKeyName() : "Not-FC";
}

bool
BaseMap::hasItem(const std::string& Key) const
  /*!
    Find is a cell group exist 
    \param Key :: Keyname
    \return true if key exists
  */
{
  ELog::RegMethod RegA("BaseMap","hasItem(string)");
  return hasItem(Key,0);
}

bool
BaseMap::hasItem(const std::string& Key,const size_t index) const
  /*!
    Find is a cell group exist and is large enough
    \param Key :: Keyname
    \param index :: Item number
    \return true if key and index exists
  */
{
  ELog::RegMethod RegA("BaseMap","hasItem(string,int)");
  
  LCTYPE::const_iterator mc=Items.find(Key);
  if (mc==Items.end())
    return 0;

  return (!index || mc->second.size()>index) ? 1 : 0;
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
BaseMap::setItem(const std::string& Key,
                 const size_t Index,
		 const int CN)
  /*!
    Insert a cell into position Index.
    - does not allow gaps in the index list
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
        Items.insert(LCTYPE::value_type(Key,{CN}));
      else
	throw ColErr::InContainerError<std::string>
	  (Key,"Key not defined for index["+StrFunc::makeString(Index)+"]");
      return;
    }
  // Replace current object [NO IDENTICAL CHECK (yet)]
  if (mc->second.size()>Index)
    {
      mc->second[Index]=CN;
      return;
    }
  // ADD NEW CELL:
  if (mc->second.size()==Index)
    mc->second.push_back(CN);
  else
    throw ColErr::IndexError<size_t>
      (Index,0,"Key["+Key+"] index oversize");
  
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
  setItems(Key,Out);
  return; 
}

void
BaseMap::setItems(const std::string& Key,
                  const std::vector<int>& cVec)
  /*!
    Insert a set of cells
    \param Key :: Keyname
    \param cVec ::  Cell list
  */
{
  ELog::RegMethod RegA("BaseMap","setItems<Vector>");
  
  LCTYPE::iterator mc=Items.find(Key);
  
  if (mc==Items.end())
    Items.insert(LCTYPE::value_type(Key,cVec));
  else
    mc->second=cVec;
  
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
    Items.insert(LCTYPE::value_type(Key,{CN}));    
  else
    mc->second.push_back(CN);
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
  ELog::RegMethod RegA("BaseMap","addItems");

  if (inpVec.empty()) return;
  
  LCTYPE::iterator mc=Items.find(Key);
  if (mc==Items.end())
    Items.insert(LCTYPE::value_type(Key,inpVec));    
  else
    {
      std::copy(inpVec.begin(),inpVec.end(),
                std::back_inserter(mc->second));
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

  size_t index;
  const std::string::size_type pos=Key.rfind("#");
  if (pos!=std::string::npos && 
      StrFunc::convert(Key.substr(pos+1),index))
    return getItem(Key.substr(0,pos),index);
    
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

  if (Index>=mc->second.size())
    throw ColErr::IndexError<size_t>
      (Index,mc->second.size(),
       "Object:"+getFCKeyName()+" Key["+Key+"] index error");
  
  return mc->second[Index];
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
  ELog::RegMethod RegA("BaseMap","getItems(Key)");

  if (Key=="All" || Key=="all") return getItems(); 
  std::vector<int> Out;  
  LCTYPE::const_iterator mc=Items.find(Key);
  if (mc==Items.end())
    throw ColErr::InContainerError<std::string>(Key,"Key");
  return mc->second;
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
      for(const int& CN : mc.second)
        if (CN)
          Out.push_back(CN);
    }
  const std::set<int> elmDuplicate(Out.begin(),Out.end());
  Out.assign(elmDuplicate.begin(),elmDuplicate.end());
  
  return Out;
}

void
BaseMap::removeVecUnit(const std::string& kName,
                       const int cellN)
  /*!
    Remove a value from a vector list
    \param kName :: keyName
    \param cellN :: cell Number
  */
{
  ELog::RegMethod RegA("BaseMap","removeVecUnit");


  LCTYPE::iterator mc=Items.find(kName);

  if (mc==Items.end())
    throw ColErr::InContainerError<std::string>(kName,"Key not present");

  std::vector<int>& SRef(mc->second);
  std::vector<int>::iterator vc=std::find(SRef.begin(),SRef.end(),cellN);
  if (vc==SRef.end())
    throw ColErr::InContainerError<int>(cellN,"Cell not present in :"+kName);

  SRef.erase(vc);
  if (SRef.empty())
    Items.erase(mc);
  return;
}

std::string
BaseMap::findCell(const int cellN) const
  /*!
    Determine if cellN exists in set and return
    cell name.
    \param cellN :: Cell to test
    \return string if found 
    \todo use std::optional 
  */
{
  for(const LCTYPE::value_type& IUnit : Items)
    {
      const std::vector<int>& SRef(IUnit.second);
      if (std::find(SRef.begin(),SRef.end(),cellN) != SRef.end())
	return IUnit.first;
    }
  return "";
}

bool
BaseMap::hasCell(const int cellN) const
  /*!
    Determine if cellN exists in set
    \param cellN :: Cell to test
    \return true if found
  */
{
  ELog::RegMethod RegA("BaseMap","hasCell");
  
  for(const LCTYPE::value_type& IUnit : Items)
    {
      const std::vector<int>& SRef(IUnit.second);
      if (std::find(SRef.begin(),SRef.end(),cellN) != SRef.end())
	return 1;
    }
  return 0;
}


bool
BaseMap::hasCell(const std::string& kName,const int cellN) const
  /*!
    Determine if cellN exists in set
    \param kName :: unit to search
    \param cellN :: Cell to test
    \return true if found
  */
{
  ELog::RegMethod RegA("BaseMap","hasCell(string)");

  LCTYPE::const_iterator mc=Items.find(kName);

  if (mc==Items.end())
    throw ColErr::InContainerError<std::string>(kName,"Key not present");

  const std::vector<int>& SRef(mc->second);
  return (std::find(SRef.begin(),SRef.end(),cellN) != SRef.end());
}

  
const std::string&
BaseMap::getName(const int cellN) const
  /*!
    Given a cell number find the string(s) associated 
    with it 
    \param cellN :: Cell number to find
    \return empty string on failure to find / string name
   */
{
  ELog::RegMethod RegA("BaseMap","getName");
  static const std::string empty;
  // Quick check for cellN in base names

  for(const LCTYPE::value_type& IV : Items)
    {
      //      const std::string kUnit(IV.first); // care might delete it!!
      const std::vector<int>& SRef(IV.second);
      std::vector<int>::const_iterator vc=
        std::find(SRef.begin(),SRef.end(),cellN);

      if (vc!=SRef.end())
        return IV.first;
    }
  return empty;  // failed
}

std::string
BaseMap::removeItemNumber(const int cellN,
                          const size_t Index)
  /*!
    Given a cell number find the string(s) associated 
    with it and remove it.
    \param cellN :: Cell number to find
    \param Index :: Item index
    \return empty string on failure to find
   */
{
  ELog::RegMethod RegA("BaseMap","removeItemNumber");
  // Quick check for cellN in base names
  size_t indexCnt(0);
  for(LCTYPE::value_type& IV : Items)
    {
      const std::string kUnit(IV.first); // care might delete it!!
      std::vector<int>& SRef(IV.second);
      std::vector<int>::const_iterator vc=
        std::find(SRef.begin(),SRef.end(),cellN);

      if (vc!=SRef.end())
        {
          if (indexCnt==Index)
            {
              removeVecUnit(kUnit,cellN);
              return kUnit;   // return string
            }
          indexCnt++;
        }
    }
  return "";  // failed
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
  std::vector<int>& SRef(mc->second);
  if (SRef.size()<=Index)
    throw ColErr::InContainerError<size_t>(Index,"Index not present in :"+Key);

  const int outN(SRef[Index]);
  SRef.erase(SRef.begin()+static_cast<long int>(Index));
  if (SRef.empty())
    Items.erase(mc);
  return outN;
}


 
}  // NAMESPACE attachSystem
