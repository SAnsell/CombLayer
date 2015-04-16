/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystal/loopItem.cxx
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
#include <iostream>
#include <string>
#include <vector>

#include "Exception.h"
#include "support.h"
#include "loopItem.h"

namespace Crystal
{

std::ostream&
operator<<(std::ostream& OX,const loopItem& A)
  /*!
    Write out the item component
    \param OX :: output stream
    \param A :: item to write
    \return Stream 
   */
{
  A.write(OX);
  return OX;
}
  
loopItem::loopItem(const size_t N) :
  cIndex(0),Items(N)
  /*!
    Constructor 
    \param N :: Size of Item list
  */
{}

loopItem::loopItem(const loopItem& A) :
  cIndex(A.cIndex),Items(A.Items)
  /*!
    Copy Constructor 
    \param A :: loopItem to copy
  */
{}

loopItem&
loopItem::operator=(const loopItem& A) 
  /*!
    Assignment operator
    \param A :: loopItem to copy
    \return *thisx
  */
{
  if (this!=&A)
   {
     cIndex=A.cIndex;
     Items=A.Items;
   }
  return *this;
}

void
loopItem::addItem(const size_t Index,const std::string& Obj) 
  /*!
    Add an item to a specific place in the list
    \param Index :: Index value 
    \param Obj :: Item to add
   */
{
  if (Index>=Items.size())
    throw ColErr::IndexError<size_t>(Index,Items.size(),"loopItem::addItem");
  Items[Index]=Obj;
  return;
}

int
loopItem::addNext(const std::string& Obj) 
  /*!
    Append an item to the list
    \param Obj :: Item to add
    \return 1 if a object full / 0 otherwize
   */
{
  if (cIndex==Items.size())
    throw ColErr::IndexError<size_t>(cIndex,Items.size(),"loopItem::addNext");
  Items[cIndex]=Obj;
  cIndex++;
  return (cIndex==Items.size()) ? 1 : 0;
}

const std::string&
loopItem::getItemStr(const size_t Index) const
  /*!
    Get a specific item
    \param Index :: Index Item to get
    \return String found
  */
{
  if ( Index>=Items.size() )
    throw ColErr::IndexError<size_t>(Index,Items.size(),
				     "loopItem::getItemStr");
  return Items[Index];
}

template<typename T>
int
loopItem::getItem(const size_t Index,T& Value) const
  /*!
    Get a specific item and convert
    \param Index :: Index Item to get
    \param Value :: Out value
    \return 1 on succes
  */
{
  if (Index >= Items.size())
    throw ColErr::IndexError<size_t>(Index,Items.size(),"loopItem::getItem");
  
  return StrFunc::convert(Items[Index],Value);
}

template<>
int
loopItem::getItem(const size_t Index,double& Value) const
  /*!
    Get a specific item and convert
    \param Index :: Index Item to get
    \param Value :: Out value
    \return 1 on succes
  */
{
  if (Index>=Items.size())
    throw ColErr::IndexError<size_t>(Index,Items.size(),"loopItem::getItem");
  
  std::string C=Items[Index];
  return StrFunc::sectPartNum(C,Value);
}

template<>
int
loopItem::getItem(const size_t Index,std::string& Value) const
  /*!
    Get a specific item and convert
    \param Index :: Index Item to get
    \param Value :: Out value
    \return 1 on succes
  */
{
  if (Index>=Items.size())
    throw ColErr::IndexError<size_t>(Index,Items.size(),"loopItem::getItem");
  
  Value=Items[Index];
  return 1;
}

void
loopItem::write(std::ostream& OX) const
  /*!
    Write out the loop item component.
    Note the care used to add quotes back 
    if the string holds difficult characters.
    \param OX :: Output stream
   */
{
  std::vector<std::string>::const_iterator vc;
  for(vc=Items.begin();vc!=Items.end();vc++)
    {
      if (vc->find_first_of(" \t\'\"")!=std::string::npos)
	OX<<"\'"<<*vc<<"\'";
      else 
	OX<<*vc;
      OX<<" ";
    }
  return;
}

///\cond TEMPLATE
template int loopItem::getItem(const size_t,int&) const;
///\endcond TEMPLATE

} // NAMESPACE Crystal
