/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xml/XMLread.cxx
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
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "XMLload.h"
#include "XMLnamespace.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLread.h"

namespace XML
{

// --------------------------------------------------------
//                   XMLread
// --------------------------------------------------------


XMLread::XMLread(XMLobject* B,const std::string& K) :
  XMLobject(B,K)
  /*!
    Constructor with junk key (value is NOT set)
    \param B :: Parent pointer
    \param K :: key
  */
{}

XMLread::XMLread(XMLobject* B,const std::string& K,
		 const std::vector<std::string>& V) :
  XMLobject(B,K)
  /*!
    Constructor with Key and Value
    \param B :: Parent pointer
    \param K :: key
    \param V :: Value to set
  */
{
  setObject(V);
}

XMLread::XMLread(const XMLread& A) :
  XMLobject(A),Comp(A.Comp)
  /*!
    Standard copy constructor
    \param A :: XMLread to copy
  */
{}

XMLread&
XMLread::operator=(const XMLread& A) 
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

XMLread*
XMLread::clone() const
  /*!
    The clone function
    \return new copy of this
  */
{
  return new XMLread(*this);
}

XMLread::~XMLread()
  /*!
    Standard destructor
  */
{ }

void 
XMLread::addLine(const std::string& Line)
  /*!
    Adds a line to the system
    \param Line :: Line to add
  */
{
  Comp+=Line;
  Comp+=' ';
  empty=0;
  return;
}


template<template<typename T,typename A> class V,typename T,typename A> 
int
XMLread::convertToContainer(V<T,A>& CT) const
  /*!
    Given a container of type V<T>, convert all of the
    lines to single objects in the system
    \param CT :: Container to fill.
    \retval 0 on failure
    \retval 1 on success
  */
{
  CT.erase(CT.begin(),CT.end());
  std::string Line = Comp;
  T tmpObj;
  while(StrFunc::section(Line,tmpObj))
    CT.push_back(tmpObj);
  // Clear points to object.
  return (CT.empty()) ? 1 : 0;
}

template<template<typename T,typename A> class V,typename T,typename A> 
int
XMLread::convertToContainer(const int dmp,V<T,A>& CA,V<T,A>& CB) const
  /*!
    Given a container of type V<T>, convert all of the
    lines to single objects in the system
    \param dmp :: Placeholder for dumped item
    \param CA :: Container to fill.
    \param CB :: Container to fill.
    \return 1 on success and 0 on failure
  */
{
  CA.erase(CA.begin(),CA.end());
  CB.erase(CB.begin(),CB.end());

  int dflag(0);
  int item(0);   
  std::string Line = Comp;
  T tmpObj;
  while(StrFunc::section(Line,tmpObj))
    {
      if (dflag!=dmp) // not dump point
        {
	  if (!item)
	    CA.push_back(tmpObj);
	  else
	    CB.push_back(tmpObj);
	  item=1-item;
	}
      dflag++;
      dflag %= 3;
    }
  if (CA.size()!=CB.size())
    CB.push_back(CA.back());
  
  // Clear points to object.
  return (CB.empty()) ? 0 : 1;
}

template<typename T> 
int
XMLread::convertToObject(T& OT) const
  /*!
    Convert the XMLread string into an object
    \param OT :: Object to fill.
    \retval 1 :: success
    \retval 0 :: failure
  */
{
  return StrFunc::convert(Comp,OT);
}

template<typename T,int S> 
int
XMLread::convertToArray(boost::multi_array<T,S>& A) const
  /*!
    Convert the XMLread string into an multi_array. 
    S is the number of dimensions. This needs to be iterative
    on the template parameter S.

    \param A :: Object to fill.
    \retval 1 :: success
    \retval 0 :: failure
    \todo THIS SHOULD NOT WORK
  */
{
  std::string Line=Comp;
  typename boost::multi_array<T,S>::iterator ac;
  for(ac=A.begin();ac!=A.end();ac++)
    if (!StrFunc::section(Line,*ac))
      return 0;
  return 1;
}

void
XMLread::setObject(const std::vector<std::string>& V) 
  /*!
    Sets all the objects based on a vector
    \param V :: Lines of string to add
   */
{
  std::vector<std::string>::const_iterator vc;
  for(vc=V.begin();vc!=V.end();vc++)
    {
      if (!vc->empty())
        {
	  Comp+=*vc;
	  Comp+=" ";
	}
    }
  empty= Comp.empty() ? 1 : 0;
  return;
}

void
XMLread::setObject(const std::string& V) 
  /*!
    Sets all the objects based on a vector
    \param V :: Line of string to add
   */
{
  Comp=V;
  empty= (!V.empty()) ? 0 : 1;
  return;
}

void
XMLread::writeXML(std::ostream& OX) const
  /*!
    Write out the object. This code is specialised
    to allow multiple "identical" group component names.
    The % tag field is used to keep "real" items apart.
    \param OX :: output stream
  */
{
  writeDepth(OX);
  std::string::size_type pos=Key.find('%');
  std::string KeyOut( (pos!=std::string::npos)  ? Key.substr(0,pos) : Key);
  if (empty)
    {
      OX<<"<"<<KeyOut<<Attr<<"/>"<<std::endl;
      return;
    }      

  // Determin how long Comp is relative to item:
  if (Comp.size()>72 || Comp.find('\n')!=std::string::npos)
    {
      OX<<"<"<<KeyOut<<Attr<<">"<<std::endl;
      writeDepth(OX);
      OX<<"  ";
      StrFunc::writeControl(Comp,OX,72,depth+2);
      writeDepth(OX);
    }
  else
    {
      OX<<"<"<<KeyOut<<Attr<<">";
      OX<<Comp;
    }
  OX<<"</"<<KeyOut<<">"<<std::endl;
  return;
}


///\cond TEMPLATE

// template int XMLread::convertToObject(boost::multi_araray<double,2>&) const;
template int XMLread::convertToObject(Geometry::Vec3D&) const;
template int XMLread::convertToContainer(std::vector<int>&) const;
template int XMLread::convertToContainer(std::vector<double>&) const;
template int XMLread::convertToContainer(const int,std::vector<double>&,
					 std::vector<double>&) const;


///\endcond TEMPLATE
  
};


