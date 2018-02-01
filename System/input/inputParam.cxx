/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   input/inputParam.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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

#include <boost/algorithm/string.hpp>  // lower / upper
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
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

  for(MTYPE::value_type MC : Keys)
    {
      IItem* IObj=new IItem(*MC.second);
      Keys.insert(MTYPE::value_type(MC.first,IObj));
      if (A.Names.find(IObj->getLong())!=A.Names.end())
	Names.insert(MTYPE::value_type(IObj->getLong(),IObj));
    }
  return;
}

void
inputParam::checkKeys(const std::string& K,const std::string& LK) const
  /*!
    Simple check to see if key in use
    \param K :: Short key
    \param LK :: Long key if given
  */
{
  if (K.empty() || findShortKey(K)!=0)
    throw ColErr::InContainerError<std::string>(K,"Key[K] in use");
  if (findLongKey(K)!=0)  
    throw ColErr::InContainerError<std::string>(LK,"Long Key[LK] in use");
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
  
IItem*
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

const IItem*
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

const IItem*
inputParam::findLongKey(const std::string& K) const
  /*!
    Given a key find the Item  Does not-throw
    \param K :: Key value (long)
    \return Ptr / 0 on failure
   */
{
  if (K.empty()) return 0;
  MTYPE::const_iterator mc;
  mc=Names.find(K);
  return (mc!=Names.end()) ? mc->second : 0;
}

const IItem*
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

const IItem*
inputParam::findKey(const std::string& K) const
  /*!
    Given a key find the Item
    Does not-throw
    \param K :: Key value (short)
    \return Ptr / 0 on failure
   */
{
  const IItem* OutPtr=findShortKey(K);
  return (!OutPtr) ? findLongKey(K) : OutPtr;
}

IItem*
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

IItem*
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

IItem*
inputParam::findKey(const std::string& K) 
  /*!
    Given a key find the Item
    Does not-throw
    \param K :: Key value (short)
    \return Ptr / 0 on failure
   */
{
  IItem* OutPtr=findShortKey(K);
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
    Determine number of data object for a given key [assuming set 0]
    \param K :: Key to seach
    \return active flag
   */
{
  ELog::RegMethod RegA("inputParam","dataCnt");
  
  const IItem* IPtr=getIndex(K);
  
  return IPtr->getNItems();
}

size_t
inputParam::setCnt(const std::string& K) const
  /*!
    Count number of groups
    \param K :: Key to search
    \return Grp count 
   */
{
  ELog::RegMethod RegA("inputParam","setCnt");
  
  const IItem* IPtr=getIndex(K);
  return IPtr->getNSets();
}


size_t
inputParam::itemCnt(const std::string& K,const size_t setIndex) const
  /*!
    Determine number of items in a specific group
    \param K :: Key to seach
    \param setIndex :: Nubmer of entry point
    \return item count
   */
{
  ELog::RegMethod RegA("inputParam","itemCnt");
  const IItem* IPtr=getIndex(K);
  return IPtr->getNItems(setIndex);
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

  const IItem* IPtr=getIndex(K);
  return IPtr->flag();
}

template<typename T>
std::set<T>
inputParam::getComponents(const std::string& K,
                          const size_t index) const
  /*!
    Find all the values for a given key
    \param K :: Key name
    \param index :: index item
    \return set of items [as string]
  */
{
  ELog::RegMethod RegA("inputParam","getComponents");

  std::set<T> Out;
  const IItem* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>(K+":IPtr");

  const size_t N=IPtr->getNSets();
  
  for(size_t i=0;i<N;i++)
    {
      const size_t NI=IPtr->getNItems(i);
      if (NI>=index)
        Out.insert(IPtr->getObj<T>(i,index));
    }
      
  return Out;
}
  
template<typename T>
T
inputParam::getFlagDef(const std::string& InpKey,
		       const FuncDataBase& Control,
		       const std::string& VarKey,  
		       const size_t itemIndex) const
  /*!
    Get a value based initially on the input line,
    and to resolve to the variable.
    \param InpKey :: Key to seach
    \param Control :: FuncDataBase to search 
    \param VarKey :: Variable Key
    \param itemIndex :: Index value
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getFlagDef");

  const IItem* IPtr=findKey(InpKey);
  
  
  if (IPtr && IPtr->getNItems(0)>itemIndex)
    return IPtr->getObj<T>(itemIndex);
  
  // Fall back to var set
  if (!Control.hasVariable(VarKey))
    throw ColErr::InContainerError<std::string>(InpKey+":"+VarKey,
						"InpKey:VarKey");

  return Control.EvalVar<T>(VarKey);
}

std::string
inputParam::getFull(const std::string& K,const size_t setIndex) const
  /*!
    Get the full string of all options for a given setindex
    \param K :: Key to seach
    \param setIndex :: Index value
    \return Values in spc separated list
   */
{
  ELog::RegMethod RegA("inputParam","getFull");

  const IItem* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>("Key : "+K);

  std::ostringstream OX;
  IPtr->writeSet(OX,setIndex);
  return OX.str();
}

template<typename T>
T
inputParam::getDefValue(const T& DefVal,
			const std::string& K,
			const size_t itemIndex) const
  /*!
    Get a value based on key
    \param DefVal :: Default value
    \param K :: Key to seach
    \param itemIndex :: Index value
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getDefValue(index)");
  return getDefValue<T>(DefVal,K,0,itemIndex);
}

template<typename T>
T
inputParam::getDefValue(const T& DefVal,
			const std::string& K,
			const size_t setIndex,
			const size_t itemIndex) const
  /*!
    Get a value based on key.
    Note: that this still throws if it is impossible to convert
    the value into the correct type
    \param DefVal :: Default value to return 
    \param K :: Key to seach
    \param setIndex :: set Value
    \param itemIndex :: Index value
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getDefValue(setIndex,index)");

  const IItem* IPtr=getIndex(K);  
  if (!IPtr) return DefVal;
  
  const size_t NItems=IPtr->getNItems(setIndex);
  
  return (NItems>itemIndex) ?
    IPtr->getObj<T>(setIndex,itemIndex) : DefVal;
}
  
template<typename T>
T
inputParam::getValue(const std::string& K,
		     const size_t itemIndex) const
  /*!
    Get a value based on key
    \param K :: Key to seach
    \param itemIndex :: Index value
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getValue(index)");
  return getValue<T>(K,0,itemIndex);
}

template<typename T>
T
inputParam::getValue(const std::string& K,
		     const size_t setIndex,
		     const size_t itemIndex) const
  /*!
    Get a value based on key
    \param K :: Key to seach
    \param setIndex :: set Value
    \param itemIndex :: Index value
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getValue(setIndex,index)");

  const IItem* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>(K+":IPtr");
  return IPtr->getObj<T>(setIndex,itemIndex);
}
  

  
template<typename T>
T
inputParam::getValueError(const std::string& K,
			  const size_t setIndex,
			  const size_t itemIndex,
			  const std::string& errMessage) const
  /*!
    Get a value based on key
    \param K :: Key to seach
    \param setIndex :: set Value
    \param itemIndex :: Index value
    \param errMessage :: error message if failure
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getValueError(setIndex,index)");
  RegA.setTrack(errMessage);  
  return getValue<T>(K,setIndex,itemIndex);
}

Geometry::Vec3D
inputParam::getCntVec3D(const std::string& K,
			const size_t setIndex,
			size_t& itemIndex,
			const std::string& ErrMessage) const
  /*!
    Get a value based on key
    \param K :: Key to seach
    \param setIndex :: set Value
    \param itemIndex :: Index value [updated by 3/1]
    \param ErrMessage :: Error message
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getCntVec3D(Err)");
  RegA.setTrack(ErrMessage);  
  return getCntVec3D(K,setIndex,itemIndex);
}

Geometry::Vec3D
inputParam::getCntVec3D(const std::string& K,
			const size_t setIndex,
			size_t& itemIndex) const
  /*!
    Get a value based on key
    \param K :: Key to seach
    \param setIndex :: set Value
    \param itemIndex :: Index value [updated by 3/1]
    \return Value
   */
{
  ELog::RegMethod RegA("inputParam","getCntVec3D(setIndex,index)");
  const IItem* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>(K+":IPtr");
  
  return IPtr->getCntVec3D(setIndex,itemIndex);
}

std::vector<std::string>
inputParam::getAllItems(const std::string& K) const
  /*!
    Accessor to the whole raw string
    \param K :: Key to seach
    \return Set of raw-strings
  */
{
  ELog::RegMethod Rega("inputParam","getAllItems");

  
  const IItem* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>(K+":IPtr");
  const size_t nSet=IPtr->getNSets();

  std::vector<std::string> Out;
  for(size_t index=0;index<nSet;index++)
    {
      const std::vector<std::string>& IVec=
	IPtr->getObjectItems(index);
      Out.insert(Out.end(),IVec.begin(),IVec.end());
    }
  
  return Out;
}

const std::vector<std::string>&
inputParam::getObjectItems(const std::string& K,
                           const size_t setIndex) const
  /*!
    Accessor to the raw string
    \param K :: Key to seach
    \param setIndex :: set Value
    \return Set of raw-strings
  */
{
  ELog::RegMethod Rega("inputParam","getObjectItems");

  const IItem* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>(K+":IPtr");
  
  return IPtr->getObjectItems(setIndex);
}

  
template<typename T>
T
inputParam::outputItem(const std::string& K,
		       const size_t setIndex,
		       const size_t itemIndex,
		       const std::string& ErrMessage) const
  /*!
    Get a value based on key
    \param K :: Key to seach
    \param setIndex :: set Value
    \param itemIndex :: Index value
    \param ErrMessage :: Error message for exception
    \return Value
*/

{
  ELog::RegMethod RegA("inputParam","outputItem(setIndex,index)");

  const IItem* IPtr=getIndex(K);
  if (!IPtr) return 0;
  try
    {
      return IPtr->getObj<T>(setIndex,itemIndex);
    }
  catch(ColErr::IndexError<size_t>&)
    {
    }
  throw ColErr::InContainerError<std::string>
    (K+"["+StrFunc::makeString(setIndex)+"]::"+
     StrFunc::makeString(itemIndex),ErrMessage);
}

template<typename T>
T
inputParam::outputDefItem(const std::string& K,
			  const size_t setIndex,
			  size_t& itemIndex,
			  const T& DefValue) const
  /*!
    Get a value based on key
    \param K :: Key to seach
    \param setIndex :: set Value
    \param itemIndex :: Index value [updated]
    \param DefValue :: Default Value
    \return Value
*/

{
  ELog::RegMethod RegA("inputParam","outputItem(setIndex,index)");

  const IItem* IPtr=getIndex(K);
  if (!IPtr) return DefValue;

  try
    {
      const T Out=IPtr->getObj<T>(setIndex,itemIndex);
      itemIndex++;
      return Out;
    }
  catch(ColErr::IndexError<size_t>&)
    { }
  return DefValue;
}

template<typename T>
int
inputParam::checkItem(const std::string& K,
		      const size_t setIndex,
		      const size_t itemIndex,
		      T& Out) const
 /*!
   Get a value based on key
   \param K :: Key to seach
   \param setIndex :: set Value
   \param itemIndex :: Index value
   \param Out :: returned value if found
   \return 1 on success / 0 on failure
 */

{
  ELog::RegMethod RegA("inputParam","checkItem(setIndex,index)");

  const IItem* IPtr=getIndex(K);
  if (!IPtr) return 0;
  try
    {
      Out=IPtr->getObj<T>(setIndex,itemIndex);
    }
  catch(ColErr::IndexError<size_t>&)
    {
      return 0;
    }
  return 1;
}

int
inputParam::checkCntVec3D(const std::string& K,
			  const size_t setIndex,
			  size_t& itemIndex,
			  Geometry::Vec3D& Out) const
  /*!
    Get a value based on key
    \param K :: Key to seach
    \param setIndex :: set Value
    \param itemIndex :: Index value
    \param Out :: returned value if found
    \return 1 on success / 0 on failure
  */

{
  ELog::RegMethod RegA("inputParam","checkCntVec3(setIndex,index)");

  const IItem* IPtr=getIndex(K);
  if (!IPtr) return 0;
  try
    {
      Out=IPtr->getCntVec3D(setIndex,itemIndex);
    }
  catch(ColErr::IndexError<size_t>&)
    {
      return 0;
    }
  return 1;
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
  
  const IItem* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>("Key failed: "+K);

  const size_t N=IPtr->getNItems();
  std::string NCValue(Value);
  boost::algorithm::to_lower(NCValue);
  for(size_t i=0;i<N;i++)
    {
      std::string OStr=IPtr->getObj<std::string>(i); 
      boost::algorithm::to_lower(OStr);      
      if (NCValue==OStr)
	return 1;
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

  const IItem* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::EmptyValue<void>("Key failed: "+K);
  const size_t N=IPtr->getNItems();
  const std::string NCValue(StrFunc::makeString(Value));

  for(size_t i=0;i<N;i++)
    if (IPtr->getObj<T>(i)==Value)
      return 1;
  
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
  ELog::RegMethod RegA("inputParam","setDesc");

  IItem* IPtr=getIndex(K);
  std::string::size_type pos=D.find('\n');
  if (pos!=std::string::npos)
    {
      std::string Out(D);
      do
	{
	  Out.replace(pos,1,"\n          ");
	  pos=Out.find('\n',pos+10);
	}
      while(pos!=std::string::npos);
      IPtr->setDesc(Out);
    }
  else
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

  IItem* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::InContainerError<std::string>(K,"Key");

  IPtr->setActive();
  return;
}

template<typename T>
void
inputParam::setValue(const std::string& K,
		     const T& A,const size_t itemIndex) 
  /*!
    Set a value based on key
    \param K :: Key to add/search
    \param A :: Object to set
    \param itemIndex :: Index value
  */
{
  setValue<T>(K,A,0,itemIndex);
  return;
}

template<typename T>
void
inputParam::setValue(const std::string& K,
		     const T& A,const size_t setIndex,
		     const size_t itemIndex) 
  /*!
    Set a value based on key
    \param K :: Key to add/search
    \param A :: Object to set
    \param setIndex :: index value to set
    \param itemIndex :: Index value

  */
{
  ELog::RegMethod RegA("inputParam","setValue");
  
  IItem* IPtr=getIndex(K);
  if (!IPtr)
    throw ColErr::InContainerError<std::string>(K,"Key Failed :");

  std::ostringstream cx;
  cx<<A;
  IPtr->setObj(setIndex,itemIndex,cx.str());
  return;
}

void
inputParam::setMultiValue(const std::string& K,
			  const size_t setIndex,
			  const std::string& Values) 
 /*!
    Set a value based on key
    \param K :: Key to add/search
    \param setIndex :: Index to multi-unit
    \param Values :: String of values
  */
{
  ELog::RegMethod RegA("inputParam","setMultValue");
  IItem* IPtr=getIndex(K);  // throws

  const std::vector<std::string> Items=
    StrFunc::splitParts(Values,' ');
  for(size_t i=0;i<Items.size();i++)
    IPtr->setObj(setIndex,i,Items[i]);
  
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
  checkKeys(K,LK);

  IItem* IPtr=new IItem(K,LK);
  IPtr->setMaxN(0,0,0);
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));
  return;  
}

void
inputParam::regItem(const std::string& K,
		    const std::string& LK,
		    const size_t ReqData,
		    const size_t MaxData)
  /*!
    Registers a particular type
    \param K :: Keyname
    \param LK :: Long keyname
    \param ReqData :: Required Number of data points [min]
    \param MaxData :: Max number of data 
  */
{
  ELog::RegMethod RegA("inputParam","regItem");
  checkKeys(K,LK);

  IItem* IPtr=new IItem(K,LK);
  IPtr->setMaxN(1,ReqData,MaxData);
  Keys.insert(MTYPE::value_type(K,IPtr));

  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));
  return;  
}

void
inputParam::regMulti(const std::string& K,const std::string& LK,
		     const size_t maxSets,const size_t reqData,
		     const size_t maxData)
  /*!
    Registers a particular Multi type
    \param K :: Keyname
    \param LK :: Long keyname
    \param maxSets :: Max number of sets
    \param reqData :: Required data per set
    \param maxData :: Max number of data per set
  */
{
  ELog::RegMethod RegA("inputParam","regMulti");
  checkKeys(K,LK);

  IItem* IPtr=new IItem(K,LK);
  IPtr->setMaxN(maxSets,reqData,maxData);
  
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));
  return;  
}


template<typename T>
void
inputParam::regDefItemList(const std::string& K,const std::string& LK,
			   const size_t reqNData,const std::vector<T>& AItems)
  /*!
    Registers a particular type
    \param K :: Keyname
    \param LK :: Long keyname
    \param reqNData :: Number of data points
    \param AItems :: Defautl item list
  */
{
  ELog::RegMethod RegA("inputParam","regDefItemList<T>");
  checkKeys(K,LK);
  
  if (reqNData>AItems.size())
    throw ColErr::IndexError<size_t>(reqNData,AItems.size(),
				     "Items and required size incompatable");
  
  IItem* IPtr=new IItem(K,LK);
  IPtr->setMaxN(1,0,10000);            // no required as has def.
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));

  for(size_t i=0;i<AItems.size();i++)
    IPtr->setObjItem<T>(0,i,AItems[i]);

  return;  
}

template<typename T>
void
inputParam::regDefItem(const std::string& K,const std::string& LK,
		       const size_t reqItem,const T& AItem)
  /*!
    Registers a particular type
    \param K :: Keyname
    \param LK :: Long keyname
    \param reqItem :: Number of data points require
    \param AItem :: Last Item with default
  */
{
  ELog::RegMethod RegA("inputParam","regDefItem<T>(A)");
  checkKeys(K,LK);

  IItem* IPtr=new IItem(K,LK);
  IPtr->setMaxN(1,0,reqItem);

  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));

  for(size_t i=0;i<reqItem;i++)
    IPtr->setObjItem<T>(0,i,AItem);
  return;  
}

template<typename T>
void
inputParam::regDefItem(const std::string& K,const std::string& LK,
		       const size_t reqData,const T& AItem,
		       const T& BItem)
  /*!
    Registers a particular type
    \param K :: Keyname
    \param LK :: Long keyname
    \param reqData :: Number of data points
    \param AItem :: Penultimate Item with default
    \param BItem :: Last Item with default
  */
{
  ELog::RegMethod RegA("inputParam","regDefItem<T>(A,B)");
  checkKeys(K,LK);
  if (reqData<2)
    throw ColErr::IndexError<size_t>(reqData,2,"reqData in:"+K);

  IItem* IPtr=new IItem(K,LK);
  IPtr->setMaxN(1,0,2);
  
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));

  IPtr->setObjItem<T>(0,0,AItem);
  IPtr->setObjItem<T>(0,1,BItem);

  return;  
}

template<typename T>
void
inputParam::regDefItem(const std::string& K,const std::string& LK,
		       const size_t reqData,const T& AItem,
		       const T& BItem,const T& CItem)
  /*!
    Registers a particular type
    \param K :: Keyname
    \param LK :: Long keyname
    \param reqData :: Number of data points
    \param AItem :: Penultimate Item with default
    \param BItem :: Last Item with default
    \param CItem :: Last Item with default
  */
{
  ELog::RegMethod RegA("inputParam","regDefItem<T>(A,B,C)");
  checkKeys(K,LK);
  if (reqData<3)
    throw ColErr::IndexError<size_t>(reqData,3,"reqData");

  IItem* IPtr=new IItem(K,LK);
  IPtr->setMaxN(1,0,3);
  Keys.insert(MTYPE::value_type(K,IPtr));
  if (!LK.empty())
    Names.insert(MTYPE::value_type(LK,IPtr));

  IPtr->setObjItem<T>(0,0,AItem);
  IPtr->setObjItem<T>(0,1,BItem);
  IPtr->setObjItem<T>(0,2,CItem);

  return;  
}


void
inputParam::processMainInput(std::vector<std::string>& Names)
  /*!
    Process all the main input. Populate pre-defined object.
    Those imtes/values in Name that are processed correctly are
    removed.
    \param Names :: Vector of keys and parameters
  */
{
  ELog::RegMethod RegA("inputParam","processMainInput");

  std::vector<std::string> Out;
  // First search for all keys [use long int as can got to -1 below]
  std::string SubName;
  double DValue;              // Dumy value to avoid checking -10.0 etc
  for(size_t i=0;i<Names.size();)
    {
      Out.push_back(Names[i]);    // erased later
      
      IItem* IPtr=0;
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
      if (IPtr)       // Index found
	{
	  const size_t NReq=IPtr->getReqItems();
	  const size_t NMax=IPtr->getMaxItems();
	  size_t processNumber(0);
	  if (i+NReq <= Names.size())
	    {
	      i++;
	      // add set if new:
	      const size_t SNum=IPtr->addSet();
	      size_t nextFlag(SNum);
	      while(nextFlag && i<Names.size())
		{
		  nextFlag=0;
		  // check if a -ve number
		  if (Names[i][0]!='-' ||
		      StrFunc::convert(Names[i],DValue))
		    {
		      nextFlag=IPtr->addObject(Names[i]);
		      if (nextFlag)
			{
			  processNumber++;
			  i++;
			}
		    }
		}
	    }	  
	  if (processNumber<NReq)   
	    ELog::EM<<"Item "<<SubName<<" failed at "
		    <<processNumber<<" ("<<NMax<<") ["<<NReq<<"]"<<ELog::endErr;
	  Out.pop_back(); // All good so remove Items
	}
      else 
	i++;
    }
  
  Names=Out;
  return;
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
      const IItem* IPtr=mc->second;
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
      const IItem* IPtr=mc->second;
      OX<<(FMTstr % IPtr->getKey() % IPtr->getLong() % 
	   (IPtr->flag() ? " set " : " not-set "));
      OX<<":: "<<*IPtr<<"\n";
    }

  return;
}

///\cond TEMPLATE

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
template void inputParam::setValue(const std::string&,const std::string&,const size_t);
template void inputParam::setValue(const std::string&,const Geometry::Vec3D&,const size_t);

template void inputParam::setValue(const std::string&,const double&,const size_t,const size_t);
template void inputParam::setValue(const std::string&,const int&,const size_t,const size_t);
template void inputParam::setValue(const std::string&,const std::string&,const size_t,const size_t);
template void inputParam::setValue(const std::string&,const Geometry::Vec3D&,const size_t,const size_t);

template double inputParam::getDefValue
  (const double&,const std::string&,const size_t) const;
template int inputParam::getDefValue
  (const int&,const std::string&,const size_t) const;
template size_t inputParam::getDefValue
  (const size_t&,const std::string&,const size_t) const;
template unsigned int inputParam::getDefValue
  (const unsigned int&,const std::string&,const size_t) const;
template long int inputParam::getDefValue
  (const long int&,const std::string&,const size_t) const;
template std::string inputParam::getDefValue
  (const std::string&,const std::string&,const size_t) const;
template Geometry::Vec3D inputParam::getDefValue
  (const Geometry::Vec3D&,const std::string&,const size_t) const;
  
template double inputParam::getValue(const std::string&,const size_t) const;
template int inputParam::getValue(const std::string&,const size_t) const;
template size_t inputParam::getValue(const std::string&,const size_t) const;
template unsigned int inputParam::getValue(const std::string&,const size_t) const;
template long int inputParam::getValue(const std::string&,const size_t) const;
template std::string inputParam::getValue(const std::string&,const size_t) const;
template Geometry::Vec3D inputParam::getValue(const std::string&,const size_t) const;

template double inputParam::getValue(const std::string&,const size_t,const size_t) const;
template int inputParam::getValue(const std::string&,const size_t,const size_t) const;
template size_t inputParam::getValue(const std::string&,const size_t,const size_t) const;
template unsigned int inputParam::getValue(const std::string&,const size_t,const size_t) const;
template long int inputParam::getValue(const std::string&,const size_t,const size_t) const;
template std::string inputParam::getValue(const std::string&,const size_t,const size_t) const;
template Geometry::Vec3D inputParam::getValue(const std::string&,const size_t,const size_t) const;


  
template double inputParam::getValueError(const std::string&,const size_t,const size_t,const std::string&) const;
template int inputParam::getValueError(const std::string&,const size_t,const size_t,const std::string&) const;
template size_t inputParam::getValueError(const std::string&,const size_t,const size_t,const std::string&) const;
template unsigned int inputParam::getValueError(const std::string&,const size_t,const size_t,const std::string&) const;
template long int inputParam::getValueError(const std::string&,const size_t,const size_t,const std::string&) const;
template std::string inputParam::getValueError(const std::string&,const size_t,const size_t,const std::string&) const;
template Geometry::Vec3D inputParam::getValueError(const std::string&,const size_t,const size_t,const std::string&) const;


template double inputParam::getDefValue
  (const double&,const std::string&,const size_t,const size_t) const;
template int inputParam::getDefValue
  (const int&,const std::string&,const size_t,const size_t) const;
template size_t inputParam::getDefValue
(const size_t&,const std::string&,const size_t,const size_t) const;
template unsigned int inputParam::getDefValue
  (const unsigned int&,const std::string&,const size_t,const size_t) const;
template long int inputParam::getDefValue
  (const long int&,const std::string&,const size_t,const size_t) const;
template std::string inputParam::getDefValue
  (const std::string&,const std::string&,const size_t,const size_t) const;
template Geometry::Vec3D inputParam::getDefValue
  (const Geometry::Vec3D&,const std::string&,const size_t,const size_t) const;

  

template int inputParam::checkItem(const std::string&,const size_t,
				   const size_t,double&) const;
template int inputParam::checkItem(const std::string&,const size_t,
				   const size_t,int&) const;
template int inputParam::checkItem(const std::string&,const size_t,
				   const size_t,size_t&) const;
 template int inputParam::checkItem(const std::string&,const size_t,
				   const size_t,std::string&) const;
  
template double
inputParam::outputItem(const std::string&,const size_t,
		       const size_t,const std::string&) const;
template int
inputParam::outputItem(const std::string&,const size_t,
		       const size_t,const std::string&) const;
template size_t
inputParam::outputItem(const std::string&,const size_t,
		       const size_t,const std::string&) const;
template long int
inputParam::outputItem(const std::string&,const size_t,
		       const size_t,const std::string&) const;
template std::string
inputParam::outputItem(const std::string&,const size_t,
		       const size_t,const std::string&) const;
  

template double
inputParam::outputDefItem(const std::string&,const size_t,
			  size_t&,const double&) const;
template int
inputParam::outputDefItem(const std::string&,const size_t,
			  size_t&,const int&) const;
template long int
inputParam::outputDefItem(const std::string&,const size_t,
			  size_t&,const long int&) const;
template size_t
inputParam::outputDefItem(const std::string&,const size_t,
			  size_t&,const size_t&) const;


template double
inputParam::getFlagDef(const std::string&,const FuncDataBase& Control,
		       const std::string&,const size_t) const;


template bool
inputParam::compValue(const std::string&,const size_t&) const;

template bool
inputParam::compValue(const std::string&,
		      const std::string&) const;

template void 
inputParam::regDefItemList(const std::string&,
			   const std::string&,const size_t,
			   const std::vector<int>&);
template void 
inputParam::regDefItemList(const std::string&,
			   const std::string&,const size_t,
			   const std::vector<std::string>&);

template std::set<std::string>
inputParam::getComponents(const std::string&,const size_t) const;
                           
///\endcond TEMPLATE
 
}  // NAMESPACE mainSystem
