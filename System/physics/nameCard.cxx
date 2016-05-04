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
  keyName(KN),
  activeFlag(72,0),nameOrder(72,std::string())
  /*!
    Constructor
  */
{
  reset();
}

nameCard::nameCard(const nameCard& A) : 
  activeFlag(A.activeFlag),nameOrder(A.nameOrder),
  Comp(A.Comp)
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
      activeFlag=A.activeFlag;
      nameOrder=A.nameOrder;
      Comp=A.Comp;
    }
  return *this;
}
  
nameCard::~nameCard()
  /*!
    Destructor
  */
{}

void
nameCard::reset()
  /*!
    Populate the Comp map
  */
{
  ELog::RegMethod RegA("nameCard","reset");
   std::erase(dIter.begin(),dIter.end());
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

  std::map<std::string,double>::iterator dIter;
  dIter=DUnit.find(kN);
  if (iIter==DUnit.end())
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


  
void
nameCard::write(std::ostream& OX) const
  /*!
    Write out the card
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("nameCard","write");

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

///\cond TEMPLATE
  
template
void nameCard::setComp(const std::string&,const double&);
template
void nameCard::setComp(const std::string&,const long int&);

template
const double& nameCard::getComp(const std::string&) const; 
template
const long int& nameCard::getComp(const std::string&) const;

///\endcond TEMPLATE

} // NAMESPACE physicsCards
      
   
