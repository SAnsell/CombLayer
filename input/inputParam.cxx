/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   input/inputParam.cxx
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
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "mathSupport.h"
#include "MapSupport.h"
#include "InputControl.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "IItemBase.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "inputParam.h"

namespace mainSystem
{

inputParam::inputParam()
  /*!
    Default constructor
  */
{}

inputParam::inputParam(const inputParam& A) 
  /*!
    Copy constructor
    \param A :: inputParamter to copy
  */
{
  copyMaps(A);
}

inputParam&
inputParam::operator=(const inputParam& A) 
  /*!
    Assignment operator
    \param A :: input to copy
    \return this
  */
{
  if (this!=&A)
    {
      copyMaps(A);
    }
  return *this;
}

inputParam::~inputParam()
  /*!
    Destructor
  */
{
  deleteMaps();
}

void
inputParam::copyMaps(const inputParam& A)
  /*!
    Copy the map from one inputParam to another
    \param A :: inputParam to used
   */
{
  ELog::RegMethod RegA("inputParam","copyMaps");

  if (!Keys.empty())
    deleteMaps();

  MTYPE::const_iterator mc;
  for(mc=A.Keys.begin();mc!=A.Keys.end();mc++)
    {
      IItemBase* IObj=mc->second->clone();
      Keys.insert(MTYPE::value_type(mc->first,IObj));
      if (A.Names.find(IObj->getLong())!=A.Names.end())
	Names.insert(MTYPE::value_type(IObj->getLong(),IObj));
    }
  return;
}

void
inputParam::deleteMaps()
  /*!
    Clean up the memory of the maps
  */
{
  MTYPE::iterator mc;
  for(mc=Keys.begin();mc!=Keys.end();mc++)
    delete mc->second;

  Keys.erase(Keys.begin(),Keys.end());
  Names.erase(Names.begin(),Names.end());
  return;
}
  
IItemBase*
inputParam::getIndex(const std::string& K)
  /*!
    Given a key find the index
    \param K :: Key value (short/long)
    \return ItemBase
   */
{
  ELog::RegMethod RegA("inputParam","getIndex");
  MTYPE::const_iterator mc;
  mc=Keys.find(K);
  if (mc==Keys.end())
    {
      mc=Names.find(K);
      if (mc==Names.end())  // get previous RegA item:
	throw ColErr::InContainerError<std::string>(K,"key code");
    }
  return mc->second;
}

const IItemBase*
inputParam::getIndex(const std::string& K) const
  /*!
    Given a key find the index
    \param K :: Key value (short/long)
    \return ItemBase
   */
{
  ELog::RegMethod RegA("inputParam","getIndex");
  MTYPE::const_iterator mc;
  mc=Keys.find(K);
  if (mc==Keys.end())
    {
      mc=Names.find(K);
      if (mc==Names.end())  // get previous RegA item:
	throw ColErr::InContainerError<std::string>(K,"key code");
    }
  return mc->second;
}

const IItemBase*
inputParam::findLongKey(const std::string& K) const
  /*!
    Given a key find the Item  Does not-throw
    \param K :: Key value (long)
    \return Ptr / 0 on failure
   */
{
  MTYPE::const_iterator mc;
  mc=Names.find(K);
  return (mc!=Names.end()) ? mc->second : 0;
}

const IItemBase*
inputParam::findShortKey(const std::string& K) const
  /*!
    Given a key find the Item
    Does not-throw
    \param K :: Key value (short)
    \return Ptr / 0 on failure
   */
{
  MTYPE::const_iterator mc;
  mc=Keys.find(K);
  return (mc!=Keys.end()) ? mc->second : 0;
}

const IItemBase*
inputParam::findKey(const std::string& K) const
  /*!
    Given a key find the Item
    Does not-throw
    \param K :: Key value (short)
    \return Ptr / 0 on failure
   */
{
  const IItemBase* OutPtr=findShortKey(K);
  return (!OutPtr) ? findLongKey(K) : OutPtr;
}

IItemBase*
inputParam::findLongKey(const std::string& K) 
  /*!
    Given a key find the Item  Does not-throw
    \param K :: Key value (long)
    \return Ptr / 0 on failure
   */
{
  MTYPE::const_iterator mc;
  mc=Names.find(K);
  return (mc!=Names.end()) ? mc->second : 0;
}

IItemBase*
inputParam::findShortKey(const std::string& K)
  /*!
    Given a key find the Item
    Does not-throw
    \param K :: Key value (short)
    \return Ptr / 0 on failure
   */
{
  MTYPE::const_iterator mc;
  mc=Keys.find(K);
  return (mc!=Keys.end()) ? mc->second : 0;
}

IItemBase*
inputParam::findKey(const std::string& K) 
  /*!
    Given a key find the Item
    Does not-throw
    \param K :: Key value (short)
    \return Ptr / 0 on failure
   */
{
  IItemBase* OutPtr=findShortKey(K);
  return (!OutPtr) ? findLongKey(K) : OutPtr;
}

bool
inputParam::hasKey(const std::string& K) const
  /*!
    Determine if an item with the key exists
    Does not-throw 
    \param K :: Key value (short)
    \return true if item exists
  */
{
  return (findShortKey(K)!=0 || findLongKey(K)!=0);
}

size_t
inputParam::dataCnt(const std::string& K) const
  /*!
    Determine if active
    \param K :: Key to seach
    \return active flag
   */
{
  ELog::RegMethod RegA("inputParam","dataCnt");
  const IItemBase* IPtr=getIndex(K);
  return IPtr->dataCnt();
}

size_t
inputParam::grpCnt(const std::string& K) const
  /*!
    Count number of groups
    \param K :: Key to seach
    \return Grp count 
   */
{
  ELog::RegMethod RegA("inputParam","grpCnt");
  const IItemBase* IPtr=getIndex(K);
  return IPtr->getNComp();
}


size_t
inputParam::itemCnt(const std::string& K,const size_t Index) const
  /*!
    Determine number of items in a specific group
    \param K :: Key to seach
    \param Index :: Nubmer of entry point
    \return active flag
   */
{
  ELog::RegMethod RegA("inputParam","itemCnt");
  const IItemBase* IPtr=getIndex(K);
  return IPtr->getNCompData(Index);
}


bool
inputParam::flag(const std::string& K) const
  /*!
    Get Flag state
    \param K :: Key to seach
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","flag");

  const IItemBase* IPtr=getIndex(K);
  return IPtr->flag();
}

template<typename T>
T
inputParam::getFlagDef(const std::string& InpKey,
		       const FuncDataBase& Control,
		       const std::string& VarKey,  
		       const size_t I) const
  /*!
    Get a value based initially on the input line,
    and to resolve to the variable.
    \param InpKey :: Key to seach
    \param Control :: FuncDataBase to search 
    \param VarKey :: Variable Key
    \param I :: Index value
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getFlagDef");

  const IItemBase* IPtr=findKey(InpKey);
  const IItemObj<T>* Ptr=dynamic_cast< const IItemObj<T>* >(IPtr);
  if (Ptr && I<Ptr->getNData())
    return Ptr->getObj(I);
  const IItemMulti<T>* MPtr=dynamic_cast< const IItemMulti<T>* >(IPtr);
  if (MPtr && I<MPtr->getNData())
    return MPtr->getObj(I);
  
  // Fall back to var set
  if (!Control.hasVariable(VarKey))
    {
      throw ColErr::InContainerError<std::string>(InpKey+":"+VarKey,
						  RegA.getFull());
    }
  return Control.EvalVar<T>(VarKey);
}


std::string
inputParam::getFull(const std::string& K,const size_t I) const
  /*!
    Get the full string of all options for a given index
    \param K :: Key to seach
    \param I :: Index value
    \return Values in spc separated list
   */
{
  ELog::RegMethod RegA("inputParam","getFull");

  const IItemBase* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>("Key : "+K);

  if (I>=IPtr->dataCnt())
    throw ColErr::IndexError<size_t>(I,IPtr->dataCnt(),
				     "key failed : "+K);

  // Output streamx
  std::ostringstream OX;
  IPtr->write(OX);
  return OX.str();
}

template<typename T>
const T&
inputParam::getValue(const std::string& K,const size_t I) const
  /*!
    Get a value based on key
    \param K :: Key to seach
    \param I :: Index value
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getValue");

  const IItemBase* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>("Key : "+K);

  if (I>=IPtr->dataCnt())
    throw ColErr::IndexError<size_t>(I,IPtr->dataCnt(),
				     "key failed : "+K);

  const IItemObj<T>* Ptr=dynamic_cast< const IItemObj<T>* >(IPtr);
  if (Ptr)
    return Ptr->getObj(I);

  const IItemMulti<T>* MPtr=dynamic_cast< const IItemMulti<T>* >(IPtr);
  if (!MPtr)
    throw ColErr::CastError<IItemBase>(IPtr,"key failed :"+K);

  return MPtr->getObj(I);
}

template<typename T>
const T&
inputParam::getCompValue(const std::string& K,const size_t Index,
			 const size_t Item) const
  /*!
    Get a value based on key
    \param K :: Key to seach
    \param Index :: Index value
    \param Item :: Item value [ of many ]
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getCompValue");

  const IItemBase* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>(K+":IPtr");
  const IItemMulti<T>* MPtr=dynamic_cast< const IItemMulti<T>* >(IPtr);
  if (!MPtr)
    throw ColErr::CastError<IItemBase>(IPtr,"key failed :"+K);

  return MPtr->getObj(Index,Item);
}

bool
inputParam::compNoCaseValue(const std::string& K,
			    const std::string& Value) const
  /*!
    Compare the value in the item to Value
    without consideration to case.
    \param K :: Key to use
    \param Value :: Value to compare
    \return true if equal 
   */
{
  ELog::RegMethod RegA("inputParam","compNoCaseValue");
  
  const IItemBase* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>("Key failed: "+K);
  const size_t N=IPtr->dataCnt();
  std::string NCValue(Value);
  boost::algorithm::to_lower(NCValue);
  for(size_t i=0;i<N;i++)
    {

      const IItemObj<std::string>* Ptr=
	dynamic_cast< const IItemObj<std::string>* >(IPtr);
      const IItemMulti<std::string>* MPtr=
	dynamic_cast< const IItemMulti<std::string>* >(IPtr);
      if (Ptr || MPtr) 
	{
	  std::string OStr=(Ptr) ? Ptr->getObj(i) : MPtr->getObj(i);
	  boost::algorithm::to_lower(OStr);      
	  if (NCValue==OStr)
	    return 1;
	}
    }
  return 0;
}


template<typename T>
bool
inputParam::compValue(const std::string& K,const T& Value) const
  /*!
    Compare the Value to the item stored in the key 
    \param K :: Key to seach
    \param Value :: Value to compare
    \return true if equal
   */
{
  ELog::RegMethod RegA("inputParam","compValue");

  const IItemBase* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>("Key failed: "+K);
  const size_t N=IPtr->dataCnt();
  for(size_t i=0;i<N;i++)
    {
      const IItemObj<T>* Ptr=dynamic_cast< const IItemObj<T>* >(IPtr);
      const IItemMulti<T>* MPtr=dynamic_cast< const IItemMulti<T>* >(IPtr);
      if (Ptr) 
	if (Ptr->getObj(i)==Value)
	  return 1;
      if (MPtr) 
	if (MPtr->getObj(i)==Value)
	  return 1;
      }
  return 0;
}

void
inputParam::setDesc(const std::string& K,const std::string& D)
/*!
  Set a value based on key
  \param K :: Key for item
  \param D :: Description to add
*/
{
  ELog::RegMethod RegA("inputParam","setFlag");

  IItemBase* IPtr=getIndex(K);
  IPtr->setDesc(D);
  return;
}

void
inputParam::setFlag(const std::string& K)
/*!
  Set a value based on key
  \param K :: Key to add/search
*/
{
  ELog::RegMethod RegA("inputParam","setFlag");

  IItemBase* IPtr=getIndex(K);
  IItemFlag* Ptr=dynamic_cast<IItemFlag*> (IPtr);
  if (!Ptr)
    throw ColErr::CastError<IItemBase>(IPtr,"Key Failed :"+K);

  Ptr->setObj();
  return;
}

template<typename T>
void
inputParam::setValue(const std::string& K,const T& A,
		     const size_t I) 
  /*!
    Set a value based on key
    \param K :: Key to add/search
    \param A :: Object ot set
    \param I :: Index value
  */
{
  ELog::RegMethod RegA("inputParam","setValue(N)");

  IItemBase* IPtr=getIndex(K);
  IItemObj<T>* Ptr=dynamic_cast<IItemObj<T>*> (IPtr);
  if (!Ptr)
    throw ColErr::CastError<IItemBase>(IPtr,"Key Failed :"+K);
  if (I>=Ptr->getNData())
    throw ColErr::IndexError<size_t>(I,Ptr->getNData(),
				     "Key Failed :"+K);

  Ptr->setObj(I,A);
  return;
}

void
inputParam::regFlag(const std::string& K,const std::string& LK)
  /*!
    Registers a Flag
    \param K :: Keyname
    \param LK :: Long keyname
  */
{
  ELog::RegMethod RegA("inputParam","regFlag");
  
  if (K.empty() || findShortKey(K)!=0)
    {
      ELog::EM<<"Failed to register item:"<<K<<ELog::endErr;
      return;
    }

  IItemFlag* IPtr=new IItemFlag(K,LK);
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));
  return;  
}

template<typename T>
void
inputParam::regItem(const std::string& K,
		    const std::string& LK,
		    const size_t NData)
  /*!
    Registers a particular type
    \param K :: Keyname
    \param LK :: Long keyname
    \param NData :: Number of data points
  */
{
  ELog::RegMethod RegA("inputParam","regItem<T>");
  
  if (K.empty() || findShortKey(K)!=0)
    {
      ELog::EM<<"Failed to register item:"<<K<<ELog::endErr;
      return;
    }
  IItemObj<T>* IPtr=new IItemObj<T>(NData,K,LK);
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));
  return;  
}

template<typename T>
void
inputParam::regMulti(const std::string& K,const std::string& LK,
		     const size_t nData,const long int nReq)
  /*!
    Registers a particular Multi type
    \param K :: Keyname
    \param LK :: Long keyname
    \param nData :: number of data items per unit
    \param nReq :: number of data actually required [-ve to mean all]
  */
{
  ELog::RegMethod RegA("inputParam","regMulti<T>");
  
  if (K.empty() || findShortKey(K)!=0)
    {
      ELog::EM<<"Failed to register item:"<<K<<ELog::endErr;
      return;
    }
  const size_t nAReq(nReq<0 ? nData : static_cast<size_t>(nReq));
  IItemMulti<T>* IPtr=new IItemMulti<T>(K,LK,nData,nAReq);
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));
  return;  
}


template<typename T>
void
inputParam::regDefItemList(const std::string& K,const std::string& LK,
			   const size_t NData,const std::vector<T>& AItems)
  /*!
    Registers a particular type
    \param K :: Keyname
    \param LK :: Long keyname
    \param NData :: Number of data points
    \param AItem :: Items with defaults [size <= NData]
  */
{
  ELog::RegMethod RegA("inputParam","regDefItemList<T>");
  
  if (K.empty() || findShortKey(K)!=0 ||
      NData<1)
    {
      ELog::EM<<"Failed to register item :"<<K<<ELog::endErr;
      return;
    }
  const size_t NDef=AItems.size();
  IItemObj<T>* IPtr=new IItemObj<T>(NData,NData-NDef,K,LK);
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));

  for(size_t i=0;i<AItems.size();i++)
    IPtr->setDefObj(NData-NDef+i,AItems[i]);

  return;  
}

template<typename T>
void
inputParam::regDefItem(const std::string& K,const std::string& LK,
		       const size_t NData,const T& AItem)
  /*!
    Registers a particular type
    \param K :: Keyname
    \param LK :: Long keyname
    \param NData :: Number of data points
    \param AItem :: Last Item with default
  */
{
  ELog::RegMethod RegA("inputParam","regDefItem<T>(A)");
  
  if (K.empty() || findShortKey(K)!=0 ||
      NData<1)
    {
      ELog::EM<<"Failed to register item :"<<K<<ELog::endErr;
      return;
    }

  IItemObj<T>* IPtr=new IItemObj<T>(NData,NData-1,K,LK);
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));

  IPtr->setDefObj(NData-1,AItem);
  return;  
}

template<typename T>
void
inputParam::regDefItem(const std::string& K,const std::string& LK,
		       const size_t NData,const T& AItem,const T& BItem)
  /*!
    Registers a particular type
    \param K :: Keyname
    \param LK :: Long keyname
    \param NData :: Number of data points
    \param AItem :: Penultimate Item with default
    \param BItem :: Last Item with default
  */
{
  ELog::RegMethod RegA("inputParam","regDefItem<T>(A,B)");
  
  if (K.empty() || NData<2)
    {
      ELog::EM<<"Failed to register item :"<<K
	      <<" == Points =="<<NData<<ELog::endErr;
      return;
    }
  IItemObj<T>* IPtr=new IItemObj<T>(NData,NData-2,K,LK);
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));

  IPtr->setDefObj(NData-2,AItem);
  IPtr->setDefObj(NData-1,BItem);
  return;  
}

template<typename T>
void
inputParam::regDefItem(const std::string& K,const std::string& LK,
		       const size_t NData,const T& AItem,
		       const T& BItem,const T& CItem)
  /*!
    Registers a particular type
    \param K :: Keyname
    \param LK :: Long keyname
    \param NData :: Number of data points
    \param AItem :: First Item with default
    \param BItem :: Second Item with default
    \param CItem :: Last Item with default
  */
{
  ELog::RegMethod RegA("inputParam","regDefItem<T>(A)");
  
  if (K.empty() || NData<3)
    {
      ELog::EM<<"Failed to register item :"<<K
	      <<" == Points =="<<NData<<ELog::endErr;
      return;
    }
  IItemObj<T>* IPtr=new IItemObj<T>(NData,NData-3,K,LK);
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));

  IPtr->setDefObj(NData-3,AItem);
  IPtr->setDefObj(NData-2,BItem);
  IPtr->setDefObj(NData-1,CItem);
  return;  
}

void
inputParam::processMainInput(std::vector<std::string>& Names)
  /*!
    Process all the main input
    \param Names :: Vector of keys and parameters
  */
{
  ELog::RegMethod RegA("inputParam","processMainInput");

  std::vector<std::string> Out;
  // First search for all keys [use long int as can got to -1 below]
  std::string SubName;
  double DValue;
  for(size_t i=0;i<Names.size();)
    {
      Out.push_back(Names[i]);    // erased later
      IItemBase* IPtr=0;
      const int numberFlag=StrFunc::convert(Names[i],DValue);
      if (Names[i].size()>2 && 
	    Names[i][0]=='-' && Names[i][1]=='-')
	{
	  SubName=Names[i].substr(2);
	  IPtr=findLongKey(SubName);
	}
      else if (!numberFlag && Names[i].size()>1 
	       && Names[i][0]=='-' && Names[i][1]!='-')
	{
	  SubName=Names[i].substr(1);
	  IPtr=findShortKey(SubName);
	}
      // Did we find anything:
      size_t cN(0);
      if (IPtr)       // Index found
	{
	  const size_t Nreq=IPtr->getNReqData();    
	  const size_t Nparam=IPtr->getNData();
	  IPtr->addNewSection();
          i++;
	  // check enough items exist
	  if (i+Nreq <= Names.size())
	    {
	      size_t processCnt(1);
	      while(processCnt && cN<Nparam &&
		    i<Names.size())
		{
		  // returns number of Names[i] used :
		  processCnt=IPtr->convert(cN,i,Names);
		  i+=processCnt;
		  if (processCnt) cN++;   
		}
	    } 

	  if (cN<Nreq)   
	    ELog::EM<<"Item "<<SubName<<" failed at "
		    <<cN<<" ("<<Nparam<<") ["<<Nreq<<"]"<<ELog::endErr;
	  Out.pop_back();
	}
      if (!IPtr)
	i++;
    }
  Names=Out;
  return ;
}

void
inputParam::writeDescription(std::ostream& OX) const
  /*!
    Construct all the keys and items 
    \param OX :: Output stream
   */
{
  boost::format FMTstr(" -%1$s%|20t|%2$s%|50t|%3$s");
  std::vector<std::string> K,N,L;

  MTYPE::const_iterator mc;
  for(mc=Keys.begin();mc!=Keys.end();mc++)
    {
      const IItemBase* IPtr=mc->second;
      OX<<(FMTstr % IPtr->getKey() % IPtr->getLong() % 
	   IPtr->getDesc() )<<std::endl;
    }
  return;
}

void
inputParam::write(std::ostream& OX) const
  /*!
    Construct all the keys and items and values
    \param OX :: Output stream
  */
{
  boost::format FMTstr(" -%1$s%|10t|%2$s%|20t|%3$s");
  MTYPE::const_iterator mc;
  for(mc=Keys.begin();mc!=Keys.end();mc++)
    {
      const IItemBase* IPtr=mc->second;
      OX<<(FMTstr % IPtr->getKey() % IPtr->getLong() % 
	   (IPtr->flag() ? " set " : " not-set "));
      OX<<":: "<<*IPtr<<std::endl;
    }

  return;
}

///\cond TEMPLATE

template void 
inputParam::regItem<int>(const std::string&,const std::string&,
			 const size_t);
template void 
inputParam::regItem<size_t>(const std::string&,const std::string&,
			 const size_t);
template void 
inputParam::regItem<double>(const std::string&,const std::string&,
			    const size_t);
template void 
inputParam::regItem<std::string>(const std::string&,const std::string&,
				 const size_t);
template void 
inputParam::regItem<Geometry::Vec3D>(const std::string&,const std::string&,
				     const size_t);
// MULTI:
template void 
inputParam::regMulti<int>(const std::string&,const std::string&,
			  const size_t,const long int);

template void 
inputParam::regMulti<double>(const std::string&,const std::string&,
			     const size_t,const long int);

template void 
inputParam::regMulti<std::string>(const std::string&,const std::string&,
				  const size_t,const long int);

template void 
inputParam::regMulti<Geometry::Vec3D>(const std::string&,const std::string&,
				      const size_t,const long int);


// DEFAULT : INT
template void 
inputParam::regDefItem<int>(const std::string&,const std::string&,
			    const size_t,const int&);
template void 
inputParam::regDefItem<int>(const std::string&,const std::string&,
			    const size_t,const int&,const int&);
template void 
inputParam::regDefItem<int>(const std::string&,const std::string&,
			    const size_t,const int&,const int&,const int&);
// DEFAULT : double
template void 
inputParam::regDefItem<double>(const std::string&,const std::string&,
			    const size_t,const double&);
template void 
inputParam::regDefItem<double>(const std::string&,const std::string&,
			    const size_t,const double&,const double&);
template void 
inputParam::regDefItem<double>(const std::string&,const std::string&,
			    const size_t,const double&,const double&,
			       const double&);
// DEFAULT : std::string
template void 
inputParam::regDefItem<std::string>
(const std::string&,const std::string&,const size_t,const std::string&);

template void 
inputParam::regDefItem<std::string>
  (const std::string&,const std::string&,
   const size_t,const std::string&,const std::string&);

template void 
inputParam::regDefItem<std::string>
  (const std::string&,const std::string&,const size_t,
   const std::string&,const std::string&,const std::string&);

// DEFAULT : long int
template void 
inputParam::regDefItem<long int>
(const std::string&,const std::string&,const size_t,const long int&);

template void 
inputParam::regDefItem<long int>
  (const std::string&,const std::string&,
   const size_t,const long int&,const long int&);

template void 
inputParam::regDefItem<long int>
  (const std::string&,const std::string&,const size_t,
   const long int&,const long int&,const long int&);


// SET VALUE

template void inputParam::setValue(const std::string&,const double&,const size_t);
template void inputParam::setValue(const std::string&,const int&,const size_t);
template void inputParam::setValue(const std::string&,const 
				   std::string&,const size_t);
template void inputParam::setValue(const std::string&,
				   const Geometry::Vec3D&,const size_t);

template const double& inputParam::getValue(const std::string&,const size_t) const;
template const int& inputParam::getValue(const std::string&,const size_t) const;
template const size_t& inputParam::getValue(const std::string&,const size_t) const;
template const unsigned int& inputParam::getValue(const std::string&,const size_t) const;
template const long int& inputParam::getValue(const std::string&,const size_t) const;
template const std::string& 
inputParam::getValue(const std::string&,const size_t) const;
template const Geometry::Vec3D& 
inputParam::getValue(const std::string&,const size_t) const;

template const Geometry::Vec3D& 
inputParam::getCompValue(const std::string&,const size_t,const size_t) const;
template const std::string&
inputParam::getCompValue(const std::string&,const size_t,const size_t) const;
template const int&
inputParam::getCompValue(const std::string&,const size_t,const size_t) const;

template double
inputParam::getFlagDef(const std::string&,const FuncDataBase& Control,
		       const std::string&,const size_t) const;

template bool inputParam::compValue(const std::string&,
				    const std::string&) const;

template void 
inputParam::regDefItemList(const std::string&,
			   const std::string&,const size_t,
			   const std::vector<int>&);
template void 
inputParam::regDefItemList(const std::string&,
			   const std::string&,const size_t,
			   const std::vector<std::string>&);

///\endcond TEMPLATE
 
}  // NAMESPACE mainSystem
