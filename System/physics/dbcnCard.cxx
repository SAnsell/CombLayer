/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   physics/dbcnCard.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "dbcnCard.h"

namespace physicsSystem
{
  
dbcnCard::dbcnCard() :
  activeFlag(72,0),nameOrder(72,std::string())
  /*!
    Constructor
  */
{
  reset();
}

dbcnCard::dbcnCard(const dbcnCard& A) : 
  activeFlag(A.activeFlag),nameOrder(A.nameOrder),
  Comp(A.Comp)
  /*!
    Copy constructor
    \param A :: dbcnCard to copy
  */
{}

dbcnCard&
dbcnCard::operator=(const dbcnCard& A)
  /*!
    Assignment operator
    \param A :: dbcnCard to copy
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
  
dbcnCard::~dbcnCard()
  /*!
    Destructor
  */
{}

void
dbcnCard::reset()
  /*!
    Populate the Comp map
  */
{
  ELog::RegMethod RegA("dbcnCard","populate");
  
  // TTYPE : index : type [1=int,2=double,0=not-set]; long int : double
  typedef std::map<std::string,TTYPE>::value_type MVAL;

  std::fill(activeFlag.begin(),activeFlag.end(),0);
  
  Comp.erase(Comp.begin(),Comp.end());
  Comp.insert(MVAL("rndSeed",TTYPE(0,1,123456781L,0.0)));
  Comp.insert(MVAL("debugPrint",TTYPE(1,1,0,0.0)));                  
  Comp.insert(MVAL("eventStart",TTYPE(2,1,0,0.0)));            
  Comp.insert(MVAL("eventEnd",TTYPE(3,1,0,0.0)));              
  Comp.insert(MVAL("maxEvents",TTYPE(4,1,600,0.0)));              
  Comp.insert(MVAL("detUnderFlowLimit",TTYPE(5,2,0,80.0)));
  Comp.insert(MVAL("printSurfCalcFlag",TTYPE(6,1,0,0.0)));  
  Comp.insert(MVAL("histStart",TTYPE(7,1,1,0.0)));   
  Comp.insert(MVAL("surfTol",TTYPE(8,2,0,1e-4)));        
  Comp.insert(MVAL("printCollision",TTYPE(10,1,0,0.0)));
  Comp.insert(MVAL("stride",TTYPE(12,1,152917,0.0)));       
  Comp.insert(MVAL("gen",TTYPE(13,1,1,0.0))); 
  Comp.insert(MVAL("printVOV",TTYPE(14,1,0,0.0)));
  Comp.insert(MVAL("histScale",TTYPE(15,2,0,1.0)));         
  Comp.insert(MVAL("eleAngDefMethod",TTYPE(16,1,0,0.0)));
  Comp.insert(MVAL("electronStragglingType",TTYPE(17,1,2,0.0)));
  Comp.insert(MVAL("forcePHTVRtree",TTYPE(22,1,0,0.0)));    
  Comp.insert(MVAL("bankSize",TTYPE(27,1,2048,0.0)));         
  Comp.insert(MVAL("genxsType",TTYPE(31,1,0,0.0)));                  
  Comp.insert(MVAL("ionSmooth",TTYPE(32,1,0,0.0)));                 
  Comp.insert(MVAL("sqwMethod",TTYPE(38,1,1,0.0)));                 

  nameOrder.erase(nameOrder.begin(),nameOrder.end());
  for(const MVAL& mc : Comp)
    nameOrder[std::get<0>(mc.second)]=mc.first;
  return;
}
  
template<>
const double&
dbcnCard::getItem(const std::string& keyName) const
  /*!
    Get an item based on the type
    \param keyName :: Index number to return
    \return value 
   */
{
  ELog::RegMethod RegA("dbcnCard","getItem<double> const");

  std::map<std::string,TTYPE>::const_iterator mc=Comp.find(keyName);
  if (mc!=Comp.end() && std::get<1>(mc->second)==2)
    return std::get<3>(mc->second);

  throw ColErr::InContainerError<std::string>
    (keyName,"Unknown Index value");
}

template<>
const long int&
dbcnCard::getItem(const std::string& keyName) const
  /*!
    Get an item based on the type
    \param keyName :: Index number to return
    \return long int result
   */
{
  ELog::RegMethod RegA("dbcnCard","getItem<int> const");
  
  std::map<std::string,TTYPE>::const_iterator mc;
  mc=Comp.find(keyName);
  if (mc!=Comp.end() && std::get<1>(mc->second)==1)
    return std::get<2>(mc->second);

  throw ColErr::InContainerError<std::string>
    (keyName,"Unknown Index value");
}
  
template<>
double&
dbcnCard::getItem(const std::string& keyName) 
  /*!
    Get an item based on the type
    \param keyName :: Index number to return
    \return reference name 
   */
{
  ELog::RegMethod RegA("dbcnCard","getItem<double>");
  std::map<std::string,TTYPE>::iterator mc;
  mc=Comp.find(keyName);
  if (mc!=Comp.end() && std::get<1>(mc->second)==2)
    return std::get<3>(mc->second);

  throw ColErr::InContainerError<std::string>
    (keyName,"Unknown Index value");
}

template<>
long int&
dbcnCard::getItem(const std::string& keyName) 
  /*!
    Get an item based on the type
    \param keyName :: Index number to return
    \return long int result
   */
{
  ELog::RegMethod RegA("dbcnCard","getItem<int>");
  
  std::map<std::string,TTYPE>::iterator mc;
  mc=Comp.find(keyName);
  if (mc!=Comp.end() && std::get<1>(mc->second)==1)
    return std::get<2>(mc->second);

  throw ColErr::InContainerError<std::string>
    (keyName,"Unknown Index value");
}
  
void
dbcnCard::setActive(const std::string& key,const int flag)
  /*!
    Activate a card
    \param key :: name for index
    \param flag :: flag for index
   */
{
  ELog::RegMethod RegA("dbcnCard","setActive");
  std::map<std::string,TTYPE>::const_iterator mc=
    Comp.find(key);

  if (mc==Comp.end())
    throw ColErr::InContainerError<std::string>(key,"key not found");

  const size_t index=std::get<0>(mc->second);
  activeFlag[index]=flag;
  return;
}

template<typename T>
void
dbcnCard::setComp(const std::string& Key,const T& Value)
  /*!
    Set component value [and activate]
    \param Key :: Key value for item
    \param Value :: value to set
   */
{
  ELog::RegMethod RegA("dbcnCard","setComp");

  getItem<T>(Key)=Value;
  setActive(Key,1);

  return;
}

template<typename T>
const T&
dbcnCard::getComp(const std::string& Key) const
  /*!
    Get component value 
    \param Key :: Key value for item
    \param Value :: value to set
   */
{
  ELog::RegMethod RegA("dbcnCard","setComp(double)");
  return getItem<T>(Key);
}

size_t
dbcnCard::keyType(const std::string& keyName) const
  /*!
    Determine typenumber of the keyType 
    \param keyName :: Key name to look up
    \retval 1 : long int 
    \retval 2 : double
    \retval 0 : Fail / Not used
   */
{
  std::map<std::string,TTYPE>::const_iterator mc=
    Comp.find(keyName);
  return (mc!=Comp.end()) ? std::get<1>(mc->second) : 0;
}
  
        
std::string
dbcnCard::itemString(const std::string& keyName) const
  /*!
    Process an item in to a string
    \param keyName :: Kye name
    \return string
   */
{
  ELog::RegMethod RegA("dbcnCard","itemString");

  const size_t kTYPE=keyType(keyName);
  if (kTYPE==1)
    return StrFunc::makeString(getItem<long int>(keyName));
  else if (kTYPE==2)
    return StrFunc::makeString(getItem<double>(keyName));

  throw ColErr::InContainerError<std::string>(keyName,"Unknow keyName value");
}

  
void
dbcnCard::write(std::ostream& OX) const
  /*!
    Write out the card
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("dbcnCard","write");

  size_t lastValid;
  for(lastValid=activeFlag.size();lastValid &&
	!activeFlag[lastValid-1];lastValid--) ;
  if (!lastValid) return;

  std::ostringstream cx;
  cx<<"dbcn ";
  size_t jcnt(0);
  for(size_t i=0;i<lastValid;i++)
    {
      if (activeFlag[i])
	{
	  if (jcnt)  // write old j cnt;
	    {
	      if (jcnt>1) cx<<jcnt;
	      cx<<"j ";
	      jcnt=0;
	    }
	  cx<<itemString(nameOrder[i])<<" ";
	}
      else
	jcnt++;
    }
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

///\cond TEMPLATE
  
template
void dbcnCard::setComp(const std::string&,const double&);
template
void dbcnCard::setComp(const std::string&,const long int&);

template
const double& dbcnCard::getComp(const std::string&) const; 
template
const long int& dbcnCard::getComp(const std::string&) const;

///\endcond TEMPLATE

} // NAMESPACE physicsCards
      
   
