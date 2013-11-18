/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xml/XMLcomment.cxx
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
#include <vector>
#include <list>
#include <map>
#include <string>
#include <sstream>

#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "XMLload.h"
#include "XMLnamespace.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLcomment.h"


namespace XML
{

// --------------------------------------------------------
//                   XMLcomment
// --------------------------------------------------------


XMLcomment::XMLcomment(XMLobject* B,const std::string& K,
		       const std::string& Line) :
  XMLobject(B,K)
  /*!
    Constructor with junk key (value is NOT set)
    \param B :: Parent Object
    \param K :: key
    \param Line :: Line to add
  */
{
  addLine(Line);
}

XMLcomment::XMLcomment(XMLobject* B,const std::string& Line) :
  XMLobject(B,"comment")
  /*!
    Constructor with junk key (value is NOT set)
    \param B :: Parent Object
    \param Line :: Line to add
  */
{
  addLine(Line);
}

XMLcomment::XMLcomment(XMLobject* B,const std::string& K,
		 const std::vector<std::string>& V) :
  XMLobject(B,K)
  /*!
    Constructor with Key and Value
    \param B :: Parent Object
    \param K :: key
    \param V :: Values to set
  */
{
  Comp.resize(V.size());
  copy(V.begin(),V.end(),Comp.begin());
  setEmpty(0);
}

XMLcomment::XMLcomment(XMLobject* B,const std::vector<std::string>& V) :
  XMLobject(B,"comment")
  /*!
    Constructor with Key and Value
    \param B :: Parent Object
    \param V :: Value to set
  */
{
  Comp.resize(V.size());
  copy(V.begin(),V.end(),Comp.begin());
  setEmpty(0);
}

XMLcomment::XMLcomment(const XMLcomment& A) :
  XMLobject(A),Comp(A.Comp)
  /*!
    Standard copy constructor
    \param A :: XMLcomment to copy
  */
{}

XMLcomment&
XMLcomment::operator=(const XMLcomment& A) 
  /*!
    Standard assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      XMLobject::operator=(A);
      Comp=A.Comp;
    }
  return *this;
}

XMLcomment*
XMLcomment::clone() const
  /*!
    The clone function
    \return new copy of this
  */
{
  return new XMLcomment(*this);
}

XMLcomment::~XMLcomment()
  /*!
    Standard destructor
  */
{ }

std::string&
XMLcomment::getFront()
  /*!
    Gets the first item in the stack
    \retval Front object 
    \throw ExBasy if stack empty
  */
{
  if (Comp.empty())
    throw ColErr::ExBase("XMLcomment::getFront : Stack empty");
  return Comp.front();
}

const std::string&
XMLcomment::getFront() const
  /*!
    Gets the first item in the stack
    \retval Front object 
    \throw ExBase if stack empty
  */
{
  if (Comp.empty())
    throw ColErr::ExBase("XMLcomment::getFront : Stack empty");
  
  return Comp.front();
}

void 
XMLcomment::addLine(const std::string& Line)
  /*!
    Adds a line to the system
    \param Line :: Line to add
  */
{
  Comp.push_back(Line);
  setEmpty(0);
  return;
}

void
XMLcomment::setObject(const std::vector<std::string>& V) 
  /*!
    Sets all the objects based on a vector
    \param V :: Lines of string to add
   */
{
  Comp.clear();

  if (!V.empty())
    {
      Comp.resize(V.size());
      copy(V.begin(),V.end(),Comp.begin());
      setEmpty(0);
    }
  else
    setEmpty(1);
  return;
}

int
XMLcomment::pop()
  /*!
    Takes the front item of the list
    \retval 0 :: Items still left
    \retval 1 :: Last item
  */
{
  Comp.pop_front();
  return (Comp.empty()) ? 1 : 0;
}

void
XMLcomment::writeXML(std::ostream& OX) const
  /*!
    Write out the object. This code is specialised
    to allow multiple "identical" group component names.
    \param OX :: output stream
  */
{
  if (isEmpty() || Comp.empty())
    return;
  writeDepth(OX);
  OX<<"<!--";
  // Remove difficult to parse symbols:
  CStore::const_iterator vc=Comp.begin();
  OX<<XML::procString(*vc);

  for(vc++;vc!=Comp.end();vc++)
    {
      OX<<std::endl;
      writeDepth(OX);
      OX<<"   ";
      OX<<XML::procString(*vc);
    }

  OX<<"-->"<<std::endl;
  return;
}
  
}   // NAMESPACE XML


