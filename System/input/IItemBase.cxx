/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   input/IItem.cxx
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
#include <typeinfo>

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
#include "Binary.h"
#include "IItemBase.h"


namespace mainSystem
{

std::ostream&
operator<<(std::ostream& OX,const IItem& A)
  /*!
    Output stream writer
    \param OX :: Output stream
    \param A :: ItemBase Object to write
    \return Stream Obj
  */
{
  A.write(OX);
  return OX;
}

IItem::IItem(const std::string& K) : 
  Key(K),active(0),activeSet(0),activeItem(0),
  maxSets(0),maxItems(0),reqItems(0)
  /*!
    Constructor only with  descriptor
    \param K :: Key Name
  */
{}

IItem::IItem(const std::string& K,const std::string& L) :
  Key(K),Long(L),active(0),activeSet(0),activeItem(0),
  maxSets(0),maxItems(0),reqItems(0)
  /*!
    Full Constructor 
    \param K :: Key Name
    \param L :: Long name
  */
{}

IItem::IItem(const IItem& A) : 
  Key(A.Key),Long(A.Long),Desc(A.Desc),active(A.active),
  activeSet(A.activeSet),activeItem(A.activeItem),
  maxSets(A.maxSets),maxItems(A.maxItems),
  reqItems(A.reqItems),DItems(A.DItems)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

IItem&
IItem::operator=(const IItem& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this != &A)
    {
      Key=A.Key;
      Long=A.Long;
      Desc=A.Desc;
      active=A.active;
      activeSet=A.activeSet;
      activeItem=A.activeItem;
      maxSets=A.maxSets;
      maxItems=A.maxItems;
      reqItems=A.reqItems;
      DItems=A.DItems;
    }
  return *this;
}

void
IItem::setMaxN(const size_t S,const size_t R,const size_t I)
  /*!
    Set max/required list
    \param S :: Max sets
    \param R :: Required items per set
    \param I :: Max Items per set
   */
{
  maxSets=S;
  maxItems=I;
  reqItems=R;
  return;
}

void
IItem::checkIndex(const size_t setIndex,const size_t itemIndex) const
  /*!
    Simple range check for set and item value
    \param  setIndex :: set values
    \param  itemIndex :: item values
   */
{
    if (setIndex >= DItems.size())
    throw ColErr::IndexError<size_t>(setIndex,DItems.size(),Key+":setIndex");

  if (itemIndex>=DItems[setIndex].size())
    throw ColErr::IndexError<size_t>
      (itemIndex,DItems[setIndex].size(),Key+":itemIndex");

  return;
}
  
size_t
IItem::getNSets() const
  /*!
    Number of data sets
    \return number of sets in the model
   */
{
  return DItems.size();
}

size_t
IItem::getNItems(const size_t setIndex) const
  /*!
    Number of items
    \param setIndex :: Index value 
    \return Number of items in a given set
  */
{
  ELog::RegMethod RegA("IItem","getNItems");

  if (setIndex==0  && DItems.empty()) return 0;
  if (setIndex>=DItems.size())
    throw ColErr::IndexError<size_t>(setIndex,DItems.size(),
				     " ["+Key+"] setIndex");
  return DItems[setIndex].size();
}

bool
IItem::isValid(const size_t setIndex) const
  /*!
    Number of data sets
    \return number of sets in the model
   */
{
  ELog::RegMethod RegA("IItem","isValid");
  
  if (setIndex>=DItems.size())
    return 0;

  return (DItems[setIndex].size()<reqItems) ? 0 : 1;
      
}

template<>
void
IItem::setObjItem(const size_t setIndex,const size_t itemIndex,
		  const std::string& V)
  /*!
    Set the object based on the setIndex and the itemIndex 
    Allows a +1 basis but not more:	
    \param setIndex :: Set number
    \param itemIndex :: Item index 
    \param V :: Convertable item
    \return 
  */
{
  ELog::RegMethod RegA("IItem","setObjItem<string>");
  setObj(setIndex,itemIndex,V);
  return;
}

template<typename T>
void
IItem::setObjItem(const size_t setIndex,const size_t itemIndex,
		     const T& V)
  /*!
    Set the object based on the setIndex and the itemIndex 
    Allows a +1 basis but not more:	
    \param setIndex :: Set number
    \param itemIndex :: Item index 
    \param V :: Convertable item
  */
{
  ELog::RegMethod RegA("IItem","setObjItem");

  setObj(setIndex,itemIndex,StrFunc::makeString(V));
  return;
}

void
IItem::setObj(const size_t setIndex,const size_t itemIndex,
	      const std::string& V)
  /*!
    Set the object based on the setIndex and the itemIndex 
    Allows a +1 basis but not more:	
    \param setIndex :: group set number
    \param itemIndex :: Item number
    \param V :: String to be converted
  */
{
  ELog::RegMethod RegA("IItem","setObj");
  const size_t SS(DItems.size());

  if (setIndex>=maxSets || setIndex>SS+1)
    throw ColErr::IndexError<size_t>(setIndex,DItems.size(),Key+"::setIndex");
  if (setIndex==SS)
    DItems.push_back(std::vector<std::string>());

  const size_t IS(DItems[setIndex].size());


  if (itemIndex>=maxItems || itemIndex>IS+1)
    {
      ELog::EM<<"Max = "<<maxItems<<ELog::endDiag;
      throw ColErr::IndexError<size_t>(itemIndex,DItems[setIndex].size(),
				     Key+"::itemIndex");
    }
  if (itemIndex==IS)
    DItems[setIndex].push_back(V);
  else
    DItems[setIndex][itemIndex]=V;

  return;
}

void
IItem::setObj(const size_t itemIndex,const std::string& V)
  /*!
    Set the object based on the setIndex and the itemIndex 
    Allows a +1 basis but not more:	
    \param itemIndex :: Item number
    \param V :: Value to set
  */
{
  setObj(0,itemIndex,V);
  return;
}

void
IItem::setObj(const std::string& V)
  /*!
    Set the object based on 0,0 time
    \param V :: Value to set
  */
{
  setObj(0,0,V);
  return;
}


const std::vector<std::string>&
IItem::getObjectItems(const size_t setIndex) const
  /*!
    Get Object [assuming setIndex/itemIndex ==0]
    \param setIndex :: set value
    \return vector of strings 
  */
{ 
  ELog::RegMethod RegA("IItem","getObjectItems");

  if (setIndex >= DItems.size())
    throw ColErr::IndexError<size_t>(setIndex,DItems.size(),Key+":setIndex");

  return DItems[setIndex];
}

  
template<typename T>
T
IItem::getObj() const
  /*!
    Get Object [assuming setIndex/itemIndex ==0]
    \return Object
  */
{ 
  ELog::RegMethod RegA("IItem","getObj(int)");
  return getObj<T>(0,0);
}

template<typename T>
T
IItem::getObj(const size_t itemIndex) const
  /*!
    Get Object [assuming  setIndex [0]]]
    \param itemIndex :: item count
    \return Object
  */
{ 
  ELog::RegMethod RegA("IItem","getObj(int)");

  return getObj<T>(0,itemIndex);
}

template<typename T>
T
IItem::getObj(const size_t setIndex,const size_t itemIndex) const
  /*!
    Get Object
    \param setIndex :: Index
    \param itemIndex :: item count
    \return Object
  */
{ 
  ELog::RegMethod RegA("IItem","getObj");

  if (setIndex >= DItems.size())
    throw ColErr::IndexError<size_t>(setIndex,DItems.size(),Key+":setIndex");

  if (itemIndex>=DItems[setIndex].size())
    throw ColErr::IndexError<size_t>
      (itemIndex,DItems[setIndex].size(),Key+":itemIndex");

  T ObjValue;
  if (!StrFunc::convert(DItems[setIndex][itemIndex],ObjValue))
    throw ColErr::TypeMatch(DItems[setIndex][itemIndex],
				  typeid(T).name(),Key+":convert error");

  return ObjValue;
}

template<>
Geometry::Vec3D
IItem::getObj(const size_t setIndex,const size_t itemIndex) const
  /*!
    Get Object
    \param setIndex :: Index
    \param itemIndex :: item count
    \return Vec3D object
  */
{ 
  ELog::RegMethod RegA("IItem","getObj");

  checkIndex(setIndex,itemIndex);
  
  Geometry::Vec3D Value;
  if (StrFunc::convert(DItems[setIndex][itemIndex],Value))
    return Value;

  if (itemIndex+3>DItems[setIndex].size() ||
      !StrFunc::convert(DItems[setIndex][itemIndex],Value[0]) ||
      !StrFunc::convert(DItems[setIndex][itemIndex+1],Value[1]) ||
      !StrFunc::convert(DItems[setIndex][itemIndex+2],Value[2]) )
    throw ColErr::TypeMatch(DItems[setIndex][itemIndex],
				  "Geomtery::Vec3D",Key+":convert error");

  return Value;
}

  
template<>
std::string
IItem::getObj(const size_t setIndex,const size_t itemIndex) const
  /*!
    Get Object
    \param setIndex :: Index
    \param itemIndex :: item count
    \return string value [as stored]
  */
{ 
  ELog::RegMethod RegA("IItem","getObj<string>");

  checkIndex(setIndex,itemIndex);
  return DItems[setIndex][itemIndex];
}


Geometry::Vec3D
IItem::getCntVec3D(const size_t setIndex,size_t& itemIndex) const
  /*!
    Get a simple Vec3D [cannot be a link component]
    \param setIndex :: Index
    \param itemIndex :: item count
    \return Vec3D if valid 
  */
{ 
  ELog::RegMethod RegA("IItem","getCntVec3D");

  if (setIndex >= DItems.size())
    throw ColErr::IndexError<size_t>(setIndex,DItems.size(),Key+":setIndex");

  const size_t NItems=DItems[setIndex].size();
  if (itemIndex>=NItems)
    throw ColErr::IndexError<size_t>
      (itemIndex,NItems,Key+":itemIndex");

  Geometry::Vec3D Value;
  if (StrFunc::convert(DItems[setIndex][itemIndex],Value))
    {
      itemIndex++;
      return Value;
    }

  if (itemIndex+3>NItems ||
      !StrFunc::convert(DItems[setIndex][itemIndex],Value[0]) ||
      !StrFunc::convert(DItems[setIndex][itemIndex+1],Value[1]) ||
      !StrFunc::convert(DItems[setIndex][itemIndex+2],Value[2]) )
    throw ColErr::TypeMatch(DItems[setIndex][itemIndex],
				  "Geomtery::Vec3D",Key+":convert error");

  itemIndex+=3;
  return Value;
}
  
size_t
IItem::addSet()
  /*!
    Adds V to the last set items
    \return setSize on success  / 0 on failure
  */
{
  ELog::RegMethod RegA("IItem","addSet");
  
  if (!active && maxSets)
    activeSet=0;
  else
    activeSet++;

  // ALL STUFF IS MADE ACTIVE to include flags
  activeItem=0;
  active=1;

  if (activeSet>=maxSets)
    return 0;
  
  if (DItems.size()<=activeSet)
    DItems.push_back(std::vector<std::string>());
  return activeSet+1;
}

bool
IItem::addObject(const std::string& V)
  /*!
    Adds V to the last set items
    \param V :: Item to add
    \return 1 on success / 0 on failure
  */
{
  ELog::RegMethod RegA("IItem","addObject");

  if (activeSet>=maxSets || V.empty() ||
      activeItem>=maxItems)
    return 0;

  if (activeItem>=DItems[activeSet].size())
    DItems[activeSet].push_back(V);
  else
    DItems[activeSet][activeItem]=V;
  activeItem++;
  return 1;
}


void
IItem::writeSet(std::ostream& OX,const size_t setIndex) const
  /*!
    Complex functiion to convert the system into a string
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("IItem","writeSet");

  if (setIndex>=DItems.size())
    throw ColErr::IndexError<size_t>(setIndex,DItems.size(),
				     " ["+Key+"] setIndex");
  
  for(const std::string& Item : DItems[setIndex])
    OX<<Item<<" ";

  return;
}

void
IItem::write(std::ostream& OX) const
  /*!
    Complex function to convert the system into a string
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("IItem","write");

  for(size_t i=0;i<DItems.size();i++)
    {
      if (i) OX<<"\n";
      for(size_t j=0;j<DItems[i].size();j++)
	{
	  if (j) OX<<" ";
	  OX<<DItems[i][j];
	}
    }
  return;
}


///\cond TEMPLATE

template int IItem::getObj(const size_t,const size_t) const;
template unsigned int IItem::getObj(const size_t,const size_t) const;
template double IItem::getObj(const size_t,const size_t) const;
template size_t IItem::getObj(const size_t,const size_t) const;
template long int IItem::getObj(const size_t,const size_t) const;

template std::string IItem::getObj(const size_t) const;
template int IItem::getObj(const size_t) const;
template unsigned int IItem::getObj(const size_t) const;
template double IItem::getObj(const size_t) const;
template size_t IItem::getObj(const size_t) const;
template long int IItem::getObj(const size_t) const;
template Geometry::Vec3D IItem::getObj(const size_t) const;


template void
IItem::setObjItem(const size_t,const size_t,const double&);
template void
IItem::setObjItem(const size_t,const size_t,const int&);
template void
IItem::setObjItem(const size_t,const size_t,const long int&);
template void
IItem::setObjItem(const size_t,const size_t,const size_t&);
template void
IItem::setObjItem(const size_t,const size_t,const Geometry::Vec3D&);


///\endcond TEMPLATE
 
}  // NAMESPACE mainSystem


