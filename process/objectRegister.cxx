/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/objectRegister.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <set>
#include <string>
#include <algorithm>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "objectRegister.h"

namespace ModelSupport
{

objectRegister::objectRegister() : 
  cellNumber(1000000)
  /*!
    Constructor
  */
{}

objectRegister::~objectRegister() 
  /*!
    Destructor
  */
{}

void
objectRegister::reset()
  /*!
    Delete all the references to the shared_ptr register
  */
{
  Components.erase(Components.begin(),Components.end());
  return;
}
objectRegister& 
objectRegister::Instance() 
  /*!
    Effective this object			
    \return objectRegister object
  */
{
  static objectRegister A;
  return A;
}

int
objectRegister::getCell(const std::string& Name,const int Index) const
  /*!
    Get the start cell of an object
    \param Name :: Name of the object to get
    \param Index :: Offset number
    \return Cell number
  */
{
  MTYPE::const_iterator mc;
  if (Index>=0)
    {
      std::ostringstream cx;
      cx<<Name<<Index;
      mc=regionMap.find(cx.str());
    }
  else
    mc=regionMap.find(Name);
  if (mc!=regionMap.end())
    return mc->second.first;
  return 0;
}

int
objectRegister::getRange(const std::string& Name,const int Index) const
  /*!
    Get the range of an object
    \param Name :: Name of the object to get
    \param Index :: Offset number
    \return Range
   */
{
  MTYPE::const_iterator mc;
  if (Index>=0)
    {
      std::ostringstream cx;
      cx<<Name<<Index;
      mc=regionMap.find(cx.str());
    }
  else
    mc=regionMap.find(Name);
  if (mc!=regionMap.end())
    return mc->second.second;
  return 0;
}

std::string
objectRegister::inRange(const int Index) const
  /*!
    Get the range of an object
    \param Name :: Name of the object to get
    \param Index :: Offset number
    \return string
   */
{
  static std::string prev;
  
  MTYPE::const_iterator mc;
  mc=regionMap.find(prev);

  if (mc!=regionMap.end() && 
      Index>=mc->second.first && 
      Index<=mc->second.first+mc->second.second)
    return mc->first;
    
  for(mc=regionMap.begin();mc!=regionMap.end();mc++)
    {
      const std::pair<int,int>& IP=mc->second;
      if (Index>=IP.first && Index<=IP.first+IP.second)
	{
	  prev=mc->first;
	  return mc->first;
	}
    }
  return std::string("");
}

int
objectRegister::cell(const std::string& Name,const int Index,const int size)
  /*!
    Add a component and get a new cell number 
    \param Name :: Name of the unit
    \param Index :: Index on number [-ve not used]
    \param size :: Size of unit to register
    \return the start number of the cellvalue
  */
{
  ELog::RegMethod RegA("objectRegister","cell");

  std::ostringstream cx;
  cx<<Name;
  if (Index>=0)
    cx<<Index;

  MTYPE::const_iterator mc=regionMap.find(cx.str());  
  if (mc!=regionMap.end())
    {
      if (mc->second.second<size)
	ELog::EM<<"Insufficient space reserved for "<<cx.str()<<ELog::endErr;
      return mc->second.first;
    }
  regionMap.insert(MTYPE::value_type(cx.str(),
				     std::pair<int,int>(cellNumber,size)));
  cellNumber+=size;
  return cellNumber-size;
}

void
objectRegister::addObject(const CTYPE& Ptr)
  /*! 
    Register a shared_ptr of an object. 
    Requirement that 
    - (a) The object already exists as a range
    - (b) No repeat object
    All failures result in an exception.
    \param Ptr :: FixedComp object [shared_ptr]
  */
{
  ELog::RegMethod RegA("objectRegister","addObject(Obj)");
  if (Ptr)
    addObject(Ptr->getKeyName(),Ptr);
   else
    throw ColErr::EmptyValue<void>("Ptr Shared_ptr");
  return;
}

void
objectRegister::addObject(const std::string& Name,
			 const CTYPE& Ptr)
  /*!
    Register a shared_ptr of an object. 
    Requirement that 
    - (a) The object already exists as a range
    - (b) No repeat object
    All failures result in an exception.
    \param Name :: Name of the object						
    \param Ptr :: Shared_ptr
  */
{
  ELog::RegMethod RegA("objectRegister","addObject");
  // First check that we have it in Register:
  if (regionMap.find(Name)==regionMap.end())
    throw ColErr::InContainerError<std::string>(Name,"regionMap empty");
  // Does it exist:
  if (Components.find(Name)!=Components.end())
    throw ColErr::InContainerError<std::string>(Name,"Exisiting object");
  Components.insert(cMapTYPE::value_type(Name,Ptr));
  return;
}

bool
objectRegister::hasObject(const std::string& Name) const
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return true (object exists
  */
{
  ELog::RegMethod RegA("objectRegister","hasObject");

  cMapTYPE::const_iterator mc=Components.find(Name);
  return (mc!=Components.end()) ? 1 : 0;
}

template<typename T>
const T*
objectRegister::getObject(const std::string& Name) const
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectRegister","getObject(const)");

  cMapTYPE::const_iterator mc=Components.find(Name);
  return (mc!=Components.end()) 
    ? dynamic_cast<const T*>(mc->second.get()) : 0;
}

template<typename T>
T*
objectRegister::getObject(const std::string& Name) 
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectRegister","getObject");

  cMapTYPE::iterator mc=Components.find(Name);
  return (mc!=Components.end()) 
    ? dynamic_cast<T*>(mc->second.get()) : 0;
}


template<>
const attachSystem::ContainedComp* 
objectRegister::getObject(const std::string& Name) const
  /*!
    Special for containedComp as it could be a componsite
    of containedGroup
    \param Name :: Name
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectRegister","getObject(containedComp)");
  const std::string::size_type pos=Name.find(":");
  if (pos==std::string::npos || !pos || pos==Name.size()-1)
    {
      cMapTYPE::const_iterator mc=Components.find(Name);
      return (mc!=Components.end()) ?
	dynamic_cast<const attachSystem::ContainedComp*>(mc->second.get()) 
	: 0;
    }
  const std::string PreItem=Name.substr(0,pos);
  const std::string PostItem=Name.substr(pos);
  ELog::EM<<"Sub item == "<<PreItem<<ELog::endDebug;
  ELog::EM<<"Post item == "<<PostItem<<ELog::endDebug;
  return 0;
}

void
objectRegister::write(const std::string& OFile) const
  /*!
    Write out to a file
    \param OFile :: output file
  */
{
  ELog::RegMethod RegA("objectRegister","write");
  if (!OFile.empty())
    {
      const char* FStatus[]={"void","fixed"};
      std::ofstream OX(OFile.c_str());

      boost::format FMT("%s%|30t|%d    ::     %d %|20t|(%s)");
      MTYPE::const_iterator mc;
      for(mc=regionMap.begin();mc!=regionMap.end();mc++)
	{
	  const CTYPE::element_type* FPTR=
	    getObject<CTYPE::element_type>(mc->first);
	  const int flag=(FPTR) ? 1 : 0;
	  OX<<(FMT % mc->first % mc->second.first % 
	       mc->second.second % FStatus[flag]);
	  if (flag)
	    OX<<" "<<FPTR->getCentre();
	  OX<<std::endl;

	}
    }
  return;
  
}

template const attachSystem::FixedComp* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::ContainedComp* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::ContainedGroup* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::TwinComp* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::SecondTrack* 
  objectRegister::getObject(const std::string&) const;

template attachSystem::FixedComp* 
  objectRegister::getObject(const std::string&);

template attachSystem::ContainedComp* 
  objectRegister::getObject(const std::string&);

template attachSystem::ContainedGroup* 
  objectRegister::getObject(const std::string&);

template attachSystem::TwinComp* 
  objectRegister::getObject(const std::string&);

template attachSystem::SecondTrack* 
  objectRegister::getObject(const std::string&);


  

} // NAMESPACE ModelSupport
