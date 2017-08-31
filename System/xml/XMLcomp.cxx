/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xml/XMLcomp.cxx
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
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "support.h"
#include "XMLload.h"
#include "XMLnamespace.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLcomp.h"

namespace XML
{

// --------------------------------------------------------
//                   XMLcomp
// --------------------------------------------------------


template<typename T>
XMLcomp<T>::XMLcomp(XMLobject* B,const std::string& K) :
  XMLobject(B,K),Value()
  /*!
    Constructor with junk key (value is NOT set)
    \param B :: Parent object
    \param K :: key
  */
{}

template<typename T>
XMLcomp<T>::XMLcomp(XMLobject* B,const std::string& K,const T& V) :
  XMLobject(B,K),Value(V)
  /*!
    Constructor with Key and Value
    \param B :: Parent object
    \param K :: key
    \param V :: Value to set
  */
{
  setEmpty(0);
}

template<typename T>
XMLcomp<T>::XMLcomp(const XMLcomp<T>& A) :
  XMLobject(A),Value(A.Value)
  /*!
    Standard copy constructor
    \param A :: XMLcomp to copy
  */
{}

template<typename T>
XMLcomp<T>&
XMLcomp<T>::operator=(const XMLcomp<T>& A) 
  /*!
    Standard assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      XMLobject::operator=(A);
      Value=A.Value;
    }
  return *this;
}


template<typename T>
XMLcomp<T>*
XMLcomp<T>::clone() const
  /*!
    The clone function
    \return new copy of this
  */
{
  return new XMLcomp<T>(*this);
}

template<typename T>
XMLcomp<T>::~XMLcomp()
  /*!
    Standard destructor
  */
{ }

template<typename T>
void
XMLcomp<T>::writeXML(std::ostream& OX) const
  /*!
    Write out the object
    \param OX :: output stream
  */
{
  writeDepth(OX);
  std::string::size_type pos=Key.find('%');
  const std::string KeyOut=Key.substr(0,pos);

  if (isEmpty())
    {
      OX<<"<"<<KeyOut<<Attr<<"/>"<<std::endl;
      return;
    }
  OX<<"<"<<KeyOut<<Attr<<">";
  OX<<Value;
  OX<<"</"<<KeyOut<<">"<<std::endl;
  return;
}

template<>
void
XMLcomp<bool>::writeXML(std::ostream& OX) const 
  /*!
    Write out the object
    \param OX :: output stream
  */
{
  writeDepth(OX);
  std::string::size_type pos=Key.find('%');
  const std::string KeyOut=Key.substr(0,pos);
  if (isEmpty())
    {
      OX<<"<"<<KeyOut<<Attr<<"/>"<<std::endl;
      return;
    }
  OX<<"<"<<KeyOut<<Attr<<">";
  OX<<((Value) ? 1 : 0);
  OX<<"</"<<KeyOut<<">"<<std::endl;
  return;
}

template<>
void
XMLcomp<std::string>::writeXML(std::ostream& OX) const
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
  if (isEmpty())
    {
      OX<<"<"<<KeyOut<<"/>"<<std::endl;
      return;
    }      

  OX<<"<"<<KeyOut<<Attr<<">";
  // Remove difficult to parse symbols:
  OX<<XML::procString(Value);

  OX<<"</"<<KeyOut<<">"<<std::endl;
  return;
}

#ifdef SpecDataHold_h
template<>
void
XMLcomp<TimeData::SpecDataHold>::writeXML(std::ostream& OX) const
  /*!
    Write out a SpecDataHold XML object 
    - This is specialised since it is necessary
    to write out the depth.
    \param OX :: output stream
  */
{
  writeDepth(OX);
  std::string::size_type pos=Key.find('%');
  const std::string KeyOut=Key.substr(0,pos);
  if (isEmpty())
    {
      OX<<"<"<<KeyOut<<"/>"<<std::endl;
      return;
    } 

  OX<<"<"<<KeyOut<<Attr<<">"<<std::endl;
  Value.write(OX,this->depth+2);
  OX<<"</"<<KeyOut<<">"<<std::endl;
  return;
}
#endif

template<>
void
XMLcomp<nullObj>::writeXML(std::ostream& OX) const
  /*!
    Write out a null XML object (\</Key\>)
    \param OX :: output stream
  */
{
  writeDepth(OX);
  std::string::size_type pos=Key.find('%');
  std::string KeyOut( (pos!=std::string::npos)  ? Key.substr(0,pos) : Key);
  OX<<"<"<<KeyOut<<Attr<<"/>"<<std::endl;
  return;
}

template<>
void
XMLcomp<tm*>::writeXML(std::ostream& OX) const
  /*!
    Write out a time XML object (\</Key\>)
    \param OX :: output stream
    \todo :: SPECIAL FOR DATE
  */
{
  writeDepth(OX);
  std::string::size_type pos=Key.find('%');
  std::string KeyOut( (pos!=std::string::npos)  ? Key.substr(0,pos) : Key);
  OX<<"<"<<KeyOut<<"/>"<<std::endl;
  return;
}
  
}   // NAMESPACE 

/// \cond TEMPLATE

template class XML::XMLcomp<double>;
template class XML::XMLcomp<std::string>;
template class XML::XMLcomp<int>;
template class XML::XMLcomp<long int>;
template class XML::XMLcomp<unsigned int>;
template class XML::XMLcomp<size_t>;
template class XML::XMLcomp<bool>;
template class XML::XMLcomp<Geometry::Vec3D>;
template class XML::XMLcomp<Geometry::Quaternion>;

//template class XML::XMLcomp<std::vector<double> >;
// template class XML::XMLcomp<std::vector<int> >;
//template class XML::XMLcomp<std::vector<std::string> >;
template class XML::XMLcomp<XML::nullObj>;

#ifdef SpecDataHold_h
// template class XML::XMLcomp<tm*>;
template class XML::XMLcomp<TimeData::SpecDataHold>;
#endif

/// \endcond TEMPLATE
