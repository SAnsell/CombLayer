/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupport/defaultConfig.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "defaultConfig.h"

namespace mainSystem
{

defaultConfig::defaultConfig(std::string  Key)  :
  keyName(std::move(Key))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

defaultConfig::defaultConfig(const defaultConfig& A) : 
  keyName(A.keyName),varVal(A.varVal),varName(A.varName),
  varVec(A.varVec),flagName(A.flagName),multiSet(A.multiSet)
  /*!
    Copy constructor
    \param A :: defaultConfig to copy
  */
{}

defaultConfig&
defaultConfig::operator=(const defaultConfig& A)
  /*!
    Assignment operator
    \param A :: defaultConfig to copy
    \return *this
  */
{
  if (this!=&A)
    {
      varVal=A.varVal;
      varName=A.varName;
      varVec=A.varVec;
      flagName=A.flagName;
      multiSet=A.multiSet;
    }
  return *this;
}


defaultConfig::~defaultConfig() 
 /*!
   Destructor
 */
{}

void
defaultConfig::setVar(const std::string& K,const Geometry::Vec3D& V) 
  /*!
    Set a string type variable
    \param K :: Keyname
    \param V :: Varaible value
  */
{
  if (!K.empty())
    varVec[K]=V;
  return;
}

void
defaultConfig::setVar(const std::string& K,const double V) 
  /*!
    Set a string type variable
    \param K :: Keyname
    \param V :: Varaible value
  */
{
  if (!K.empty())
    varVal[K]=V;
  return;
}

void
defaultConfig::setVar(const std::string& K,const std::string& V) 
  /*!
    Set a string type variable
    \param K :: Keyname
    \param V :: Varaible value
  */
{
  if (!K.empty() && !V.empty())
    varName[K]=V;
  return;
}

void
defaultConfig::setOption(const std::string& K,const std::string& V) 
  /*!
    Set a string type variable
    \param K :: Keyname
    \param V :: Varaible value
  */
{
  if (!K.empty() && !V.empty())
    flagName[K]=V;
  return;
}

void
defaultConfig::setMultiOption(const std::string& K,
			      const size_t index,
			      const std::string& V) 
  /*!
    Set a string type variable
    \param K :: Keyname
    \param index :: Index item in multi 
    \param V :: Varaible value
  */
{
  ELog::RegMethod RegA("defaultConfig","setMultiOption");
  if (!K.empty() && !V.empty())
    multiSet.push_back(TTYPE(K,index,V));
  return;
}

void
defaultConfig::process(FuncDataBase& Control,
		       mainSystem::inputParam& IParam) const
  /*!
    Convert input parameters into variables based on -v/-va cards
    \param Control :: FuncDataBase to update
    \param IParam :: Input arguments
   */
{
  ELog::RegMethod RegA("defaultConfig","process");


  std::map<std::string,double>::const_iterator dc;
  for(dc=varVal.begin();dc!=varVal.end();dc++)
    Control.addVariable(dc->first,dc->second);

  std::map<std::string,std::string>::const_iterator mc;
  for(mc=varName.begin();mc!=varName.end();mc++)
    Control.addVariable(mc->first,mc->second);

  std::map<std::string,Geometry::Vec3D>::const_iterator vc;
  for(vc=varVec.begin();vc!=varVec.end();vc++)
    Control.addVariable(vc->first,vc->second);

  // Option flags
  for(const auto& [aStr, bStr] : flagName)
    {
      if(!IParam.flag(aStr))
	IParam.setValue(aStr,bStr);
    }


  // Multi set
  for(const TTYPE& TI : multiSet)
    {
      if (!IParam.flag(std::get<0>(TI)))
	{
	  IParam.setMultiValue(std::get<0>(TI),std::get<1>(TI),
			       std::get<2>(TI));
	}
    }
  // Set active flags:
  for(const auto& [aStr, bStr] : flagName)
    IParam.setFlag(aStr);
  for(const TTYPE& TI : multiSet)
    IParam.setFlag(std::get<0>(TI));
  return;
}

  
}  // NAMESPACE mainSystem
