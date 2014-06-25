/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/defaultConfig.cxx
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "defaultConfig.h"

namespace mainSystem
{

defaultConfig::defaultConfig(const std::string& Key)  :
  keyName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

defaultConfig::defaultConfig(const defaultConfig& A) : 
  keyName(A.keyName),varVal(A.varVal),varName(A.varName),
  varVec(A.varVec),flagName(A.flagName)
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
    varVec.insert(std::pair<std::string,Geometry::Vec3D>(K,V));
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
    varVal.insert(std::pair<std::string,double>(K,V));
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
    varName.insert(std::pair<std::string,std::string>(K,V));
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
    flagName.insert(std::pair<std::string,std::string>(K,V));
  return;
}

void
defaultConfig::process(FuncDataBase& Control,
		       mainSystem::inputParam& IParam) const
  /*!
    Process the cards
    \param Control :: FuncDataBase to update
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

  for(mc=flagName.begin();mc!=flagName.end();mc++)
    {
      if(!IParam.flag(mc->first))
	IParam.setValue(mc->first,mc->second);
    }
  return;
}


  
}  // NAMESPACE ModeSupport
