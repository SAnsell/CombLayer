/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachComp/CellMap.cxx
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
#include "CellMap.h"

namespace attachSystem
{

CellMap::CellMap()
 /*!
    Constructor 
  */
{}

CellMap::CellMap(const CellMap& A) : 
  Cells(A.Cells)
  /*!
    Copy constructor
    \param A :: CellMap to copy
  */
{}

CellMap&
CellMap::operator=(const CellMap& A)
  /*!
    Assignment operator
    \param A :: CellMap to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Cells=A.Cells;
    }
  return *this;
}

void
CellMap::setCell(const std::string& Key,const int CN)
  /*!
    Insert a cell 
    \param Key :: Keyname
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("CellMap","setCell");
  
  if (Key.empty() || Cells.find(Key)!=Cells.end())
    throw ColErr::InContainerError<std::string>(Key,"Key already present");

  Cells.insert(LCTYPE::value_type(Key,CN));
  return; 
}

void
CellMap::setCell(const std::string& Key,const size_t Index,
		 const int CN)
  /*!
    Insert a cell 
    \param Key :: Keyname
    \param Index :: Index number
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("CellMap","setCell(s,i,i)");

  const std::string keyVal=Key+StrFunc::makeString(Index);
  if (Cells.find(keyVal)!=Cells.end())
    throw ColErr::InContainerError<std::string>(keyVal,"Key already present");

  Cells.insert(LCTYPE::value_type(keyVal,CN));
  return; 
}

  
int
CellMap::getCell(const std::string& Key) const
  /*!
    Get a cell number
    \param Key :: Keyname
    \return cell number
  */
{
  ELog::RegMethod RegA("CellMap","getCell");

  LCTYPE::const_iterator mc=Cells.find(Key);
  if (mc==Cells.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");

  return mc->second;
}

int
CellMap::getCell(const std::string& Key,const size_t Index) const
  /*!
    Get a cell number
    \param Key :: Keyname
    \param Index :: Index number
    \return cell number
  */
{
  ELog::RegMethod RegA("CellMap","getCell(s,index)");

  const std::string keyVal=Key+StrFunc::makeString(Index);
  LCTYPE::const_iterator mc=Cells.find(keyVal);
  if (mc==Cells.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");

  return mc->second;
}


}  // NAMESPACE attachSystem
