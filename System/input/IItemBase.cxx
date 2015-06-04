/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   input/IItem.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
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
  Key(K),maxSets(0),maxItems(0),reqItems(0)
  /*!
    Constructor only with  descriptor
    \param K :: Key Name
  */
{}

IItem::IItem(const std::string& K,const std::string& L) :
  Key(K),Long(L),maxSets(0),maxItems(0),reqItems(0)
  /*!
    Full Constructor 
    \param K :: Key Name
    \param L :: Long name
  */
{}

IItem::IItem(const IItem& A) : 
  Key(A.Key),Long(A.Long),Desc(A.Desc),active(0),
  maxSets(A.maxSets),maxItems(A.maxItems),reqItems(A.reqItems),
  DItems(A.DItems)
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
      maxSets=A.maxSets;
      maxItems=A.maxItems;
      reqItems=A.reqItems;
      DItems=A.DItems;
    }
  return *this;
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
  if (DItems.size()>=setIndex)
    throw ColErr::IndexError<size_t>(setIndex,DItems.size(),"setIndex");
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
  if (DItems.size()>=setIndex)
    return 0;

  return (DItems[setIndex].size()<reqItems) ? 0 : 1;
      
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
  
  if (DItems.size()>=setIndex)
    throw ColErr::IndexError<size_t>(setIndex,DItems.size(),"setIndex");
  if (DItems[setIndex].size()>=itemIndex)
    throw ColErr::IndexError<size_t>
      (itemIndex,DItems[setIndex].size(),"itemIndex");

  T ObjValue;
  if (!StrFunc::convert(DItems[setIndex][itemIndex],ObjValue))
    throw ColErr::TypeMatch(DItems[setIndex][itemIndex],
				  typeid(T).name(),"convert error");

  return ObjValue;
      
}


void
IItem::writeSet(std::ostream& OX,const size_t setIndex) const
  /*!
    Complex functiion to convert the system into a string
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("IItem","writeSet");

  if (DItems.size()>=setIndex)
    throw ColErr::IndexError<size_t>(setIndex,DItems.size(),"setIndex");
  
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

  for(const std::vector<std::string>& VItem : DItems)
    {
      for(const std::string& Item : VItem)
	OX<<Item<<" ";
      OX<<"\n";
    }
  return;
}


///\cond TEMPLATE

// template class IItem<long int>;
// template class IItem<unsigned int>;
// template class IItem<size_t>;
// template class IItem<double>;
// template class IItem<std::string>;
// template class IItem<Geometry::Vec3D>;

///\endcond TEMPLATE
 
}  // NAMESPACE mainSystem


