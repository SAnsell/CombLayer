/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/nameCard.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "nameCard.h"

namespace physicsSystem
{
  
nameCard::nameCard(const std::string& KN) :
  keyName(KN),active(0)
  /*!
    Constructor
    \param KN :: Name
  */
{
  reset();
}

nameCard::nameCard(const nameCard& A) : 
  keyName(A.keyName),active(A.active),
  nameOrder(A.nameOrder),regNames(A.regNames),
  DUnit(A.DUnit),IUnit(A.IUnit),SUnit(A.SUnit)
  /*!
    Copy constructor
    \param A :: nameCard to copy
  */
{}

nameCard&
nameCard::operator=(const nameCard& A)
  /*!
    Assignment operator
    \param A :: nameCard to copy
    \return *this
  */
{
  if (this!=&A)
    {
      keyName=A.keyName;
      active=A.active;
      regNames=A.regNames;
      nameOrder=A.nameOrder;
      DUnit=A.DUnit;
      IUnit=A.IUnit;
      SUnit=A.SUnit;
    }
  return *this;
}
  
  
nameCard::~nameCard()
  /*!
    Destructor
  */
{}

const std::string&
nameCard::getTypeName(const nameCard::MData& AType)
  /*!
    Convert a type to a MDATA name
    \param AType :: MData object to process
    \return MData type
  */
{
  static const std::string AList[3]=
    { "double","integer","string" };
  return AList[static_cast<size_t>(AType)-1];
}

nameCard::MData
nameCard::convertName(const std::string& Item)
  /*!
    Convert a name to a MData type index
    \param Item :: Item to convert
    \return MData type
  */
{
  ELog::RegMethod RegA("nameCard","convertName");
  
  if (Item=="DBL" || Item=="dbl" ||
      Item=="double" || Item=="DOUBLE")
    return MData::DBL;
  if (Item=="INT" || Item=="int" ||
      Item=="integer" || Item=="INTEGER")
    return MData::INT;
  if (Item=="STR" || Item=="str" ||
      Item=="string" || Item=="STRING")
    return MData::STR;

  throw ColErr::InContainerError<std::string>
    (Item,"Item no known type");
}

  
void
nameCard::registerItems(const std::vector<std::string>& Items)
  /*!
    Register Items into name
  */
{
  std::string K,Type;
  for(std::string full : Items)
    {
      StrFunc::section(full,K);
      StrFunc::section(full,Type);
      
      regNames.emplace(K,convertName(Type));
    }
  return;
}
  
void
nameCard::reset()
  /*!
    Populate the Comp map
  */
{
  ELog::RegMethod RegA("nameCard","reset");
  DUnit.erase(DUnit.begin(),DUnit.end());
  IUnit.erase(IUnit.begin(),IUnit.end());
  SUnit.erase(SUnit.begin(),SUnit.end());
  nameOrder.clear();
  return;
}
  
  

template<>
void
nameCard::setItem(const std::string& kN,const double& Value) 
  /*!
    Set an item based on the type
    \param kN :: Index number to return
    \\parma Value :: vale to set
   */
{
  ELog::RegMethod RegA("nameCard","setItem<double>");

  active=1;
  std::map<std::string,double>::iterator dIter;
  dIter=DUnit.find(kN);
  if (dIter==DUnit.end())
    {
      DUnit.emplace(kN,Value);
      nameOrder.push_back(kN);
    }
  else
    {
      dIter->second=Value;
    }
  return;
}

template<>
void
nameCard::setItem(const std::string& kN,const long int& Value) 
  /*!
    Get an item based on the type
    \param keyName :: Index number to return
    \param Value :: vale to set
   */
{
  ELog::RegMethod RegA("nameCard","setItem<long int>");

  active=1;
  std::map<std::string,long int>::iterator iIter;
  iIter=IUnit.find(kN);
  if (iIter==IUnit.end())
    {
      IUnit.emplace(kN,Value);
      nameOrder.push_back(kN);
    }
  else
    {
      iIter->second=Value;
    }
  return;
}


template<>
void
nameCard::setItem(const std::string& kN,
                  const std::string& Value) 
  /*!
    Set an item based on the type
    \param kN :: Index number to return
    \param Value :: vale to set
   */
{
  ELog::RegMethod RegA("nameCard","setItem<string>");

  active=1;
  std::map<std::string,std::string>::iterator sIter;
  sIter=SUnit.find(kN);
  if (sIter==SUnit.end())
    {
      SUnit.emplace(kN,Value);
      nameOrder.push_back(kN);
    }
  else
    {
      sIter->second=Value;
    }
  return;
}

template<>
void
nameCard::setRegItem(const std::string& kN,
		     const std::string& Item)
  /*!
    Turn a registered item into a real item
    \param kN :: KeyNmae
    \param Item :: value [convertable]
   */
{
  ELog::RegMethod RegA("nameCard","setRegUnit<string>");

  std::map<std::string,MData>::const_iterator mc=
    regNames.find(kN);
  if (mc==regNames.end())
    throw ColErr::InContainerError<std::string>
      (kN,"keyName not registered");
  const MData AType=mc->second;
  if (AType==MData::DBL)
    {
      double V;
      if (StrFunc::convert(Item,V))
	{
	  setItem(kN,V);
	  return;
	}
    }
  else if (AType==MData::INT)
    {
      long int V;
      if (StrFunc::convert(Item,V))
	{
	  setItem(kN,V);
	  return;
	}
    }
  else if (AType==MData::STR)
    {
      setItem(kN,Item);
      return;
    }
  throw ColErr::InContainerError<std::string>
    (Item," Key = "+kN+" unconverted to "+getTypeName(AType));
}

template<>
void
nameCard::setRegItem(const std::string& kN,
		     const long int& Item)
  /*!
    Turn a registered item into a real item
    \param kN :: KeyNmae
    \param Item :: value [convertable]
   */
{
  ELog::RegMethod RegA("nameCard","setRegUnit<int>");

  std::map<std::string,MData>::const_iterator mc=
    regNames.find(kN);
  if (mc==regNames.end())
    throw ColErr::InContainerError<std::string>
      (kN,"keyName not registered");

  const MData AType=mc->second;
  if (AType==MData::DBL)
    {
      setItem(kN,static_cast<double>(Item));
      return;
    }
  else if (AType==MData::INT)
    {
      setItem(kN,Item);
      return;
    }
  else if (AType==MData::STR)
    {
      setItem(kN,StrFunc::makeString(Item));
      return;
    }
  throw ColErr::InContainerError<long int>
    (Item," Key = "+kN+" unconverted to "+getTypeName(AType));
}

template<>
void
nameCard::setRegItem(const std::string& kN,
		     const double& Item)
  /*!
    Turn a registered item into a real item
    \param kN :: KeyNmae
    \param Item :: value [convertable]
   */
{
  ELog::RegMethod RegA("nameCard","setRegUnit<double>");

  std::map<std::string,MData>::const_iterator mc=
    regNames.find(kN);
  if (mc==regNames.end())
    throw ColErr::InContainerError<std::string>
      (kN,"keyName not registered");

  const MData AType=mc->second;
  if (AType==MData::DBL)
    {
      setItem(kN,Item);
      return;
    }
  else if (AType==MData::INT)
    {
      setItem(kN,static_cast<long int>(Item));
      return;
    }
  else if (AType==MData::STR)
    {
      setItem(kN,StrFunc::makeString(Item));
      return;
    }
  throw ColErr::InContainerError<double>
    (Item," Key = "+kN+" unconverted to "+getTypeName(AType));
}
  

template<>
const double&
nameCard::getItem<double>(const std::string& kN) const
  /*!
    Set an item based on the type
    \param kN :: Index number to return
   */
{
  ELog::RegMethod RegA("nameCard","getItem<double>");

  std::map<std::string,double>::const_iterator dIter;
  dIter=DUnit.find(kN);
  if (dIter==DUnit.end())
    throw ColErr::InContainerError<std::string>(kN,"Key(double)");

  return dIter->second;
}

template<>
const long int& 
nameCard::getItem<long int>(const std::string& kN)  const
  /*!
    Set an item based on the type
    \param kN :: Index number to return
   */
{
  ELog::RegMethod RegA("nameCard","getItem<long int>");

  std::map<std::string,long int>::const_iterator iIter;
  iIter=IUnit.find(kN);
  if (iIter==IUnit.end())
    throw ColErr::InContainerError<std::string>(kN,"Key(long int)");

  return iIter->second;
}

template<>
const std::string&
nameCard::getItem<std::string>(const std::string& kN) const
  /*!
    Set an item based on the type
    \param kN :: Index number to return
   */
{
  ELog::RegMethod RegA("nameCard","getItem<string>");

  std::map<std::string,std::string>::const_iterator sIter;
  sIter=SUnit.find(kN);
  if (sIter==SUnit.end())
    throw ColErr::InContainerError<std::string>(kN,"Key(string)");

  return sIter->second;
}
  
void
nameCard::write(std::ostream& OX) const
  /*!
    Write out the card
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("nameCard","write");

  ELog::EM<<"Writing "<<keyName<<ELog::endDiag;
  if (!active) return;
  
  std::map<std::string,double>::const_iterator dIter;
  std::map<std::string,long int>::const_iterator iIter;
  std::map<std::string,std::string>::const_iterator sIter;

  std::ostringstream cx;
  
  cx<<keyName<<" ";
  for(const std::string& K : nameOrder)
    {
      cx<<" "<<K<<"=";
      iIter=IUnit.find(K);
      if (iIter!=IUnit.end())
        {
          cx<<iIter->second;
          continue;
        }
      dIter=DUnit.find(K);
      if (iIter!=IUnit.end())
        {
          cx<<dIter->second;
          continue;
        }
      sIter=SUnit.find(K);
      if (iIter!=IUnit.end())
        {
          cx<<sIter->second;
          continue;
        }
    }
  
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}


} // NAMESPACE physicsSystem
      
   
