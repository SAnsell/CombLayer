/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/PointMap.cxx
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "PointMap.h"

namespace attachSystem
{

PointMap::PointMap() 
 /*!
    Constructor 
  */
{}

PointMap::PointMap(const PointMap& A) : 
  Items(A.Items)
  /*!
    Copy constructor
    \param A :: PointMap to copy
  */
{}
  
PointMap&
PointMap::operator=(const PointMap& A)
  /*!
    Assignment operator
    \param A :: PointMap to copy
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
PointMap::getFCKeyName() const
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
PointMap::hasPoint(const std::string& Key,
		   const size_t index) const
  /*!
    Find is a point name exist and is large for index
    \param Key :: Keyname
    \param index :: Item number
    \return true if key and index exists
  */
{
  ELog::RegMethod RegA("PointMap","hasPoint");
  
  LCTYPE::const_iterator mc=Items.find(Key);
  if (mc==Items.end())
    return 0;

  return (!index || mc->second.size()>index) ? 1 : 0;
}
  
void
PointMap::setPoint(const std::string& Key,
		  const Geometry::Vec3D& Pt)
  /*!
    Insert a point
    \param Key :: Keyname
    \param Pt :: Point number
  */
{
  setPoint(Key,0,Pt);
  return; 
}

void
PointMap::setPoint(const std::string& Key,
		   const size_t Index,
		   const Geometry::Vec3D& Pt)
  /*!
    Insert a point into position Index.
    - does not allow gaps in the index list
    \param Key :: Keyname
    \param Index :: Index number
    \param Pt :: Point number
  */
{
  ELog::RegMethod RegA("PointMap","setItem(s,i,Vec3D)");

  LCTYPE::iterator mc=Items.find(Key);
  if (mc==Items.end())
    {
      if (Index==0)
        Items.insert(LCTYPE::value_type(Key,{Pt}));
      else
	throw ColErr::InContainerError<std::string>
	  (Key,"Key not defined for index["+std::to_string(Index)+"]");
      return;
    }
  // Replace current object [NO IDENTICAL CHECK (yet)]
  if (mc->second.size()>Index)
    {
      mc->second[Index]=Pt;
      return;
    }
  // ADD NEW CELL:
  if (mc->second.size()==Index)
    mc->second.push_back(Pt);
  else
    throw ColErr::IndexError<size_t>
      (Index,0,"Key["+Key+"] index oversize");
  
  return; 
}

void
PointMap::addPoint(const std::string& Key,
		   const Geometry::Vec3D& Pt)
  /*!
    Insert a cell 
    \param Key :: Keyname
    \param Pt :: Point
  */
{
  LCTYPE::iterator mc=Items.find(Key);
  if (mc==Items.end())
    Items.insert(LCTYPE::value_type(Key,{Pt}));    
  else
    mc->second.push_back(Pt);
  return;
}

const Geometry::Vec3D&
PointMap::getPoint(const std::string& Key) const
  /*!
    Get a point
    Keyname uses first name or key#number
    \param Key :: Keyname
    \return Point
  */
{
  ELog::RegMethod RegA("BaseMap","getPoint");

  size_t index;
  const std::string::size_type pos=Key.rfind("#") ;
  if (pos!=std::string::npos && 
      StrFunc::convert(Key.substr(pos+1),index))
    return getPoint(Key.substr(0,pos),index);
    
  return getPoint(Key,0);
}

const Geometry::Vec3D&
PointMap::getPoint(const std::string& Key,const size_t Index) const
  /*!
    Get a cell number
    \param Key :: Keyname
    \param Index :: Index number 
    \return point
  */
{
  ELog::RegMethod RegA("PointMap","getPoint(s,index)");

  LCTYPE::const_iterator mc=Items.find(Key);
  if (mc==Items.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");

  if (Index>=mc->second.size())
    throw ColErr::IndexError<size_t>
      (Index,mc->second.size(),
       "Object:"+getFCKeyName()+" Key["+Key+"] index error");
  
  return mc->second[Index];
}
  
const Geometry::Vec3D&
PointMap::getLastPoint(const std::string& Key) const
  /*!
    Get the last cell number in vector
    \param Key :: Keyname
    \return point
  */
{
  ELog::RegMethod RegA("PointMap","getLastPoint(s,index)");

  LCTYPE::const_iterator mc=Items.find(Key);
  if (mc==Items.end() || mc->second.empty())
    throw ColErr::InContainerError<std::string>
      (Key,"Object:"+getFCKeyName()+":Key not present");
  
  return mc->second.back();
}
  

size_t
PointMap::getNPoints(const std::string& Key) const
  /*!
    Returns the number of of points: 
    Note a bit of care is needed over the case on a single value
    \param Key :: keyname to search
    \return size of points
   */
{
  ELog::RegMethod RegA("PointMap","getNPoints");

  if (Key=="All" || Key=="all")
    return getAllPoints().size();
  
  std::vector<int> Out;  
  LCTYPE::const_iterator mc=Items.find(Key);
  if (mc==Items.end())
    throw ColErr::InContainerError<std::string>(Key,"Key");
  return mc->second.size();
}

const std::vector<Geometry::Vec3D>&
PointMap::getPoints(const std::string& Key) const
  /*!
    Returns a vector of cells: 
    Note a bit of care is needed over the case on a single value
    and a copy constructor is called. Maybe this can be optimised out
    \param Key :: keyname to search
    \return vector of points
   */
{
  ELog::RegMethod RegA("PointMap","getPoints");

  LCTYPE::const_iterator mc=Items.find(Key);
  if (mc==Items.end())
    throw ColErr::InContainerError<std::string>(Key,"Key");
  return mc->second;
}

std::vector<Geometry::Vec3D>
PointMap::getAllPoints() const
  /*!
    Returns a vector of points: 
    Note a bit of care is needed over the case on a single value
    \return vector of points
   */
{
  ELog::RegMethod RegA("PointMap","getAllPoints");

  std::vector<Geometry::Vec3D> Out;

  for(const LCTYPE::value_type& mc : Items)
    {
      for(const Geometry::Vec3D& Pt : mc.second)
	Out.push_back(Pt);
    }
  // elimiate duplicates ??
  
  return Out;
}

Geometry::Vec3D
PointMap::removePoint(const std::string& Key,
		      const size_t Index) 
 /*!
    Remove a cell number a cell
    \param Key :: KeyName for cell
    \param Index :: Item index
    \return point removed
  */
{
  ELog::RegMethod RegA("PointMap","removePoint");

  LCTYPE::iterator mc=Items.find(Key);
  if (mc==Items.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");

  std::vector<Geometry::Vec3D>& SRef(mc->second);
  if (SRef.size()<=Index)
    throw ColErr::InContainerError<size_t>(Index,"Index not present in :"+Key);

  const Geometry::Vec3D outN(SRef[Index]);
  SRef.erase(SRef.begin()+static_cast<long int>(Index));
  if (SRef.empty()) Items.erase(mc);
  
  return outN;
}

Geometry::Vec3D
PointMap::getSignedPoint(const std::string& Key,
			 const size_t Index) const
  /*!
    Get the rule based on a point (normally for a direction)
    \param Key :: Keyname
    \param Index :: Offset number
    \return Signed Point 
   */
{
  ELog::RegMethod RegA("PointMap","getSignedPoint");

  if (Key.empty()) return Geometry::Vec3D();

  return (Key[0]=='-' || Key[0]=='#' || 
	  Key[0]=='*' || Key[0]=='%') ?
    -getPoint(Key.substr(1),Index) : getPoint(Key,Index);
}



  
  
}  // NAMESPACE attachSystem
