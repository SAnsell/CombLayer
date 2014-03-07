/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/MaterialSupport.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "MaterialSupport.h"

namespace ModelSupport
{

template<>
int
EvalMat(const FuncDataBase& Control,const std::string& Key)
  /*!
    Determine the material name based either on the string
    value or the material number
    \param Control :: Database of variables
    \param Key :: Name for Material
    \return MatNumber
  */
{
  ELog::RegMethod RegA("MaterialSupport[F]","EvalMat<int>");

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();

  const std::string A=Control.EvalVar<std::string>(Key);
  if (DB.createMaterial(A))
    return DB.getIndex(A);
  const int out=Control.EvalVar<int>(Key);  // throws if cannot make an int
  if(!DB.hasKey(out))
    throw ColErr::InContainerError<int>(out,"Material not present");
  return out;
}

template<>
std::string
EvalMat(const FuncDataBase& Control,const std::string& Key)
  /*!
    Determine the material name based either on the string
    value or the material number
    \param Control :: Database of variables
    \param Key :: Name for Material
    \return MatNumber
  */
{
  ELog::RegMethod RegA("MaterialSupport[F]","EvalMat<string>");

  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();
  const std::string A=Control.EvalVar<std::string>(Key);
  int out = DB.hasKey(A);
  if (out) 
    return A;

  out=Control.EvalVar<int>(Key);  // throws if cannot make an int
  if(!DB.hasKey(out))
    throw ColErr::InContainerError<int>(out,"Material not present");

  return DB.getKey(out);
}

template<typename T>
T
EvalMat(const FuncDataBase& Control,
	const std::string& KeyA,
	const std::string& KeyB)
  /*!
    Determine the material name based either on the string
    value or the material number
    \param Control :: Database of variables
    \param KeyA :: First string to search    
    \param KeyB :: Second string to search 
    \return MatNumber
  */
{
  ELog::RegMethod RegA("MaterialSupport[F]","EvalMat<string,string>");
  return (Control.hasVariable(KeyA)) ? 
    EvalMat<T>(Control,KeyA) : EvalMat<T>(Control,KeyB);
}

template<typename T>
T
EvalDefMat(const FuncDataBase& Control,
	   const std::string& KeyA,
	   const T& defVal)
  /*!
    Determine the material name based either on the string
    value or the material number
    \param Control :: Database of variables
    \param KeyA :: First string to search    
    \param defVal :: Defaul value
    \return MatNumber
  */
{
  ELog::RegMethod RegA("MaterialSupport[F]","EvalDefMat");
  return (Control.hasVariable(KeyA)) ? 
    EvalMat<T>(Control,KeyA) : defVal;
}

int
EvalMatName(const std::string& matName)
  /*!
    Convert material name into a number
    \param matName :: Material to change
    \return index 
  */
{
  ELog::RegMethod RegA("MaterialSupport[F]","EvalMatName");

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();

  if (DB.createMaterial(matName))
    return DB.getIndex(matName);
  
  int index;
  if (StrFunc::convert(matName,index))
    {
      if(!DB.hasKey(index))
	throw ColErr::InContainerError<int>(index,"Material not present");
      return index;
    }
  throw ColErr::InContainerError<std::string>(matName,"Material not present");
}


template int 
EvalMat(const FuncDataBase&,const std::string&,
	const std::string&);

template std::string
EvalMat(const FuncDataBase&,const std::string&,
	 const std::string&);

template int 
EvalDefMat(const FuncDataBase&,const std::string&,
	   const int&);




}


