/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystal/CifItem.cxx
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
#include <map>
#include <vector>

#include "Exception.h"
#include "support.h"
#include "CifItem.h"

namespace Crystal
{

CifItem::CifItem() :
  status(0)
  /*!
    CifItem Constructor
  */
{}

CifItem::CifItem(const CifItem& A) :
  status(A.status),Key(A.Key),
  Data(A.Data)
  /*!
    CifItem Copy Constructor
    \param A :: CifItem 
  */
{}

CifItem&
CifItem::operator=(const CifItem& A)
  /*!
    Assignment operator
    \param A :: CifItem
    \return *this
  */
{
  if (this!=&A)
    {
      status=A.status;
      Key=A.Key;
      Data=A.Data;
    }
  return *this;
}

void
CifItem::clearAll()
  /*!
    Clear and reset status
  */
{
  status=0;
  Key.clear(); 
  Data.clear(); 
  return;
}

bool
CifItem::hasKey(const std::string& keyName) const
  /*!
    Determine if the current loop has a particular key
    Note the check on status.
    \param keyName :: Key to test
    \return true/false on found/not found
  */
{
  return  (status && keyName==Key) ? 1 : 0;
}  

template<typename T>
int
CifItem::getItem(T& A)
  /*!
    Process the output data type
    \param A :: Item ot write to 
    \return 1 on success  and 0 on failure
   */
{
  const int retVal=StrFunc::section(Data,A);
  if (retVal) status=0;
  return retVal;
}

template<>
int
CifItem::getItem(double& A)
  /*!
    Process the output data type
    \param A :: Item ot write to 
    \return 1 on success  and 0 on failure
   */
{
  const int retVal=StrFunc::sectPartNum(Data,A);
  if (retVal)
    status=0;
  return retVal;
}

template<>
int
CifItem::getItem(std::string& A)
  /*!
    Process the output data type
    \param A :: Item ot write to 
    \return 1 on success  and 0 on failure
   */
{
  if (!Data.empty())
    {
      A=Data;
      status=0;
      return 0;
    }
  return 1;
}


int
CifItem::registerLine(std::string& Line)
  /*!
    Given a line item determine if loop action required
    \param Line :: Key item to add to list
    \retval 0 :: Incomplete
    \retval 1 :: Finished
  */
{
  if (Line.empty())
    return (status==2) ? 1 : 0;

  if (status==2 || status==0)
    {
      std::string Item;
      if (StrFunc::section(Line,Item) && 
	  (Item[0]=='_') )
        {
	  Key=Item;
	  status=1;
	}
      else
	return 1;
    }
  // Now process Item:
  if (StrFunc::quoteBlock(Line,Data)) 
    {
      status=2;
      return 1;
    }
  return 0;
}

///\cond TEMPLATE
template int CifItem::getItem(int&);
///\endcond TEMPLATE

}  // NAMESPACE Crystal
