/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xml/XMLattribute.cxx
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
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <iterator>

#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "support.h" 
#include "XMLload.h"
#include "XMLnamespace.h"
#include "XMLattribute.h"


namespace XML
{

std::ostream& 
operator<<(std::ostream& OX,const XMLattribute& A)
  /*!
    Calls XMLattribute method write to output class
    \param OX :: Output stream
    \param A :: XMLattribute to write
    \return Current state of stream
  */
{
  A.writeXML(OX);
  return OX;
}

// --------------------------------------------------------
//                   XMLattribute
// --------------------------------------------------------

XMLattribute::XMLattribute() :
  empty(1)
  /*!
    Constructor
  */
{}

XMLattribute::XMLattribute(const std::string& A,
			   const std::string& B) :
  empty(0)
  /*!
    Constructor (with single attribute)
    \param A :: Name of unit
    \param B :: Values
  */
{
  AObj.push_back(A);
  Val.push_back(B);
}

XMLattribute::XMLattribute(const XMLattribute& A) :
  empty(A.empty),AObj(A.AObj),Val(A.Val)
  /*!
    Copy Constructor
    \param A :: XMLattribute to copy
  */
{}

XMLattribute&
XMLattribute::operator=(const XMLattribute& A) 
  /*!
    Assignment operator
    \param A :: XMLattribute to copy
    \return *this
  */
{
  if (this!=&A)
    {
      empty=A.empty;
      AObj=A.AObj;
      Val=A.Val;
    }
  return *this;
}

XMLattribute::~XMLattribute()
  /*!
    Destructor
  */
{}

void
XMLattribute::addAttribute(const std::string& Name,
			   const std::string& V)
  /*!
    Adds a value to the attributions. Clears
    the empty flag.
    \param Name :: Unit name
    \param V :: Attribute Value
  */

{
  empty=0;
  AObj.push_back(Name);
  Val.push_back(V);
  return;
}

void
XMLattribute::addAttribute(const std::vector<std::string>& Vec)
  /*!
    Adds an attriubte to the main system.
    \param Vec :: list of string compents in the form key="value"
   */
    
{
  std::string Key,Atb;
  std::vector<std::string>::const_iterator vc;
  for(vc=Vec.begin();vc!=Vec.end();vc++)
    {
      std::string Part=*vc;
      if (splitAttribute(Part,Key,Atb)>0)
	addAttribute(Key,Atb);
    }
  return;
}

int
XMLattribute::setAttribute(const std::string& Key,const std::string& Value)
  /*!
    Set a given attribute 
    \param Key :: Key name
    \param Value :: Value of attribute
    \retval 0 :: success
    \retval 1 :: Attribute does not exists (but now added)
   */
{
  const size_t item=hasAttribute(Key);
  if (!item)
    {
      addAttribute(Key,Value);
      return 1;
    }
  
  Val[item-1]=Value;
  return 0;
}

size_t
XMLattribute::hasAttribute(const std::string& Name) const
  /*!
    Determines if an attribute exists 
    \param Name :: Attribute to find
    \retval 0 :: No Attribute found
    \retval Index+1 :: Attribute of index
   */
{
  if (empty) return 0;

  std::vector<std::string>::const_iterator vc;
  vc=find(AObj.begin(),AObj.end(),Name);
  if (vc!=AObj.end())
    return 1+static_cast<size_t>(distance(AObj.begin(),vc));
  return 0;
}

std::string
XMLattribute::getAttribute(const std::string& Name) const
  /*!
    Determines if an attribute exists and return value
    \param Name :: Attribute to find
    \return Value object 
   */

{
  const size_t Index=hasAttribute(Name);
  if (Index)
    return Val[Index-1];
  return "";
}

std::pair<std::string,std::string>
XMLattribute::getComp(const size_t Index) const
  /*!
    Get a single component out of the system
    \param Index :: index to use
    \throw IndexError if Index out of range
    \return pair of Key:Value
   */
{
  if (empty || Index>=this->getNum())
    throw ColErr::IndexError<size_t>(Index,this->getNum(),
				     "XMLattribute::getComp");
  return std::pair<std::string,std::string>(AObj[Index],Val[Index]);
}

void
XMLattribute::writeXML(std::ostream& OX) const
  /*!
    Write out to the stream
    \param OX :: Output stream
   */
{
  if (!empty)
    {
      for(size_t i=0;i<AObj.size();i++)
	OX<<" "<<AObj[i]<<"=\""<<Val[i]<<"\"";
    }
  return;
}

}  //NAMESPACE XML

