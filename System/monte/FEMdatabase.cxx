/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/FEMdatabase.cxx
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
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "FEMdatabase.h"

namespace ModelSupport
{
  
FEMdatabase::FEMdatabase() 
  /*!
    Constructor
  */
{
  initMaterial();
}

FEMdatabase&
FEMdatabase::Instance() 
  /*!
    Singleton constructor
    \return FEMdatabase object
  */
{
  static FEMdatabase DObj;
  return DObj;
}

void
FEMdatabase::addMaterial(const std::string& matName,
			 const double cP,const double K)
  /*!
    This takes the heat capacity and the conductivity
    \param Cp :: heat capacity [J/(g . K)]
    \param K :: thermal conductivity [ W/(cm . K) ]
  */
{
  ELog::RegMethod RegA("FEMdatabase","addMaterial");
  
  ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  const int index=DB.getIndex(matName);
  if (MStore.find(index)!=MStore.end())
    throw ColErr::InContainerError<std::string>(matName,"Material exists");

  const MonteCarlo::Material& MRef=DB.getMaterial(index);
  
  IndexMap.emplace(matName,index);
  // note rhoCp is rho(g/cc) * Cp (J/g/k) 
  MStore.emplace(index,FEMinfo{cP*MRef.getMacroDensity(),K});
  
  return;
}


void
FEMdatabase::initMaterial()
  /*!
    Default initializer [horrible]
    to populate a few materials mainly for development testing
    UNITS : 
  */
{
  ELog::RegMethod RegA("FEMdatabase","initMaterial");  

  // 
  // Material : HeatCapacity : Conductivity 
  //             [J/ (g K) ]     :    [W/ (cm K) ]
  addMaterial("Void",0,0);      
  addMaterial("Aluminium",0.921096,2.32);  //1.90 for other alloys
  addMaterial("Copper",0.376812,3.83);
  addMaterial("Silicon300K",0.711756,0.619);
  addMaterial("H2O",4.2,0.598);
  addMaterial("Stainless304",0.500,0.162);
  addMaterial("Stainless304L",0.500,0.162);
  addMaterial("Titanium",0.523,0.194);
  addMaterial("Gold",0.126,3.10);
  
  return;

}

bool
FEMdatabase::hasKey(const std::string& Key) const
  /*!
    Determine if the string exists in the data base
    \param Key :: KeyName
    \return 1 / 0 on fail
  */
{
  ELog::RegMethod RegA("FEMdatabase","hasKey<string>");
  
  SCTYPE::const_iterator sc=IndexMap.find(Key);
  return (sc==IndexMap.end()) ? 0 : 1;
}

bool
FEMdatabase::hasKey(const int KeyNum) const
  /*!
    Determine if the index exists in the data base
    \param KeyNum :: key number
    \return 1 / 0 on fail
  */
{
  ELog::RegMethod RegA("FEMdatabase","hasKey<int>");
  
  MTYPE::const_iterator mc=MStore.find(KeyNum);
  return (mc==MStore.end()) ? 0 : 1;
}

const std::string&
FEMdatabase::getKey(const int keyNum) const
  /*!
    Determine if the index exists in the data base
    \param KeyNum :: key number
    \return matNumber / 0 on fail
  */
{
  ELog::RegMethod RegA("FEMdatabase","getKey");

  for(const auto& [name,index] : IndexMap)
    if (index==keyNum) return name;

  throw ColErr::InContainerError<int>(keyNum,"KeyNum");
}

  
int
FEMdatabase::getIndex(const std::string& Key) const
  /*!
    Determine if the index exists in the data base
    \param Key :: key name
    \return matNumber 
  */
{
  ELog::RegMethod RegA("FEMdatabase","getIndex");
  
  SCTYPE::const_iterator sc=IndexMap.find(Key);
  if (sc==IndexMap.end())
    throw ColErr::InContainerError<std::string>(Key,"Key");

  return sc->second;
}

double
FEMdatabase::getK(const std::string& Key) const
  /*!
    Get a k (thermal conductivity) value [W/cm/K]
    \param Key :: Material name
    \return Thermal conductivity [W/cm/K]
  */
{
  ELog::RegMethod RegA("FEMdatabase","getK"); 
 
  SCTYPE::const_iterator sc=IndexMap.find(Key);
  if (sc==IndexMap.end())
    throw ColErr::InContainerError<std::string>(Key,"Key in IndexMap");

  return getK(sc->second);
}

double
FEMdatabase::getK(const int index) const
  /*!
    Get a k (thermal conductivity) value [W/cm/K]
    \param index :: Material number
    \return Thermal conductivity [W/cm/K]
  */
{
  ELog::RegMethod RegA("FEMdatabase","getK(int)");

  MTYPE::const_iterator mc=MStore.find(index);
  if (mc==MStore.end())
    throw ColErr::InContainerError<int>(index,"Index key in MStore");
  return mc->second.K;
}

  
double
FEMdatabase::getRhoCp(const std::string& key) const
  /*!
    Get a rho * Cp value
    \param key :: Material name
    \return rho * Cp  [ J/cm^3/K ]
  */
{
  ELog::RegMethod RegA("FEMdatabase","getRhoCp(int)");

  SCTYPE::const_iterator sc=IndexMap.find(key);
  if (sc==IndexMap.end())
    throw ColErr::InContainerError<std::string>(key,"Key in IndexMap");

  return getRhoCp(sc->second);
}

double
FEMdatabase::getRhoCp(const int index) const
  /*!
    Get a rho * Cp value
    \param index :: Material number
    \return rho * Cp  [ J/cm^3/K ]
  */
{
  ELog::RegMethod RegA("FEMdatabase","getRhoCp(int)");

  MTYPE::const_iterator mc=MStore.find(index);
  if (mc==MStore.end())
    {
      ModelSupport::DBMaterial& DB=
	ModelSupport::DBMaterial::Instance();
      throw ColErr::InContainerError<std::string>
	(DB.getKey(index)," Index :"+std::to_string(index)+" in MStore");
    }
  return mc->second.RhoCp;
}


  
} // NAMESPACE ModelSupport
