/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/inputSupport.cxx
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
#include <iterator>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cctype>
#include <complex>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "InputControl.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"

#include "inputSupport.h"

/*! 
  \file inputSupport.cxx 
*/

namespace mainSystem
{

bool
hasInput(const MITYPE& inputMap,const std::string& keyName)
  /*!
    Has map got a key
    \param inputMap :: Map of vector to search
    \param keyName :: keyname
  */
{
  MITYPE::const_iterator mc = inputMap.find(keyName);
  return (mc==inputMap.end()) ? 0 : 1;
}

size_t
sizeInput(const MITYPE& inputMap,const std::string& keyName)
  /*!
    Size of item in the key [if exists]
    \param inputMap :: Map of vector to search
    \param keyName :: keyname
    \return number of entries
  */
{
  MITYPE::const_iterator mc = inputMap.find(keyName);
  return (mc==inputMap.end()) ? 0 : mc->second.size();
}
  
  
template<typename T>
bool
findInput(const MITYPE& inputMap,const std::string& keyName,
	  const size_t index,T& OutValue)
  /*!
    Get a unit from a map
    \param inputMap :: Map of values
    \param keyName :: keyname
    \param index :: Index offset in found vector
    \param OutValue :: Default value if not found /result space
    \return true if object exists in correct form
  */
{
  MITYPE::const_iterator mc;
  if ((mc=inputMap.find(keyName)) != inputMap.end())
    {
      if (index<mc->second.size() &&
	  StrFunc::convert(mc->second[index],OutValue))
	return 1;
    }
  return 0;
}

template<typename T>
T
getInput(const MITYPE& inputMap,const std::string& keyName,
	 const size_t index)
  /*!
    Get a unit from a map
    \param inputMap :: Map of values
    \param keyName :: keyname
    \param index :: index in found vector
  */
{
  MITYPE::const_iterator mc;
  if ((mc=inputMap.find(keyName)) != inputMap.end())
    {
      T testOut;
      if (index<mc->second.size() &&
	  StrFunc::convert(mc->second[index],testOut))
	return testOut;
    }
  throw ColErr::InContainerError<std::string>(keyName,"input error");
}

template<typename T>
T
getDefInput(const MITYPE& inputMap,const std::string& keyName,
	    const size_t index,const T& DefValue)
  /*!
    Get a unit from a map
    \param inputMap :: Map of values
    \param keyName :: keyname
    \param DefValue :: output value [default]
    \param index :: index offset
  */
{
  MITYPE::const_iterator mc;
  if ((mc=inputMap.find(keyName)) != inputMap.end())
    {
      T testOut;
      if (index<mc->second.size() &&
	  StrFunc::convert(mc->second[index],testOut))
	return testOut;
    }
  return DefValue;
}


  
/// \cond TEMPLATE 

template double
getInput(const MITYPE&,const std::string&,const size_t);

template std::string
getInput(const MITYPE&,const std::string&,const size_t);

template size_t
getInput(const MITYPE&,const std::string&,const size_t);


template Geometry::Vec3D
getDefInput(const MITYPE&,const std::string&,const size_t,
	    const Geometry::Vec3D&);

template double
getDefInput(const MITYPE&,const std::string&,const size_t,const double&);

template size_t
getDefInput(const MITYPE&,const std::string&,const size_t,const size_t&);

template std::string
getDefInput(const MITYPE&,const std::string&,const size_t,const std::string&);


template bool
findInput(const MITYPE&,const std::string&,const size_t,size_t&);

template bool
findInput(const MITYPE&,const std::string&,const size_t,double&);

template bool
findInput(const MITYPE&,const std::string&,const size_t,std::string&);

/// \endcond TEMPLATE 

}  // NAMESPACE stlFunc
