/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/sourceDataBase.cxx
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
#include <numeric>
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
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "support.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "stringCombine.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SourceBase.h"
#include "sourceDataBase.h"

namespace SDef
{

sourceDataBase::sourceDataBase() 
  /*!
    Constructor
  */
{}

sourceDataBase::~sourceDataBase() 
  /*!
    Destructor
  */
{}
  
sourceDataBase& 
sourceDataBase::Instance() 
  /*!
    Effective this object			
    \return sourceDataBase object
  */
{
  static sourceDataBase A;
  return A;
}

void
sourceDataBase::reset()
  /*!
    Delete all the references to the shared_ptr register
  */
{
  Components.erase(Components.begin(),Components.end());
  return;
}

void
sourceDataBase::registerSource(const std::string& Name,
			       const SourceBase& SObj)
  /*!
    Add a source and manage the memory
    \param Name :: Name of source
    \param SObj :: source Ptr
  */
{
  ELog::RegMethod RegA("sourceDataBase","registerSource");
  
  SMAP::iterator mc=Components.find(Name);
  if (mc != Components.end())
    {
      if (mc->second.get()== &SObj) return;
      Components.erase(mc);
    }
  Components.emplace(Name,STYPE(SObj.clone()));
  return;
}

const SourceBase*
sourceDataBase::getInternalSource(const std::string& Name) const
  /*!
    Find a SourceBase [if it exists] 
    \param Name :: Name of source
    \return SourcePtr / 0 
  */
{
  ELog::RegMethod RegA("sourceDataBase","getInternalSource(const)");

  SMAP::const_iterator mc=Components.find(Name);
  return (mc!=Components.end()) ? mc->second.get() : 0;
}

SourceBase*
sourceDataBase::getInternalSource(const std::string& Name) 
  /*!
    Find a SourceBase [if it exists] 
    \param Name :: Name of source
    \return SourcePtr / 0 
  */
{
  ELog::RegMethod RegA("sourceDataBase","getInternalSource()");

  SMAP::const_iterator mc=Components.find(Name);
  return (mc!=Components.end()) ? mc->second.get() : 0;
}

template<typename T>
const T*
sourceDataBase::getSource(const std::string& Name) const
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return SourcePtr / 0 
  */
{
  ELog::RegMethod RegA("sourceDataBase","getSource(const)");

  const SourceBase* SPtr = getInternalSource(Name);
  return dynamic_cast<const T*>(SPtr);
}

template<typename T>
const T*
sourceDataBase::getSourceThrow(const std::string& Name,
                               const std::string& Err) const
  /*!
    Find a source 
    Throws InContainerError if not present in correct type
    \param Name :: Name
    \param Err :: Error string for exception
    \return SourcePtr 
  */
{
  ELog::RegMethod RegA("sourceDataBase","getSourceThrow(const)");
  
  const T* SPtr=getSource<T>(Name);
  if (!SPtr)
    throw ColErr::InContainerError<std::string>(Name,Err);
  return SPtr;
}

template<typename T>
T*
sourceDataBase::getSource(const std::string& Name) 
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return SourcePtr / 0 
  */
{
  ELog::RegMethod RegA("sourceDataBase","getSource()");

  SourceBase* SPtr = getInternalSource(Name);
  return dynamic_cast<T*>(SPtr);
}

template<typename T>
T*
sourceDataBase::getSourceThrow(const std::string& Name,
                               const std::string& Err) 
  /*!
    Find a source 
    Throws InContainerError if not present in correct type
    \param Name :: Name
    \param Err :: Error string for exception
    \return SourcePtr 
  */
{
  ELog::RegMethod RegA("sourceDataBase","getSourceThrow()");
  
  T* SPtr=getSource<T>(Name);
  if (!SPtr)
    throw ColErr::InContainerError<std::string>(Name,Err);
  return SPtr;
}

std::shared_ptr<SourceBase>
sourceDataBase::getSharedThrow(const std::string& Name,
                               const std::string& Err)
  /*!
    Find a source 
    Throws InContainerError if not present in correct type
    \param Name :: Name
    \param Err :: Error string for exception
    \return SourcePtr 
  */
{
  ELog::RegMethod RegA("sourceDataBase","getSharedThrow");
  
  SMAP::const_iterator mc=Components.find(Name);
  if (mc==Components.end())
    throw ColErr::InContainerError<std::string>(Name,Err);
      
  return mc->second;
}

  
void
sourceDataBase::write(const std::string&,
                      std::ostream&) const
  /*!
    Write out the source for MCNP
    \param SName :: source name
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("sourceDataBase","write");
  
  return;
}

///\cond TEMPLATE
template SourceBase* 
sourceDataBase::getSource(const std::string&); 

template SourceBase* 
sourceDataBase::getSourceThrow(const std::string&,const std::string&);

template const SourceBase* 
sourceDataBase::getSource(const std::string&) const; 

template const SourceBase* 
sourceDataBase::getSourceThrow(const std::string&,const std::string&) const;

///\endcond TEMPLATE
  
} // NAMESPACE SDef
